/*******************************TRICK HEADER******************************
PURPOSE: (A pre-compiled, SWIG-friendly events manager)

PROGRAMMERS:
  (((Gary Turner) (OSR) (01/2024) (ANTARES) (initial)))
**********************************************************************/
#ifndef CML_VERIFICATION_COMPOUND_EVENTS_MANAGER_HH
#define CML_VERIFICATION_COMPOUND_EVENTS_MANAGER_HH

#include "../../../include/compound_event_manager.hh"
#include "sample_sim_data.hh"


class VerifCompoundEventsManager : public CompoundEventsManager
{
 public:
  EventTrigger<int> trigger_i;
  EventTrigger<bool> trigger_b;
  CompoundEvent event;

  VerifCompoundEventsManager (SampleSimData & sim_data)
    :
    CompoundEventsManager(sim_data.time),
    trigger_i( sim_data.time, "integer trigger"),
    trigger_b( sim_data.time, "boolean trigger"),
    event( sim_data.time)

  {
    trigger_i.set_watch( sim_data.sim_int, (int)0);
    trigger_i.multi_shot = true;
    trigger_b.set_watch( sim_data.sim_bool, true);
    trigger_b.multi_shot = true;
    trigger_b.comparison_logic = EventTriggerBase::EQ;

    // configure the events
    event.name = "Test Event";
    event.message = "Test Event processed";
    event.add_to_ext_bool_on( sim_data.action_assgt);

    event.action_triggers.add_trigger(trigger_i);
    event.action_triggers.add_trigger(trigger_b);
    event.action_triggers.require_all = true;
    event.multi_shot = true;
    event.action_triggers.multi_shot = true;
    add_event( event);
  }
};
#endif
