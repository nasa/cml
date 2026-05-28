events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

# Set watch reference to NULL
events_manager_verif.manager.sim_stop_verif.set_watch_primitives(trick.get_address('events_manager_verif.base_bool'),None)

trick.sim_services.exec_set_terminate_time(9.0)
