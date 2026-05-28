exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())

vehicle.eclipse.subscribe()

trick.sim_services.exec_set_terminate_time(120000)
