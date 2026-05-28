/*******************************************************************************
PURPOSE:
   (Dynamic Mass Group methods)

PROGRAMMERS:
   (((Gary Turner) (OSR) (March 2014)
                       (New implementation of dynamic mass for JEOD 2.x))
    ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
   )
*******************************************************************************/
#include <cstring> // NULL
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/dynamic_mass_group.hh"

/*******************************************************************************
Method: DynamicMassGroup
Purpose: (Constructor)
*******************************************************************************/
DynamicMassGroup::DynamicMassGroup()
:
  delta_mass_is_redistributed(false),
  name("unnamed"),
  countdown_reset(100),
  flow_warn_msg_shown(false),
  needs_tree_update(true), // Forces the full update, including strings,
                           // at initialization
  total_mass(0.0),
  total_initial_mass(0.0),
  total_consumed_mass(0.0),
  countdown_to_root_test(0),
  num_dyn_masses(0),
  root_body_ptr(NULL)
{
  subscribe_name = "DynamicMassGroup:";
}

/*******************************************************************************
Method:  (add_mass_to_group)
Purpose: (Adds a DynamicMassBody to a group.)
*******************************************************************************/
void DynamicMassGroup::add_mass_to_group(
    DynamicMassBody *mass)
{
  add_mass_to_group_internal(mass, true);
}

/*******************************************************************************
Method:  (add_string_to_group)
Purpose: (Adds a DynamicMassString to a group)
*******************************************************************************/
void DynamicMassGroup::add_string_to_group(
    DynamicMassString * new_mass_string)
{
  if (new_mass_string == NULL) {
    CMLMessage::fail (
      __FILE__, __LINE__, "Setup error.\n",
      "A NULL pointer was given to the add_string_to_group (", name, ") function.\n"
      "This is not allowed.\n"
      "Unsure how to proceed, terminating for safety.\n");
  }

  // check uniqueness to avoid inadvertent double-counting
  for (auto it = mass_strings.begin(); it != mass_strings.end(); ++it) {
    if (new_mass_string == *it) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid object addition\n",
        "Attempt to add a DynamicMassString that is already assigned\n"
        "to this group (", name, "). \n"
        "Cannot duplicate a mass-string in a single group.\n");
      return;
    }
  }

  // new_mass_string is unique in this group, add it.
  mass_strings.push_back(new_mass_string);

  // if group has already been initialized, be sure to initialize this string.
  if (initialized) {
    new_mass_string->initialize();
  }

  // Make sure that all of the body elements from this string are added
  // to the group.
  std::list<DynamicMassBody *> & string_bodies =
                                         new_mass_string->get_body_collection();

  for (auto it = string_bodies.begin(); it != string_bodies.end(); ++it) {
    // add each mass in the new_mass_string to the group, but don't send an
    // error message on any that have already been added, just skip over them.
    add_mass_to_group_internal( *it, false);
  }

  // inform the new_mass_string that it has been added:
  new_mass_string->set_string_in_group();
}

