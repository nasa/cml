exec(open("Log_data/log_events_manager_verif.py").read())
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)

events_manager_verif.manager.sim_stop_verif.subscribe()
events_manager_verif.manager.sim_stop_verif.multi_shot = True
events_manager_verif.dyn_flag = True

# Set the sim-off flag to allow sim-termination
trick.add_read(1.0, "events_manager_verif.sim_off = True")
# Remove the dyn-flag that has been holding up sim-termination
trick.add_read(1.5, "events_manager_verif.dyn_flag = False")



#Safety in case terminate does not work.
trick.sim_services.exec_set_terminate_time(2.0)
