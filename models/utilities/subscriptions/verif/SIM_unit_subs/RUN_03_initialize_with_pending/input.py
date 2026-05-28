exec(open("Log_data/log_subscriptions.py").read())

trick.add_read(1.0, "subs_verif.subs.subscribe()")
trick.add_read(2.0, "subs_verif.subs.subscribe()")
trick.add_read(3.0, "subs_verif.subs.subscribe()")
trick.add_read(4.0, "subs_verif.subs.initialize()")
trick.stop(4.0)
