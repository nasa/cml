/*############################################################################
PURPOSE:
  (Handling for groups of triggers)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/

#include "../include/trigger_group.hh"

/*******************************************************************************
operate
Purpose:(Determines whether all of the triggers are triggered.)
*******************************************************************************/
bool TriggerGroup::operate() {

  bool triggered = true;
  bool are_any_triggers_active = false;

  for (auto& it : triggers) {
    if (it.first) {
      triggered = it.second->operate() && triggered;
      are_any_triggers_active = true;
      // Operate on all active triggers (even if it has already been
      // determined that one of them is not triggered) because periodic
      // triggers should be updated every frame.
    }
  }

  return triggered && are_any_triggers_active;

}


/*******************************************************************************
add_trigger
Purpose:(Adds a trigger to the group.)
*******************************************************************************/
void TriggerGroup::add_trigger( TriggerBase& trigger) {
  triggers.push_back(std::pair<bool, TriggerBase*>(true, &trigger));
}


/*******************************************************************************
set_trigger_enable
Purpose:(Enables or disables a trigger in this group. If there are multiple
         triggers with the same name, they will be enabled/disabled together.)
*******************************************************************************/
bool TriggerGroup::set_trigger_enable( std::string trigger_name,
                                       bool        enable_flag)
{
  bool trigger_found = false;
  for (auto& it : triggers) {
    if (trigger_name.compare(it.second->name) == 0) {
      it.first = enable_flag;
      trigger_found = true;
    }
  }

  return trigger_found;
}
