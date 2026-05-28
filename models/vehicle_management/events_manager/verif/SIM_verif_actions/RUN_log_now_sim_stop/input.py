trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)
exec(open("Log_data/log_events_manager_verif.py").read())
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

events_manager_verif.xx_in = 2.2

# Shut the sim down and fire logging at t=2.4s
events_manager_verif.manager.log_now_verif.set_reference(2.399)

trick.add_read(2.4,'''
events_manager_verif.sim_off = True
''')

events_manager_verif.manager.log_now_verif.subscribe()
events_manager_verif.manager.sim_stop_verif.subscribe()

# Set the sim to shut sim off sometime after t = 2.4 to show that the sim_stop_verif
# event shuts the sim off prior to the set time
trick.sim_services.exec_set_terminate_time(9.0)
