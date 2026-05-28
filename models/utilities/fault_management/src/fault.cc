/*############################################################################
PURPOSE:
  (Base class for managing fault injection for a single variable.)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/

#include "../include/fault.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
Fault::Fault() :
  enabled(false),
  is_fire_limited(false),
  fire_limit(1),
  initialized(false),
  was_triggered_last_pass(false),
  fire_count(0)
{}


/*******************************************************************************
initialize
Purpose:(Initializes the fault. This method only sets the initialized flag, but
         some child classes perform checks first.)
*******************************************************************************/
void Fault::initialize() {
  initialized = true;
}


/*******************************************************************************
update
Purpose:(Injects the fault if a trigger group has been triggered.)
*******************************************************************************/
void Fault::update() {
  if (initialized && enabled && is_triggered()) {
    if (is_fire_limited) {
      if (fire_count >= fire_limit) {
        // Set enabled = false and overwrite the was_triggered_last_pass flag
        // so that if the fault is re-enabled, it will launch as though from
        // a fresh triggering; it can then fire another <fire_limit> times.
        disable();
        return;
      }
      fire_count++;
    }

    overwrite_value();
  }
}


/*******************************************************************************
add_trigger_group
Purpose:(Adds a trigger group to the fault.)
*******************************************************************************/
void Fault::add_trigger_group(TriggerGroup& trigger_group) {
  trigger_groups.push_back(&trigger_group);
}


/*******************************************************************************
set_param
Purpose:(Generic method for setting fault parameters. This base method is
         called if the child class does not recognize the parameter name.)
*******************************************************************************/
bool Fault::set_param( std::string param_name,
                       double,
                       bool)
{
  CMLMessage::error(__FILE__, __LINE__,
    "Fault Management Error\n",
    "The fault: ", name, "\nwas asked to set the unknown parameter: ", param_name, "\n");
  return false;
}


/*******************************************************************************
is_triggered
Purpose:(Determines whether any trigger group is triggered.)
*******************************************************************************/
bool Fault::is_triggered() {
  bool is_triggered = false;
  for (auto tg : trigger_groups) {
    is_triggered = tg->operate() || is_triggered;
    // Operate on all trigger groups (even if it has already been determined
    // that one of them is triggered) because periodic triggers should be
    // updated every frame.
  }

  // If this is a fresh trigger, then reset anything that needs to be reset
  // for a new application of the fault.
  if (!was_triggered_last_pass && is_triggered) {
    reset();
  }

  was_triggered_last_pass = is_triggered;

  return is_triggered;
}