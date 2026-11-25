/*******************************TRICK HEADER******************************
PURPOSE: (A pre-compiled, SWIG-friendly events manager)

PROGRAMMERS:
  (((Gary Turner) (OSR) (01/2024) (ANTARES) (initial)))
**********************************************************************/
#ifndef CML_VERIF_EVENTS_MANAGER_HH
#define CML_VERIF_EVENTS_MANAGER_HH

#include "../../../include/compound_event_manager.hh"
#include "sample_sim_data.hh"


class VerifCompoundEventsManager : public cml::CompoundEventsManager
{
 public:
  cml::EventTrigger<double> trigger;
  cml::CompoundEvent event;

  VerifCompoundEventsManager (SampleSimData & sim_data)
    :
    cml::CompoundEventsManager(sim_data.time),
    trigger( sim_data.time),
    event( sim_data.time)

  {
    trigger.set_watch( sim_data.sim_var, 0.8);

    // configure the events
    event.name = "Test Event";
    event.message = "Test Event processed";
    event.delay_offset = 0.8;
    event.add_to_ext_bool_on( sim_data.action_assgt);

    event.action_triggers.add_trigger(trigger);
    add_event( event);
  }

  VerifCompoundEventsManager(const VerifCompoundEventsManager&) = delete;
  VerifCompoundEventsManager& operator=(const VerifCompoundEventsManager&) = delete;
};
#endif
