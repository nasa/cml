'''
RUN_2* tests the application of the function-modifier.
We adjust trigger to require a value > 0.8
With no modifier, this will be satsified at t=8
and trigger 2 s after that.
'''

exec(open("Log_data/log_data.py").read())
verif.framework.vars_file_name = "Unit_test_data/vars.txt"
verif.framework.data_file_name = "Unit_test_data/data_2.txt"

verif.manager.enabled = True
trick.CMLMessage.set_publish_level( trick.CMLMessage.Debug)

verif.manager.event.subscribe();

verif.manager.trigger.set_reference(0.8)
verif.manager.trigger.comparison_logic = trick.EventTriggerBase.GT
trick.stop(10)
