exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())
exec(open("Modified_data/time_default_config.py").read())

trick.sim_services.exec_set_terminate_time(0.5)
