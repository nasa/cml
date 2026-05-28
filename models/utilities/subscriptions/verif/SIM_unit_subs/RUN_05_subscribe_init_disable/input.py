exec(open("Log_data/log_subscriptions.py").read())

subs_verif.subs.subscribe()
subs_verif.subs.subscribe()
trick.add_read(1.0, "subs_verif.subs.initialize()")
trick.add_read(2.0, "subs_verif.subs.disable()")

trick.stop(2.0)
