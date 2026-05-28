exec(open("Log_data/log_verif.py").read())
log_verif(1.0)

exec(open("Modified_data/env_setup.py").read())

# Integ groups not specified at construction, have to run command after init
trick.add_read(0, """
dummy_veh_launcher_verif.launcher.set_intended_integ_group(trick.get_address("group2.integ_loop.integ_group[0]"))
""")

trick.add_read(50, """
dummy_veh_launcher_verif.launcher.launch()
""")

trick.sim_services.exec_set_terminate_time(100.0)
