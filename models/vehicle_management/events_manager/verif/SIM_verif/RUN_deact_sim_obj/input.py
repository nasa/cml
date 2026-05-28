exec(open("Log_data/log_events_manager_verif.py").read())
drg2.add_variable("events_manager_verif.manager.deact_sim_obj_verif.active")
trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)

events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

# Deactivate the sim object, should occur at t = 5.0
# This will stop the framework manager and the copy_time method
# Consequently, the position and sim_time variables should freeze at x=2.5m and
# t=5 respectively while the sim continues to t=9
events_manager_verif.manager.deact_sim_obj_verif.subscribe()

# Set the sim to shut sim off after t = 5.0
trick.sim_services.exec_set_terminate_time(9.0)
