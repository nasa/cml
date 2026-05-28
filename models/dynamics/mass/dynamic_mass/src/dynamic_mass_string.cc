/*******************************************************************************

   PURPOSE:
   (Provides capbility to deplete mass equally from bodies strung together.)

   ASSUMPTIONS AND LIMITATIONS:
   ((Limitation 1: Relies on the DynBodyMass functionality to actually update
                   the body mass.  This just distributes mass changes across
                   the connected bodies.))

   PROGRAMMERS:
   (((Gary Turner) (OSR) (July 2014)
                      (New implementation of dynamic mass for JEOD 2.x))
    ((Bingquan Wang) (OSR) (April 2017)
                      (Disabled the compilation warning of float-point
                       number equality comparison))
    ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
   )
*******************************************************************************/
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/dynamic_mass_string.hh"

/*******************************************************************************
Method:
Purpose: Constructor
*******************************************************************************/
DynamicMassString::DynamicMassString()
  :
  DynamicMassBodyPropertiesInterface(),
  flow_down(false),
  mass_consumed(0.0),
  body_collection(),
  available_bodies(),
  string_in_group(false)
{ }

/*****************************************************************************
initialize
Purpose:(Copies the body_collection to available_bodies)
*****************************************************************************/
void
DynamicMassString::initialize()
{
  available_bodies = body_collection;
}

/*******************************************************************************
Method: distribute_mass_consumption
Purpose: (Distributes the mass consumption to the available sources.)
*******************************************************************************/
void
DynamicMassString::distribute_mass_consumption()
{
  // If there are no bodies available on this string or no mass consumed on this
  // string, nothing to do.

  if (available_bodies.empty() ||
      MathUtils::is_near_equal(mass_consumed_step, 0.0)) {
    // Note - no warning issued here, silent return.
    // It might be prudent to have whatever is making the demand verify
    // whether there is something available to draw on.
    return;
  }

  // There is an inevitable hierarchy here.  The available body mass is not
  // the body's consumable_mass, but the body's consumable_mass minus the
  // mass already assigned as consumed during this step.
  // Demands on a single body take precendence, then the demands from a string
  // in the order in which the strings are processed.
  // In the event that a body runs out of consumable mass, there is no go-back
  // to balance the draw down; the latter draws are distributed between
  // remaining bodies if available or just zeroed out if not.

  double mass_to_distribute = mass_consumed_step;
  bool distribute_mass = true;

  // As long as there is mass to distribute and bodies to distribute it
  // to/from, keep trying until it is all distributed, or we run out of bodies.
  while (distribute_mass) {
    // If the last body was depleted on the previous iteration, nothing more
    // can be done.
    // NOTE -- this may seem redundant with the entry-gate check of the same
    //         condition, but it is not.  The entry gate check prevents this
    //         method from running at all.  This check is to identify when
    //         the last body was removed.
    if (available_bodies.empty()) {
      CMLMessage::warn (
        __FILE__, __LINE__, "String depletion\n",
        " Dynamic Mass string exhausted all consumable mass while demand "
        "still existed.\nUnsatisfied demand is ", mass_to_distribute, " kg.\n"
        "No further warnings will be sent for this string.\n");
      break;
    }

    // Assume it is all distributed in this cycle, until demonstrated
    // otherwise.
    distribute_mass = false;

    // if flow_down, only use 1 body at a time, so put all the mass demand
    // onto that body:
    double mass_per = (flow_down)? mass_to_distribute :
                                   mass_to_distribute / available_bodies.size();

    // reset mass_to_distribute to 0 so that any shortfall can be incremented
    mass_to_distribute = 0.0;

    for (auto it = available_bodies.begin();
              it != available_bodies.end(); ++it) {
      DynamicMassBodyProperties & body_properties = (*it)->dynamic_properties;
      double mass_available = body_properties.consumable_mass -
                              body_properties.mass_consumed_step;
      // Normally - there is enough mass in each body to supply its demands
      // Add the mass_per to the mass consumed by that body
      if (mass_available >= mass_per) {
        body_properties.mass_consumed_step += mass_per;
        if (flow_down) {
          break;
        }
      }
      // If any body runs out:
      //   - record its shortage as mass still to distribute (to other bodies)
      //   - set its mass consumed equal to its consumable mass
      //   - remove it form the list of available bodies
      else {
        // increment the mass-to-distribute by the unavailable mass.
        // (Note -- right before the start of the loop, mass_to_distribute was
        //          set to 0, so this is counting unavailable mass only, not
        //          original-demand plus unavailable)
        mass_to_distribute += mass_per - mass_available;
        body_properties.mass_consumed_step = body_properties.consumable_mass;


        if (flow_down) {
          // Mass depletion is always assigned to the first body in
          // the available_bodies list.  Set mass_per so that the new top-body
          // sees all the remaining demand.
          mass_per = mass_to_distribute;
        }
        else {
          // Continue with depleting the bodies according to the mass_per
          // previously counted.  But also set:
          //  - distribute_mass flag to indicate that once all bodies have been
          //    handled, there will still be mass unaccounted for.  This will
          //    result in recomputing mass_per in a second loop through all
          //    remaining mass-bodies.
          distribute_mass = true;
        }
        // Remove this body from the list of available bodies
        it = available_bodies.erase(it);
        --it;
      }
    } // gone through all available bodies.
  }
  // Reset the mass_consumed_step; it has all been distributed to the bodies
  // (or there are no bodies left to distribute to).
  mass_consumed_step = 0.0;
}

/*****************************************************************************
reenable_bodies
Purpose:(Re-enables bodies in the event that mass is newly available)
*****************************************************************************/
void
DynamicMassString::reenable_bodies()
{
  available_bodies = body_collection;
}

/*******************************************************************************
Method:  (add_mass_to_string)
Purpose: (Adds a DynamicMassBody to a string)
*******************************************************************************/
void DynamicMassString::add_mass_to_string( // Return: -- void
    DynamicMassBody * new_mass_body)  /* In: -- DynamicMassBody to be added */
{
  if (new_mass_body == NULL) {
    CMLMessage::fail (
      __FILE__, __LINE__, "Setup error.\n",
      "A NULL pointer was given to the add_mass_to_string function. "
      "This is not allowed.\n"
      "Unsure how to proceed, terminating for safety.\n");
  }

  // check uniqueness to avoid inadvertent double-counting
  for (auto it = body_collection.begin(); it != body_collection.end(); ++it) {
    if (new_mass_body == (*it)) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid object addition\n",
        "Attempt to add a new DynamicMassBody (", new_mass_body->name, ") that is already assigned\n"
        "to this string. \n"
        "Cannot duplicate a mass-body on a single string.\n");
      return;
    }
  }
  // body is unique in this string
  body_collection.push_back(new_mass_body);

  if (string_in_group) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Verify configuration sequence\n",
      "A body has been added to the string after the string had been added\n"
      "to the group.  It is imperative to confirm that the body has also\n"
      "been added to the group or its mass will not be managed correctly.\n");
  }
}

/*******************************************************************************
Method:  update
Purpose: (Recomputes the available consumable mass so that it can be made
          available to exterior tests)
*******************************************************************************/
void
DynamicMassString::update()
{
  consumable_mass = 0.0;
  for (auto it = available_bodies.begin();
            it != available_bodies.end(); ++it) {
    consumable_mass += (*it)->dynamic_properties.consumable_mass;
  }
  mass_consumed += mass_consumed_step;
}
