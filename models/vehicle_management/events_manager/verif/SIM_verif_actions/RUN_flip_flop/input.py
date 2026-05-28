'''
This run exercises the flip-flop event with connected logging actions
- 1.0 base_bool = True has no effect because flip_flop is inactive so its
      associated-watch is also inactive.
- 2.0 base_bool = False has no effect (same reason)
- 2.5 base_bool = True has no effect (same reason)
- 3.0 flip-flop is activated and accepts the current state with base-bool =
      true. No action
- 4.0 base_bool = False. This switches the event-triggered flag in event
      base_bool_ff from true to false, triggering the flip-flop event on a
      down-trigger. The boolean action_adjust_logging_group.enable_group is
      set to false, and the action_adjust_logging_group action is executed,
      stopping the logging.
- 6.0 base_bool = True. This switches the event-triggered flag in event
      base_bool_ff from false to truee, triggering the flip-flop event on an
      up-trigger. The boolean action_adjust_logging_group.enable_group is
      set to true, and the action_adjust_logging_group action is executed,
      starting the logging.
- 8.0 base_bool = False. Same effect as t=4. No more logging.
'''

drg1 = trick.DRAscii("group_adjust_1")
drg1.set_cycle(0.1)
drg1.freq = trick.DR_Always
trick.add_data_record_group(drg1, trick.DR_Buffer)
events_manager_verif.logging_groups.add_to_list(drg1)
drg1.add_variable("events_manager_verif.sim_time")

drg2 = trick.DRAscii("group_adjust_2")
drg2.set_cycle(1)
drg2.freq = trick.DR_Always
trick.add_data_record_group(drg2, trick.DR_Buffer)
events_manager_verif.logging_groups.add_to_list(drg2)
drg2.add_variable("events_manager_verif.sim_time")

drg3 = trick.DRAscii("group_adjust_3")
drg3.set_cycle(0.5)
drg3.freq = trick.DR_Always
trick.add_data_record_group(drg3, trick.DR_Buffer)
events_manager_verif.logging_groups.add_to_list(drg3)
drg3.add_variable("events_manager_verif.sim_time")


events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)

events_manager_verif.manager.action_adjust_logging_group.log_cycle =1

trick.add_read(1.0,"events_manager_verif.base_bool = True")
trick.add_read(2.0,"events_manager_verif.base_bool = False")
trick.add_read(2.5,"events_manager_verif.base_bool = True")
trick.add_read(3.0,"events_manager_verif.manager.flip_flop.subscribe()")
trick.add_read(4.0,"events_manager_verif.base_bool = False")
trick.add_read(6.0,"events_manager_verif.base_bool = True")
trick.add_read(8.0,"events_manager_verif.base_bool = False")

trick.stop(9)
