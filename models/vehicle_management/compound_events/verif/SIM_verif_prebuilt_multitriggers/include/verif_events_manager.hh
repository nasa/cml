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
  EventTrigger<double> trigger_arm0;
  EventTrigger<double> trigger_arm1;
  EventTrigger<double> trigger_arm2;
  EventTrigger<double> trigger_disarm0;
  EventTrigger<double> trigger_disarm1;
  EventTrigger<double> trigger_disarm2;
  EventTrigger<double> trigger_act0;
  EventTrigger<double> trigger_act1;
  EventTrigger<double> trigger_act2;

  CompoundEvent event;


  VerifCompoundEventsManager (SampleSimData & sim_data)
    :
    CompoundEventsManager(sim_data.time),

    trigger_arm0(    sim_data.time),
    trigger_arm1(    sim_data.time),
    trigger_arm2(    sim_data.time),
    trigger_disarm0( sim_data.time),
    trigger_disarm1( sim_data.time),
    trigger_disarm2( sim_data.time),
    trigger_act0(    sim_data.time),
    trigger_act1(    sim_data.time),
    trigger_act2(    sim_data.time),

    event( sim_data.time)

  {
    trigger_arm0.set_watch( sim_data.trigger_var_arm[0], 1.2);
    trigger_arm0.comparison_logic = EventTriggerBase::GE;
    trigger_arm0.multi_shot = true;
    /****/
    trigger_arm1.set_watch( sim_data.trigger_var_arm[1], 2.3);
    trigger_arm1.comparison_logic = EventTriggerBase::GT;
    trigger_arm1.multi_shot = true;
    /****/
    trigger_arm2.set_watch( sim_data.trigger_var_arm[2], 3.4);
    trigger_arm2.comparison_logic = EventTriggerBase::LT;
    trigger_arm2.multi_shot = true;
    /****/
    trigger_disarm0.set_watch( sim_data.trigger_var_disarm[0], 1.2);
    trigger_disarm0.comparison_logic = EventTriggerBase::GE;
    trigger_disarm0.multi_shot = true;
    /****/
    trigger_disarm1.set_watch( sim_data.trigger_var_disarm[1], 2.3);
    trigger_disarm1.comparison_logic = EventTriggerBase::GT;
    trigger_disarm1.multi_shot = true;
    /****/
    trigger_disarm2.set_watch( sim_data.trigger_var_disarm[2], 3.4);
    trigger_disarm2.comparison_logic = EventTriggerBase::LT;
    trigger_disarm2.multi_shot = true;
    /****/
    trigger_act0.set_watch( sim_data.trigger_var_action[0], 1.2);
    trigger_act0.comparison_logic = EventTriggerBase::GE;
    /****/
    trigger_act1.set_watch( sim_data.trigger_var_action[1], 2.3);
    trigger_act1.comparison_logic = EventTriggerBase::GT;
    /****/
    trigger_act2.set_watch( sim_data.trigger_var_action[2], 0.0);
    trigger_act2.comparison_logic = EventTriggerBase::Change;

    // configure the events
    event.name = "Test Event";
    event.message = "Test Event processed";
    event.delay_offset = 2;
    event.add_to_ext_bool_on( sim_data.action_assgt);

    event.arming_triggers.add_trigger(trigger_arm0);
    event.arming_triggers.add_trigger(trigger_arm1);
    event.arming_triggers.add_trigger(trigger_arm2);
    event.arming_triggers.require_all = false;

    event.disarming_triggers.add_trigger(trigger_disarm0);
    event.disarming_triggers.add_trigger(trigger_disarm1);
    event.disarming_triggers.add_trigger(trigger_disarm2);
    event.disarming_triggers.require_all = true;

    event.action_triggers.add_trigger(trigger_act0);
    event.action_triggers.add_trigger(trigger_act1);
    event.action_triggers.add_trigger(trigger_act2);
    event.action_triggers.require_all = true;

    add_event( event);

  }
};
#endif
