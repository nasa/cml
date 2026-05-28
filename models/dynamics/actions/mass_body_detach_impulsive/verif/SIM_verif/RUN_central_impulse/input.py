exec(open("Log_data/log_data.py").read())
log_test_data(1.0)

exec(open("Modified_data/env_setup.py").read())

trick.add_read(3.0, """
vehicle.detach_B_from_A.active = True
""")


trick.sim_services.exec_set_terminate_time(10)
