exec(open("Log_data/log_verif.py").read())
log_verif(1.0)

exec(open("Modified_data/env_setup.py").read())

# Body is created at construction, don't need trick.add_read() like with integ groups
dummy_veh_launcher_verif.launcher.set_intended_integ_body_state_body()

trick.add_read(50, """
dummy_veh_launcher_verif.launcher.launch()
""")

trick.sim_services.exec_set_terminate_time(100.0)