/*******************************************************************************
Method:  (initialize)
Purpose: (Initializes all the masses already added to the group.)
*******************************************************************************/
void DynamicMassGroup::initialize()
{
  if (!is_enabled()) {
    num_dyn_masses = 0;
    CMLMessage::inform (
      __FILE__, __LINE__, "initialize error.\n",
      "The DynamicMassGroup (", name, ") is not enabled, so cannot be initialized\n.");
    return;
  }
  // check that the group has contents:
  num_dyn_masses = dyn_masses.size();
  if (num_dyn_masses == 0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Initialization error.\n",
      "There have been no masses added to group with name ", name, ".\n"
      "It cannot be initialized.\n");
    return;
  }

  // run test_root_body() first because it's a quick sanity check
  test_root_body();

  // initialize the masses
  for (unsigned int ii = 0; ii < num_dyn_masses; ++ii) {
    // It is possible that this initialization has already been done,
    // especially if the model is being re-initialized.
    // Because reinitialization will generate a warning message, check for
    // initialization status before attempting.
    // Note - while the overall group and Dynamic MassStrings may need
    //        reinitializing to accommodate configuration changes, individual
    //        DynamicMassBody instances do not need reinitializing,
    if (!dyn_masses[ii]->get_dyn_mass_initialized()) {
      dyn_masses[ii]->initialize_dyn_mass();
    }
  }

  // initialize the strings
  for (auto it = mass_strings.begin(); it != mass_strings.end(); ++it) {
    (*it)->initialize();
  }

  // execute update_group_mass() to obtain initial model outputs
  // and sync the initial overall dynamic-body mass with the mass
  // of its components.  Note that this method will only run if
  // the model is active, and it would usually be inactive at this
  // point because initialization is not complete.  So a temporary
  // override of the active flag is required.
  const bool active_local = active;
  active = true;
  update_group_mass();
  active = active_local;

  // override total initial_mass and total_consumed_mass values
  // because:
  // - this is where total_initial_mass gets defined;
  // - total_consumed_mass is a function of total_initial_mass.
  total_consumed_mass = 0.0;
  total_initial_mass = total_mass;

  // initialization now complete, forward up the chain.
  SubscriptionBase::initialize();
  subscribe_name = name;
}

/*******************************************************************************
Method:  (update_group_mass)
Purpose: (Updates the mass of all dynamic masses in the group,
          and updates the tree)
*******************************************************************************/
void DynamicMassGroup::update_group_mass()
{
  // Do nothing if not activated.
  if (!active) {
     return;
  }

  // Distribute the mass depletion assigned to a tank-string to the respective
  // tanks.  Zero out the mass depletion assigned to the string.
  for (auto it = mass_strings.begin(); it != mass_strings.end(); ++it) {
    (*it)->distribute_mass_consumption();
  }

  // Cycle through each body, checking it for any mass-consumed-step values
  // and applying them individually.
  // Overall, at the group level, the total mass can be incremented from each
  // body, and ia record kept of whether *any* mass has changed; if it has,
  // then the mass tree needs updating all the way to the root-body. 
  total_mass = 0.0;
  // Update each dynamic mass.
  for (auto it = dyn_masses.begin(); it != dyn_masses.end(); ++it) {

    // using a temporary variable here to ensure that update_mass() gets
    // called without possibility of it being blocked by needs_tree_update if
    // put in as a direct component of the OR statement.
    bool mass_change = (*it)->update_mass();
    needs_tree_update = needs_tree_update || mass_change;
    total_mass += (*it)->core_properties.mass;
  }
  total_consumed_mass = total_initial_mass - total_mass;

  // if any masses changed, update the strings, and update the mass-tree.
  // As part of the DynamicMassBody::update_mass() method, all bodies
  // superior to any body with a mass-change will have been marked as needing
  // an update.  So we can start at the root-body and go through the entire
  // tree, updating mass properties on anything that has been marked.
  if (needs_tree_update) {
    for (auto it = mass_strings.begin(); it != mass_strings.end(); ++it) {
      (*it)->update();
    }

    // Verify that the root-body-of-record is still valid.  This is
    // accomplished in 2 checks:
    // 1. at every step, verify the root body of the first mass-body
    //    in the group.  Unless the group has fractured, this is sufficient.
    //    If it fails, test every element in the group to ensure cohesion.
    // 2. Periodically, verify for every body in the group.
    // For efficiency, look for the second test first.
    if ((countdown_to_root_test == 0) ||
        (root_body_ptr != dyn_masses[0]->get_root_body())) {
      test_root_body();
    }
    if (countdown_to_root_test > 0) {
      -- countdown_to_root_test;
    }
    // else assume root body is unchanged for all dynamic masses in the group.


    // Update the entire mass tree (from the root, the update will
    // automatically descend the tree updating all sub-bodies that are
    // so flagged.
    root_body_ptr->update_mass_properties();

    //   If the delta-mass is simply redistributed, the inertial motion of
    // the center-of-mass  of the vehicle as a whole (i.e. the root-body
    // composite-body state) is conserved.  The state of the center-of-mass
    // within the vehicle will change, but the effect there is that the
    // structural frame state will respond on top of the inertial body-frame
    // motion.
    //   If the mass is lost (or gained) from an external source with no
    // associated force, the structural-frame state will not be affected.
    // The state-change of the center-of-mass within the structural frame
    // must be accommodated, resulting in un-forced motion of the
    // center-of-mass.  This is physically valid.
    //   Where the mass is lost externally with an associated force, there is
    // a two-step process.  The velocity of the center-of-mass resulting from
    // the mass-distribution evolution is simplified to a step-function, and
    // modeled as discrete steps linked to discrete mass steps, as for the case
    // of mass loss with no associated force.
    // The force is then applied to the body frame; this is equivalent to
    // assuming the net force to be zero in test-stand firings even while the
    // center-of-mass is accelerating.
    // The delta_mass_is_redistributed flag controls the determination of
    // which frame is held, and which frame is forced to move in response.
    // True - hold body-frame, adjust structural frame.
    // False (default) - hold structural-frame, adjust body-frame.

    // And, of course, if this is a simple MassBody with no state, the whole
    // distinction is moot.
    if (!delta_mass_is_redistributed) {
      jeod::DynBody * root_dyn_body_ptr = root_body_ptr->dyn_owner;
      if (root_dyn_body_ptr != NULL) { // root is a DynBody
        root_dyn_body_ptr->set_state_source (
                                          jeod::RefFrameItems::Pos_Vel_Att_Rate,
                                          root_dyn_body_ptr->structure);
        root_dyn_body_ptr->propagate_state ();
      }
      // else - root body is not a dyn body, it has no state so there is nothing
      // to update.  Just leave it alone.
    }
  }
  // reset needs_tree_update for next cycle.
  needs_tree_update = false;
}

