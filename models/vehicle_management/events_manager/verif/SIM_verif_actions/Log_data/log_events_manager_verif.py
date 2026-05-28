drg1 = trick.DRAscii("events_manager_verif_vars")
drg1.set_cycle(1.0)
drg1.freq = trick.DR_Always
trick.add_data_record_group(drg1, trick.DR_Buffer)
events_manager_verif.logging_groups.add_to_list(drg1)

drg1.add_variable("events_manager_verif.sim_time")
drg1.add_variable("events_manager_verif.position")
drg1.add_variable("events_manager_verif.xx1")
drg1.add_variable("events_manager_verif.xx2")
drg1.add_variable("events_manager_verif.xx_in")
drg1.add_variable("events_manager_verif.ii")
drg1.add_variable("events_manager_verif.sim_off")
drg1.add_variable("events_manager_verif.base_bool")
drg1.add_variable("events_manager_verif.base_float")



drg2 = trick.DRAscii("events_manager_verif_triggers")
drg2.set_cycle(1.0)
drg2.freq = trick.DR_Always
trick.add_data_record_group(drg2, trick.DR_Buffer)
events_manager_verif.logging_groups.add_to_list(drg2)

drg2.add_variable("events_manager_verif.manager.sim_stop_verif.event_triggered")
drg2.add_variable("events_manager_verif.manager.sim_stop_var_verif.event_triggered")
drg2.add_variable("events_manager_verif.manager.relative_to_activation_double_verif.event_triggered")
drg2.add_variable("events_manager_verif.manager.deact_sim_obj_verif.event_triggered")
drg2.add_variable("events_manager_verif.manager.adjust_log_verif.event_triggered")
drg2.add_variable("events_manager_verif.manager.log_now_verif.event_triggered")
drg2.add_variable("events_manager_verif.manager.two_variable_check_and.event_triggered")
drg2.add_variable("events_manager_verif.manager.two_variable_check_or.event_triggered")

def two_trigger_log():
  drg3 = trick.DRAscii("two_trigger")
  drg3.set_cycle(0.1)
  drg3.freq = trick.DR_Always
  trick.add_data_record_group(drg3, trick.DR_Buffer)
  events_manager_verif.logging_groups.add_to_list(drg3)

  drg3.add_variable("events_manager_verif.sim_time")
  drg3.add_variable("events_manager_verif.base_bool")
  drg3.add_variable("events_manager_verif.manager.two_variable_check_and.watch_a.event_triggered")
  drg3.add_variable("events_manager_verif.manager.two_variable_check_and.event_triggered")
  drg3.add_variable("events_manager_verif.manager.two_variable_check_or.event_triggered")


def trigger_set_log():
  drg4 = trick.DRAscii("trigger_set")
  drg4.set_cycle(1)
  drg4.freq = trick.DR_Always
  trick.add_data_record_group(drg4, trick.DR_Buffer)
  events_manager_verif.logging_groups.add_to_list(drg4)

  drg4.add_variable("events_manager_verif.manager.trigger_set_bool.event_triggered")
  drg4.add_variable("events_manager_verif.manager.trigger_set_int.event_triggered")
  drg4.add_variable("events_manager_verif.manager.trigger_set_double.event_triggered")
  drg4.add_variable("events_manager_verif.manager.trigger_set_all.event_triggered")
  drg4.add_variable("events_manager_verif.manager.trigger_set_any.event_triggered")
