exec(open("Log_data/log_events_manager_verif.py").read())
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)
# Subscribe to relative to activation int event
# reference will be computed to 4+3 = 7
events_manager_verif.ii = 4
events_manager_verif.manager.relative_to_activation_int_verif.subscribe()
# Skip past 7, should noot trigger because it is discrete
trick.add_read(1.0,"events_manager_verif.ii = 5")
trick.add_read(2.0,"events_manager_verif.ii = 8")
trick.add_read(3.0,"""
events_manager_verif.ii = 5
events_manager_verif.manager.relative_to_activation_int_verif.use_threshold_crossing_trigger = True
events_manager_verif.manager.relative_to_activation_int_verif.set_direction(1)
""")
# Skip past 7 again, this time it should trigger (at t=5)
trick.add_read(4.0,"events_manager_verif.ii = 6")
trick.add_read(5.0,"events_manager_verif.ii = 8")
# Set the sim to shut sim off after t = 6.0
trick.stop(6)
