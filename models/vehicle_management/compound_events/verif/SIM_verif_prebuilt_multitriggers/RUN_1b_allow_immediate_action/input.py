'''
Runs same configuration as RUN_1a, except that disarming and triggering
are now allowed in the same cycle as arming.
At t=1, we now get an immediate disarm
At t=2, we get a re-arm that was not seen in RUN_1a.
At t=5, the event arms AND triggers.
At t=6 and 7, the action-triggers 0 and 1 remain triggered from t=5 even
              though their conditions are not satisfied.
At t=7, when the remaining action-trigger (2) triggers, the event
        modes to HoldAction
At t=9, the time-delay is complete and the event triggers.
'''

exec(open("RUN_1a_basic_config/input.py").read())
verif.manager.event.allow_immediate_actions = True
