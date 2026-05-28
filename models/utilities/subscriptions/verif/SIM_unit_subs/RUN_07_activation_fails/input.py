exec(open("Log_data/log_activation_fails.py").read())

subs_verif.activation_fails.subscribe_name = "activation-failure test case"

# Add 3 pending subscriptions:
subs_verif.activation_fails.subscribe()
subs_verif.activation_fails.subscribe()
subs_verif.activation_fails.subscribe()
trick.add_read(1.0, """
print('\\n'
'********************************************************************\\n'
'ERROR at t=1 on model initialization.\\n'
'Should result in:\\n'
' - no initialization and\\n'
' - retention of pending subscriptions.\\n'
'********************************************************************')
subs_verif.activation_fails.initialize()
""")



trick.add_read(2.0,"""
print('\\n'
'********************************************************************\\n'
'ERROR at t=2 on model initialization.\\n'
'Should result in:\\n'
' - model initialization and\\n'
' - removal of pending subscriptions.\\n'
'********************************************************************')
subs_verif.activation_fails.initialize_on_failed_activation = True
subs_verif.activation_fails.initialize()
""")

trick.add_read(3.0,"""
print('\\n'
'********************************************************************\\n'
'Attempt to subscribe following initialization.\\n'
'Should result in:\\n'
' - no changes; failure of model activation leads to no subscription.\\n'
'********************************************************************')
subs_verif.activation_fails.subscribe()
""")

trick.stop(3)
