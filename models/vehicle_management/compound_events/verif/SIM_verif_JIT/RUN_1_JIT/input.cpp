#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

extern "C" int run_me()
{
  // create and configure the triggers.
  EventTrigger<double> & trigger_arm0 =
                   verif.manager.create_trigger( verif.trigger_var_arm[0],
                                                 1.2,
                                                 EventTriggerBase::GE);
  trigger_arm0.multi_shot = true;
  //****
  EventTrigger<double> & trigger_arm1 =
                   verif.manager.create_trigger( verif.trigger_var_arm[1],
                                                 2.3,
                                                 EventTriggerBase::GT);
  trigger_arm1.multi_shot = true;
  //****
  EventTrigger<double> & trigger_arm2 =
                   verif.manager.create_trigger( verif.trigger_var_arm[2],
                                                 3.4,
                                                 EventTriggerBase::LT);
  trigger_arm2.multi_shot = true;
  //****
  EventTrigger<double> & trigger_disarm0 =
                   verif.manager.create_trigger( verif.trigger_var_disarm[0],
                                                 1.2,
                                                 EventTriggerBase::GE);
  trigger_disarm0.multi_shot = true;
  //****
  EventTrigger<double> & trigger_disarm1 =
                   verif.manager.create_trigger( verif.trigger_var_disarm[1],
                                                 2.3,
                                                 EventTriggerBase::GT);
  trigger_disarm1.multi_shot = true;
  //****
  EventTrigger<double> & trigger_disarm2 =
                   verif.manager.create_trigger( verif.trigger_var_disarm[2],
                                                 3.4,
                                                 EventTriggerBase::LT);
  trigger_disarm2.multi_shot = true;
  //****
  EventTrigger<double> & trigger_act0 =
                   verif.manager.create_trigger( verif.trigger_var_action[0],
                                                 1.2,
                                                 EventTriggerBase::GE);
  //****
  EventTrigger<double> & trigger_act1 =
                   verif.manager.create_trigger( verif.trigger_var_action[1],
                                                 2.3,
                                                 EventTriggerBase::GT);
  //****
  EventTrigger<double> & trigger_act2 =
                   verif.manager.create_trigger( verif.trigger_var_action[2],
                                                 0.0,
                                                 EventTriggerBase::Change);

  EventTrigger<double> & trigger_act3 =
                   verif.manager.create_trigger( "verif.trigger_var_action[2]",
                                                 0.0,
                                                 EventTriggerBase::Change);

  // create and configure the events
  CompoundEvent & event = verif.manager.create_event();
  event.name = "Test Event";
  event.message = "Test Event processed";
  event.delay_offset = 2;
  event.add_to_ext_bool_on( verif.action_assgt);

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
  event.action_triggers.add_trigger(trigger_act3);
  event.action_triggers.require_all = true;
  event.subscribe();
  return 0;
}
