exec(open("Log_data/log_events_manager_verif.py").read())
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)
# Trigger basic event when sim_time is larger than position
events_manager_verif.manager.reference_var_verif.subscribe()

# Change adjust_log_verif event to be based off xx1
events_manager_verif.manager.adjust_log_verif.subscribe()
events_manager_verif.manager.adjust_log_verif.set_watch_primitives(trick.get_address("events_manager_verif.xx1"), 1.0)
trick.add_read(3.0,"events_manager_verif.xx1 = 2.0")

# Change sim_stop_verif event to be based off when xx2 < xx_in
events_manager_verif.manager.sim_stop_var_verif.subscribe()
events_manager_verif.manager.sim_stop_var_verif.set_watch_primitives(trick.get_address("events_manager_verif.xx2"),trick.get_address("events_manager_verif.xx_in"))

trick.add_read(5.0,"events_manager_verif.xx2 = -1.0")

# event shuts the sim off prior to the set time
trick.sim_services.exec_set_terminate_time(9.0)