/*******************************************************************************
Method:  (series_flow)
Purpose: (Used for pushing mass from an upstream tank into a downstream tank)
*******************************************************************************/
void
DynamicMassGroup::series_flow(
       unsigned int upstream_ix,
       unsigned int downstream_ix)
{
  // Do nothing if not activated.
  if (!active) {
     if (!flow_warn_msg_shown) {
       CMLMessage::warn(
         __FILE__, __LINE__, "series_flow warning.\n",
         "The DynamicMassGroup (", name, ") is not active so cannot support\n"
         "series-flow calculations.\n");
       flow_warn_msg_shown = true;
     }
     return;
  }
  flow_warn_msg_shown = false;

  // Move mass from upstream tank to downstream tank, as long as there is mass
  // available in the upstream tank.
  double upstream_available = 
        dyn_masses[upstream_ix]->dynamic_properties.consumable_mass -
        dyn_masses[upstream_ix]->dynamic_properties.mass_consumed_step;
  double downstream_demand = 
        dyn_masses[downstream_ix]->dynamic_properties.mass_consumed_step;

  if (upstream_available > downstream_demand) {
    // move all demand to the upstream tank
    dyn_masses[upstream_ix]->dynamic_properties.mass_consumed_step +=
                                                           downstream_demand;
    dyn_masses[downstream_ix]->dynamic_properties.mass_consumed_step = 0.0;
  }
  else {
    // Otherwise, take what is available.
    dyn_masses[upstream_ix]->dynamic_properties.mass_consumed_step =
                 dyn_masses[upstream_ix]->dynamic_properties.consumable_mass;
    dyn_masses[downstream_ix]->dynamic_properties.mass_consumed_step -=
                                                          upstream_available;
  }
}

