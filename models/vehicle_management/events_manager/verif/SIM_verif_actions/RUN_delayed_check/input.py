exec(open("Log_data/log_events_manager_verif.py").read())
drg1.set_cycle(0.1)

events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)
events_manager_verif.manager.delayed_check.subscribe()
# Skip past 7, should noot trigger because it is discrete
trick.add_read(0.4,"events_manager_verif.ii = 3")
trick.add_read(0.7,"events_manager_verif.ii = 2")
trick.add_read(0.8,"events_manager_verif.ii = 3")
trick.add_read(0.9,"events_manager_verif.ii = 2")
trick.stop(2)
