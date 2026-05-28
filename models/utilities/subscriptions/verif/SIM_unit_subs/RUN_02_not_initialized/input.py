exec(open("Log_data/log_subscriptions.py").read())

print("\n"
"*************************************************************************\n"
"WARNING at t=6\n"
"   Cannot unsubscribe when there are no pending subscriptions.\n"
"*************************************************************************")

subs_verif.subs.subscribe()
trick.add_read(1.0, "subs_verif.subs.subscribe()")
trick.add_read(2.0, "subs_verif.subs.unsubscribe()")
trick.add_read(3.0, "subs_verif.subs.subscribe()")
trick.add_read(4.0, "subs_verif.subs.unsubscribe()")
trick.add_read(5.0, "subs_verif.subs.unsubscribe()")
trick.add_read(6.0, "subs_verif.subs.unsubscribe()")
trick.add_read(7.0, """
subs_verif.subs.quiet_unsubscribe_warning = True
subs_verif.subs.unsubscribe()
""")

trick.stop(7.0)