/*****************************************************************************
initiate_dry_mass_config
Purpose: Removes all consumable mass.
         Allows for an analysis of the dry-mass of a vehicle.
Note: This is quite a dangerous function because it will deplete everything
      in the group.  The intent is that it be used in conjunction with
      revert_dry_mass_config().  However, there has to be a break between
      calling these 2 methods in order to extract useful data from the
      mass-tree.  It is not feasible to anticipate what these useful data
      might be, so the initiate / revert are separate methods provided with the
      understanding that they be used together:
      - mass_group.initiate_dry_mass_config()
      - <perform desired analysis on mass properties>
      - mass_group.revert_dry_mass_config()
      - continue simulation
Note:  Two architectures were considered:
       - Record, adjust and revert only the MassBody properties leaving the
         mass-string properties untouched.  This requires some code duplication
         because update_group_mass cannot be used (it updates the mass-string
         instances).
       - Record, adjust, and revert the MassBody and MassString properties.
         This complicates the maintenance of MassBody objects within a string
         because when the string is exhausted, the bodies are declared
         inactive.
       Elected to go with the 1st architecture.

*****************************************************************************/
void
DynamicMassGroup::initiate_dry_mass_config()
{
  if (!active) {
    return;
  }

  total_mass = 0.0;
  // Set mass-consumed for each dynamic mass.
  for (unsigned int ii = 0; ii < num_dyn_masses; ++ii) {
    // Store the current consumable-mass for reverting to regular operation
    dyn_masses[ii]->initiate_dry_mass_config();
    total_mass += dyn_masses[ii]->core_properties.mass;
  }
  // Need to conduct a root-body check because this is a public method and
  // root_body_ptr could be NULL.
  test_root_body();
  root_body_ptr->update_mass_properties();
}

/*****************************************************************************
revert_dry_mass_config
Purpose:(Puts the consumable mass back to what it should be)
*****************************************************************************/
void
DynamicMassGroup::revert_dry_mass_config()
{
  unsigned int num_error_bodies = 0;
  for (unsigned int ii = 0; ii < num_dyn_masses; ++ii) {
    // Check that all bodies have revertible data recorded.
    // Do not process any bodies that do not have this flag set.
    // Note - anticipate that this should be either all or none, but this
    //        mechanism has given users enough rope to hang themselves, so
    //        I'm going to make sure.
    if (!dyn_masses[ii]->get_dry_mass_revert__available()) {
      num_error_bodies++;
    }
  }
  if (num_error_bodies > 0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid function call\n",
      "Call made to revert the changes made in obtaining a dry-mass "
      "configuration\n"
      "The recovery data is not available for ", num_error_bodies, " of ", num_dyn_masses, " bodies.\n"
      "Keeping current mass configuration for those bodies with no data.\n");
  }

  // Reset the consumable-mass levels on those bodies for which reverting data
  // is available.  Safe to call on all bodies because each body has protection
  // built into its own method so it will not revert to nonexistent data.
  total_mass = 0.0;
  // Set mass-consumed for each dynamic mass.
  for (unsigned int ii = 0; ii < num_dyn_masses; ++ii) {
    dyn_masses[ii]->revert_dry_mass_config();
    total_mass += dyn_masses[ii]->core_properties.mass;
  }
  // Need to conduct a root-body check because this is a public method and
  // root_body_ptr could be NULL.
  test_root_body();
  root_body_ptr->update_mass_properties();
}

/*****************************************************************************
is_body_in_group
Purpose:(Tests whether a specified dynamic-mass-body is in this group.)
*****************************************************************************/
bool
DynamicMassGroup::is_body_in_group(
   DynamicMassBody * mass_body_query)
{
  for (unsigned int ii = 0; ii < dyn_masses.size(); ++ii) {
    if (dyn_masses[ii] == mass_body_query) {
      return true;
    }
  }
  return false;
}
  
