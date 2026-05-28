exec(open("Log_data/log_events_manager_verif.py").read())
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)
# Subscribe to relative to activation int event
# reference will be computed to 4+3 = 7
events_manager_verif.ii = 4
events_manager_verif.manager.relative_to_activation_float_verif.subscribe()
events_manager_verif.manager.relative_to_activation_float_verif.set_direction(0)
events_manager_verif.manager.relative_to_activation_float_verif.multi_shot = True
# Skip past 7, should noot trigger because it is discrete
trick.add_read(1.0,"events_manager_verif.base_float = 4")
trick.add_read(2.0,"events_manager_verif.base_float = 5")
trick.add_read(3.0,"events_manager_verif.base_float = 2")
trick.add_read(4.0,"events_manager_verif.base_float = 1")
trick.add_read(5.0,"events_manager_verif.base_float = 6")
trick.stop(5)
