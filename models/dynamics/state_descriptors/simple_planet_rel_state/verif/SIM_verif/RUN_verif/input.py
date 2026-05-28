exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())

trick.add_read(3, "vehicle.planet_rel.subscribe()")

trick.sim_services.exec_set_terminate_time(10)
