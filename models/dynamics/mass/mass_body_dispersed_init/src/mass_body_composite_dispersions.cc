/*******************************************************************************
PURPOSE:
  (Define the class MassBodyCompositeDispersions.
   This class manages the application of dispersions to composite-properties
   of a compound body.)

LIBRARY DEPENDENCIES:
  (cml/models/dynamics/mass/mass_body_distribute_comp_to_core/src/mass_body_distribute_comp_to_core.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (April 2018) (Antares) (Initial version))
   ((Gary Turner) (OSR) (Sept 2018) (Antares)
          (Split out the composite-to-core functionality to support other
           models with similar use-cases))
   ((Brent Caughron) (OSR) (Mar 2021) (Antares) (Secondary IV&V)))
*******************************************************************************/

#include "../include/mass_body_composite_dispersions.hh"

#include "cml/models/dynamics/mass/mass_body_distribute_comp_to_core/include/mass_body_distribute_comp_to_core.hh"

/*******************************************************************************
Constructor
*******************************************************************************/
MassBodyCompositeDispersions::MassBodyCompositeDispersions (
    jeod::MassBody & target_body_in)
  :
  MassPropertiesDispersionsFlags(),
  BodyAction(),
  target_body( target_body_in),
  fail_if_mass_invalid(false),
  nominal(),
  disperse(),
  adjustable_body(nullptr),
  detach_sub_tree(nullptr)
{
  // Turn off the active flag by default.
  active = false;
  // All jeod::BodyAction instances have to have a subject; conceptually this one
  // has 2 but we have to assign one or the jeod::BodyAction content will fail.
  set_subject_body(target_body);
}

/*****************************************************************************
apply
Purpose:
  Used as the extension of the jeod::BodyAction::apply method to support the
  application of this action by the Dynamics Manager's list of jeod::BodyAction
  instances.
Note:
 - typically, this will only be called in response to a positive return
   from is_ready(), which queries the activate flag. So if(!activate) is
   omitted.
*****************************************************************************/
void
MassBodyCompositeDispersions::apply(jeod::DynManager & dyn_manager)
{
  update();
  jeod::BodyAction::apply(dyn_manager);
}

/*******************************************************************************
update
Purpose:
  Public-facing interface to the protected method update_internal()
Note:
  - Having the 2nd method with the adjustable_body argument is somewhat
    redundant given that the class has an adjustable-body member, but this
    is to support an architecture in which the method is called from
    outside with the determination of adjustable-body made on the fly.
*******************************************************************************/
void
MassBodyCompositeDispersions::update()
{
  if (!active) {
    return;
  }

  // Check that the adjustable-body has been assigned:
  if (adjustable_body == nullptr) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid Configuration.\n",
      "The adjustable-body pointer has not been set.\n"
      "Modifying composite-properties of the target body by adjusting its own "
      "core-properties.\n");
    adjustable_body = &target_body;
  }
  // call update_internal to use the specified adjustable_body pointer
  // Note that this method checks the hierarchy of target-body over
  // adjustable-body when comp_to_core.update() is called; no need to check
  // it here.
  update_internal();
}
/******************************************************************************/
void
MassBodyCompositeDispersions::update( jeod::MassBody & adjustable_body_)
{
  if (!active) {
    return;
  }
  adjustable_body = &adjustable_body_;
  update_internal();
}


/*****************************************************************************
update_internal
Purpose:
  Processes through the potential for needing to create a subtree and
  pushes on to comp_to_core_updates to actually apply the mass updates.
*****************************************************************************/
void
MassBodyCompositeDispersions::update_internal()
{

  // If the specified properties are for a sub-tree, we need to:
  //   - detach that part of the tree that is not included
  //   - apply the composite-to-core modifier
  //   - re-attach the sub-tree.
  // If the specified properties are for the entire tree (below the
  // target-body), we only need to apply the composite-to-core modifier.

  // If no detach-point has been specified, just apply the modifier:
  if ( detach_sub_tree == nullptr) {
    comp_to_core_updates();
  }
  // If the detach-point has been specified and is superior to the target-body
  // (so the sub-tree below the target-body would be unaffected by
  // detaching at the detach-point), or if the detach_sub_tree is set as a
  // different mass tree from the target-body then,
  // similarly just apply the modifier:
  else if (!detach_sub_tree->is_progeny_of( target_body)) {
    comp_to_core_updates();
  }
  // If the detach-point is defined, and is between the adjustable-body and
  // target-body so the detach process will take the adjustable-body with
  // it, drop an error message and just apply the modifier.
  else if ( adjustable_body->is_progeny_of( *detach_sub_tree)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid configuration.\n",
      "The adjustable-body is a component of the sub-tree being detached.\n"
      "That would make it inaccessible to the target-body.\n"
      "This is an invalid configuration.  Continuing with the\n"
      "composite-to-core assignment without detaching the sub-tree.\n");
    comp_to_core_updates();
  }
  // If the detach-point is defined AND
  // its removal will affect the composite-properties of the target-body AND
  // its removal will leave the target and adjustable bodies connected,
  // then setup to remove it.
  else {
    // Need to find the relative position and orientation of the detaching
    // body to allow us to reattach it.
    jeod::MassBody * parent_body = const_cast<jeod::MassBody*>
                                       (detach_sub_tree->get_parent_body());
    // Unreachable, safety net:
    // detach_sub_tree cannot be root because it must be subordinate to
    // target-body, so the parent must exist,
    // However, parent_body is a pointer so lets check:
    if (parent_body == nullptr) {
      CMLMessage::error(
        __FILE__,__LINE__,"Unknown Error.\n",
        "The detach-sub-tree body does not have a parent body.\n"
        "There is nothing to detach it from.\n");
      comp_to_core_updates();
    }
    else {
      // store the relative state to support re-attach:
      double rel_pos[3]; // position of struc frame wrt parent's struc frame.
      jeod::Vector3::copy( detach_sub_tree->structure_point.position,
                     rel_pos);
      double T_parent_this[3][3]; // relative orientation
      jeod::Matrix3x3::copy( detach_sub_tree->structure_point.T_parent_this,
                       T_parent_this);
      // Attempt the detach.
      if (!detach_sub_tree->detach()) {
        // If it failed, drop an error message and apply the modifier anyway.
        // Probably unreachable; it is not clear how a simple detach could fail,
        // so this is a safety net; don't want to be attaching if detach failed.
        CMLMessage::error(
          __FILE__,__LINE__,"Mass Tree Error.\n",
          "The process to detach the detach-sub-tree from the main tree "
          "failed.\n"
          "Composite-to-core modifier being applied to the main tree without\n"
          "removal of the specified sub-tree.\n");
        comp_to_core_updates();
      }
      else {
        // Detach succeeded. Apply the modifier and reattach.
        comp_to_core_updates();
        detach_sub_tree->attach_to( rel_pos,
                                    T_parent_this,
                                    *parent_body);
      }
    }
  }
}

