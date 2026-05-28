'''
RUN_2* tests the application of the function-modifier.
We adjust trigger to require a value > 0.8
With absolute modifier, this will be satisfied at t=2
and trigger 2 s after that.
'''

exec(open("RUN_2a_function_modifier_none/input.py").read())
verif.manager.trigger.function_modifier = trick.EventTriggerBase.Absolute
