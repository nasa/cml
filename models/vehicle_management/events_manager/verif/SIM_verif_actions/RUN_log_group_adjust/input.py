'''
This scenario investigates the EventActionAdjustLoggingGroup action.

- The action_adjust_logging_group action is connected with two data-record
  groups:
  * group_adjust_1 (enabled)
  * group_adjust_2 (disabled)
  - Note: group_adjust3 is also logged in this run and used as a baseline to
    confirm that the changes re applied only to the connected record-group.
- The action is default-configured to set the log-cycle of the associated
  groups to 2 seconds.

- This action is added to three events:
    - adjust_log_group1 triggers at t=2.1 and:
      - subscribes to the second event:
      - executes the default setting of the action, setting the log-cycle
        to 2 seconds
      - enables group_adjust_2 drata-record-group due to AdjustLoggingGroup
        event class process_group() function
    - adjust_log_group2 triggers at 6.3 and:
      - subscribes to the third event
      - sets the action's enable_group flag to false
      - executes the action, which disables the logging groups per the
        new setting of enable_group.
    - adjust_log_group3 triggers at 10.3 and:
      - sets the action's enable_group flag to true
      - sets the action's log_now flag to true
      - sets the action's log_cycle variable to 0.3
      - executes the action, which:
        - forces a log at t=10.3 per setting of log_now
        - enables the logging groups per the new setting of enable_group.
        - initiates the subsequent logging of these data-record groups to
          occur at intervals of 0.3 seconds.

- By default, setting the cycle will take effect at the next multiple of the
  new cycle (referenced to 0) for both an active or inactive group.

  At t=2.1:
    - Event trigger will drop a log point at 2.1 for both data-record-groups 
      group_adjust_1 and group_adjust_2. Next will be at t=4.0.
  At t=6.3
    - both groups stop logging immediately.
  At t=10.3
    - the action is flagged to "log now" so both groups log at 10.3
    - The next multiple of 0.3 is 10.5 so both groups log at 10.5, 10.8, ...
'''

# Unique logging for this scenario:
drg1 = trick.DRAscii("group_adjust_1")
drg1.set_cycle(0.1)
drg1.freq = trick.DR_Always
trick.add_data_record_group(drg1, trick.DR_Buffer)
events_manager_verif.logging_groups.add_to_list(drg1)
drg1.add_variable("events_manager_verif.sim_time")

drg2 = trick.DRAscii("group_adjust_2")
drg2.set_cycle(1)
drg2.freq = trick.DR_Always
trick.add_data_record_group(drg2, trick.DR_Buffer)
events_manager_verif.logging_groups.add_to_list(drg2)
drg2.add_variable("events_manager_verif.sim_time")
events_manager_verif.logging_groups.disable(drg2)

drg3 = trick.DRAscii("group_adjust_3")
drg3.set_cycle(0.5)
drg3.freq = trick.DR_Always
trick.add_data_record_group(drg3, trick.DR_Buffer)
events_manager_verif.logging_groups.add_to_list(drg3)
drg3.add_variable("events_manager_verif.sim_time")


events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)

events_manager_verif.manager.adjust_log_group1.subscribe()

trick.add_read(10,"""
events_manager_verif.manager.action_adjust_logging_group.log_cycle = 0.4
""")

trick.stop(11)