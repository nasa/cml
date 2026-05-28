exec(open("Log_data/log_subscriptions.py").read())

print("\n"
"*************************************************************************\n"
"ERROR (x2)\n"
"   Cannot subscribe to a disabled model.\n"
"*************************************************************************")

subs_verif.subs.disable()

# Will produce an error and not subscribe
subs_verif.subs.subscribe()
trick.add_read(1.0, "subs_verif.subs.unsubscribe()")
# Will produce the same error as before, but only once
trick.add_read(2.0, """
subs_verif.subs.initialize()
subs_verif.subs.subscribe()
subs_verif.subs.quiet_disabled_warning = True
print('\\n'
'*************************************************************************\\n'
'Checking on quiet_disabled_warning reset:\\n'
'Before subscribe: ', subs_verif.subs.quiet_disabled_warning )
subs_verif.subs.subscribe()
print(
'No error message during subscribe()\\n'
'After  subscribe: ', subs_verif.subs.quiet_disabled_warning,'\\n'
'*************************************************************************\\n')
""")
trick.add_read(3.0, "subs_verif.subs.unsubscribe()")

trick.stop(3.0)
