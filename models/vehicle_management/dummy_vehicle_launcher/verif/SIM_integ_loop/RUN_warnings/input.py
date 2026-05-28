exec(open("Log_data/log_verif.py").read())
log_verif(1.0)

exec(open("Modified_data/env_setup.py").read())

# Body is created at construction, don't need trick.add_read() like with integ groups
dummy_veh_launcher_verif.launcher.set_intended_integ_body(None)
dummy_veh_launcher_verif.launcher.set_intended_integ_group(None)

trick.add_read(10, """
dummy_veh_launcher_verif.launcher.launch()
""")

trick.add_read(50, """
dummy_veh_launcher_verif.launcher.initialize_integ_group_actions()
""")
trick.sim_services.exec_set_terminate_time(100.0)