/*****************************************************************************
is_string_in_group
Purpose:(Tests whether a specified dynamic-mass-string is in this group.)
*****************************************************************************/
bool
DynamicMassGroup::is_string_in_group(
   DynamicMassString * mass_string_query)
{
  for (auto it = mass_strings.begin(); it != mass_strings.end(); ++it) {
    if (*it == mass_string_query) {
      return true;
    }
  }
  return false;
}

/*******************************************************************************
Method:  (test_root_body)
Purpose: (A mass group must have a common root.  This verifies that.)
*******************************************************************************/
void
DynamicMassGroup::test_root_body()
{
  countdown_to_root_test = countdown_reset;
  if (dyn_masses.size() >=1) {
    // set one root identification for reference
    const jeod::MassBody* first_root = dyn_masses[0]->get_root_body();

    // then check the others against it:
    for (unsigned int ii = 1; ii < num_dyn_masses; ii++) {
      const jeod::MassBody * second_root = dyn_masses[ii]->get_root_body();

      // Fail if the two bodies do not have the same root
      // NOTE - the root of the root-body is itself.
      if (first_root != second_root) {
        CMLMessage::fail (
          __FILE__, __LINE__, "Grouping error.\n",
          "A DynamicMassGroup (", name, ") has split.\n"
          "There are multiple root bodies branching from the same Group.\n"
          "DynamicMassGroups must stay as one block in "
          "order for their updates to work.\n"
          "Body called ", dyn_masses[0]->name, " has a root called ",
          first_root->name, " while\n", "body called ",
          dyn_masses[ii]->name, " has a root called ",
          second_root->name, "");
      }
    }
    root_body_ptr = const_cast<jeod::MassBody*>(first_root);
  }
  // This should be impossible to hit:
  else {
    CMLMessage::fail(
      __FILE__,__LINE__,"Unknown configuration.\n",
      "The DynamicMassGroup has no dynamic masses to manage.");
  }
}

/*****************************************************************************
add_mass_to_group_internal
Purpose: (Adds a DynamicMassBody to a group.
          The send_err_msg is there
          to avoid superfluous error messages when adding a string's
          contents to the group after the user already added them.
          It is true if this is coming fom an external call, and typically
          false if coming fom an internal call.)
*****************************************************************************/
void
DynamicMassGroup::add_mass_to_group_internal(
    DynamicMassBody * mass,
    bool              send_err_msg)
{
  if (mass == NULL) {
    CMLMessage::fail (
      __FILE__, __LINE__, "Setup error.\n",
      "A NULL pointer was given to the add_mass_to_group (", name, ") function.\n"
      "This is not allowed.\n"
      "Unsure how to proceed, terminating for safety.\n");
  }
  // check uniqueness to avoid inadvertent double-counting
  unsigned int num_bodies = dyn_masses.size();
  for (unsigned int ii = 0; ii < num_bodies; ++ii) {
    if (mass == dyn_masses[ii]) {
      if (send_err_msg) {
        CMLMessage::error(
          __FILE__,__LINE__,"Invalid object addition\n",
          "Attempt to add a new DynamicMassBody (", mass->name, ") that is already assigned\n"
          "to this group. \n"
          "Cannot duplicate a mass-body in a single group.\n");
      }
      return;
    }
  }
  // body is unique in this group, add it.
  dyn_masses.push_back(mass);

  // If group has already been initialized, check for common root, and send a
  // warning about late-additions invalidating the initial-mass.
  if (initialized) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Late body addition.\n",
      "Added a Mass-body to a group after initialization.  Values output as\n"
      "total_initial_mass and total_consumed_mass (the difference between\n"
      "the total-initial-mass and the current group mass) are no longer "
      "valid.\n"
      "Automatically checking for common-root and initializing new mass.");
    num_dyn_masses = dyn_masses.size();
    test_root_body();
    if (!mass->get_dyn_mass_initialized()) {
      mass->initialize_dyn_mass();
    }
  }
}
