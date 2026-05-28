exec(open("Log_data/log_events_manager_verif.py").read())
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

events_manager_verif.manager.bad_variable_verif.subscribe()
trick.stop(6)