/*****************************************************************************
comp_to_core_updates
Purpose:
  The main executable when using nominal and dispersions to generate the
  target-properties. This is intended to be an initialization routine to
  generate a dispersed set of composite-mass-properties and adjust the
  core-properties such that the desired composite-properties are met.
Note:
  - This method will only adjust the mass-properties of one body (the
    "adjustable" body) to meet the intended composite-mass-properties.
    To modify more than one body, it is necessary to generate intermediate
    composite-properties, adjust one body to meet those, then update the
    composite-properties and adjust a different body to meet those, etc.
*****************************************************************************/
void
MassBodyCompositeDispersions::comp_to_core_updates()
{
  // Use the nominal-properties (nominal) and dispersion-properties (disperse)
  // to generate new target-properties, which are the composite-properties of
  // the target-body.
  process_dispersions();

  // Create a temporary set of mass-properties to house the target values that
  // are a combination of properties from "nominal" and "disperse".
  jeod::MassProperties target_properties;

  // process_dispersions ensured that
  // disperse.{mass,position,inertia} has the actual intended disperions
  // Combine these with nominal values to get the target_properties:
  target_properties.mass = nominal.mass + disperse.mass;
  jeod::Vector3::sum( nominal.position,
                disperse.position,
                target_properties.position);
  jeod::Matrix3x3::add( nominal.inertia,
                  disperse.inertia,
                  target_properties.inertia);

  // TODO Turner 2021/12
  // If it is desired at some point that the nominal inertia values be
  // specified using anything other than body-frame basis, then follow these
  // steps:
  //  1. change the data type of nominal from MassProperties to
  //     MassPropertiesInit, and the include from mass_properties.hh to
  //     mass_properties_init.hh.
  //  2. Insert code at this point to transform target_properties.inertia
  //     (which -- being a copy of nominal.inertia -- could now be using any of
  //     the inertia specs supported by MassPropertiesInit) from its specified
  //     basis/origin into body frame basis and origin. Body frame basis and
  //     origin is assumed by comp_to_core.
  //     These transformations can be found in JEOD,
  //     dynamics/mass/src/mass_properties_init.cc
  //     MassPropertiesInit::initialize_mass_properties (...)

  // Create the comp-to-core instance that provides the mechanism by which
  // the adjustable-body's core-properties can be set to produce the desired
  // composite-properties on the target-body.
  MassBodyDistributeCompToCore comp_to_core( *adjustable_body,
                                             target_body);
  comp_to_core.fail_if_mass_invalid = fail_if_mass_invalid;

  if ( !comp_to_core.update( target_properties)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid configuration.\n",
      "Failure to initialize the composite-to-core method means that\n"
      "mass properties cannot be derived from composite-properties.\n"
      "Aborting action, mass properties remain at their current settings.\n");
  }
}

/*******************************************************************************
process_dispersions
PURPOSE:(Processes the dispersions, considering the optional flags to get
         the correct interpretations in generating the target-properties.)
*******************************************************************************/
void
MassBodyCompositeDispersions::process_dispersions()
{
  // Generate random values if so desired.
  if (random_num_distribution) { // from MassPropertiesDispersionsFlags
    disperse.disp_mag_random_number_generator();
  }

  // By default, dispersed values are absolute x = x_nom +- disp
  // Optionally, dispersed values could be relative: x = x_nom * (1.0 +- disp)
  // In this case, make them absolute.
  if (mass_disp_is_relative) {
    disperse.scale_mass( nominal.mass);
  }
  if (pos_disp_is_relative) {
    disperse.scale_position( nominal.position);
  }

  if (inertia_moi_disp_is_relative) {
    disperse.scale_moi( nominal.inertia[0][0],
                        nominal.inertia[1][1],
                        nominal.inertia[2][2]);
  }
  if (inertia_poi_disp_is_relative) {
    disperse.scale_poi( nominal.inertia[0][1],
                        nominal.inertia[0][2],
                        nominal.inertia[1][2]);
  }

  // Construct the dispersed inertia tensor from the moment and products of
  // inertia dispersions.
  disperse.generate_inertia_tensor( poi_are_negative_integrals);
}
