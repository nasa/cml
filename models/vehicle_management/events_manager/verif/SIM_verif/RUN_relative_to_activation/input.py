exec(open("Log_data/log_events_manager_verif.py").read())
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)
# Subscribe to relative to activation event at t > 0s to show it triggers
# one second (value of xx1) after activation/subscribing
# NOTE -- this event stops the sim.
events_manager_verif.xx1 = 1.0
trick.add_read(6.0,"events_manager_verif.manager.relative_to_activation_double_verif.subscribe()")

# Subscribe to relative to activation int event
events_manager_verif.ii = 4
events_manager_verif.manager.relative_to_activation_int_verif.subscribe()
trick.add_read(1.0,"events_manager_verif.ii = 5")
trick.add_read(2.0,"events_manager_verif.ii = 6")
trick.add_read(3.0,"events_manager_verif.ii = 7")
trick.add_read(4.0,"events_manager_verif.ii = 8")

# Susbscribe to relative to activation bool event
events_manager_verif.base_bool = True
events_manager_verif.manager.relative_to_activation_bool_verif.subscribe()
trick.add_read(1.0,"events_manager_verif.base_bool = False")
trick.add_read(2.0,"events_manager_verif.base_bool = True")

# Subscribe to relative to activation reference variable event
events_manager_verif.base_float = 10.1
events_manager_verif.manager.relative_to_activation_float_verif.subscribe()
trick.add_read(3.0,"events_manager_verif.base_float = 13.09")
trick.add_read(4.0,"events_manager_verif.base_float = 13.1")
trick.add_read(5.0,"events_manager_verif.base_float = 13.11")

# Sim shuts off at t = 7.0 because relative_to_activation_double_verif stops
# it.
