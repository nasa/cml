exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())

trick.add_read(30000, "vehicle.eclipse.subscribe()")
trick.add_read(90000, "vehicle.eclipse.unsubscribe()")

trick.stop(120000)
