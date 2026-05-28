exec(open("Log_data/log_verif.py").read())
log_verif(0.1)

exec(open("Modified_data/env_setup.py").read())

# This will trigger the process_inconsistent_setup() function for code coverage
dummy_veh_launcher_verif.launcher.set_intended_integ_body_state_body()

trick.add_read(0.5, """
dummy_veh_launcher_verif.launcher.launch()
""")

trick.sim_services.exec_set_terminate_time(1.0)
