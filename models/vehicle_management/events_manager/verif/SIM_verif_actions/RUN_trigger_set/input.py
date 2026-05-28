exec(open("Log_data/log_events_manager_verif.py").read())
trigger_set_log()

# Don't use these for this scenario but need to be specified anyway.
events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)
events_manager_verif.manager.two_variable_check_or.multi_shot = True


# Set two of the triggers, ANY should trigger, ALL should not
trick.add_read(0.2, """
events_manager_verif.base_bool = True
events_manager_verif.ii = 1
""")

# switch bool off, but this is a single-shot trigger so it is "sticky" so even
# though the watch-value is no longer valid, the event remains triggered.
# Also trigger-down the int event. This is a multi-shot event so its
# event-triggered flag will reset to false.
# Only 1 trigger is triggered.
trick.add_read(0.3, """
events_manager_verif.base_bool = False
events_manager_verif.ii = 0
""")

# Re-trigger the int event, 2 of the 3 triggers are now triggered
trick.add_read(0.4, """
events_manager_verif.ii = 1
""")

# De-trigger the int event and trigger the double event.
# 2 of the 3 triggers are now triggered
trick.add_read(0.5, """
events_manager_verif.ii = 0
events_manager_verif.xx1 = 1.5
""")

# De-trigger the double event and re-trigger the double event.
# 2 of the 3 triggers are now triggered
trick.add_read(0.6, """
events_manager_verif.ii = 1
events_manager_verif.xx1 = 0.5
""")

# Re-trigger the double event
# 3 of the 3 triggers are now triggered
# Logging action is triggered based on the ALL event.
trick.add_read(0.7, """
events_manager_verif.xx1 = 1.5
""")

events_manager_verif.manager.trigger_set_all.subscribe()
events_manager_verif.manager.trigger_set_any.subscribe()


trick.stop(0.8)
