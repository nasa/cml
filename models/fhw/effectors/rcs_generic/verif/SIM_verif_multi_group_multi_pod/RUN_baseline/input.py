exec(open("Log_data/log_data_rcs.py").read())

test.rcs.subscribe()

trick.sim_services.exec_set_terminate_time(1.0)
