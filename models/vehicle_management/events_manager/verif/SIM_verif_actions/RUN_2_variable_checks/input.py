exec(open("Log_data/log_events_manager_verif.py").read())
two_trigger_log()
# Don't use these for this scenario but need to be specified anyway.
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)
events_manager_verif.manager.two_variable_check_or.multi_shot = True


trick.add_read(0.2, "events_manager_verif.base_bool = True")
trick.add_read(0.4, "events_manager_verif.base_bool = False")
trick.add_read(0.7, "events_manager_verif.base_bool = True")
trick.add_read(0.9, "events_manager_verif.base_bool = False")

events_manager_verif.manager.two_variable_check_or.subscribe()
events_manager_verif.manager.two_variable_check_and.subscribe()


trick.stop(1)
