exec(open("Log_data/log_events_manager_verif.py").read())
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)

# Will change the logging rate to 0.2 instead of 1.0 at t = 3.6
events_manager_verif.manager.adjust_log_verif.subscribe()

# Will shut the sim off once position decreases past x=2.501, which occurs at t = 5
events_manager_verif.manager.sim_stop_var_verif.subscribe()

# Set base_bool to True so that adjust_log_verif event can turn it False with add_to_ext_bool_off()
events_manager_verif.base_bool = True

# Subscribe to deact_sim_obj_verif event so that adjust_log_verif event can
# unsubscribe it with add_to_unsubscribe(). Adjust its trigger time to 4.0 just
# to be make sure that it has been removed.
events_manager_verif.manager.deact_sim_obj_verif.subscribe()
events_manager_verif.manager.deact_sim_obj_verif.set_reference(3.8)

# Set the sim to shut sim off sometime after t = 5 to show that the sim_stop_var_verif
# event shuts the sim off prior to the set time
trick.sim_services.exec_set_terminate_time(9.0)
