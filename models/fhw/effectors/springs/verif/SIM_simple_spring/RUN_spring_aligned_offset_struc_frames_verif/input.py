exec(open("Log_data/log_spring_verif_rec.py").read())
log_test_data(0.005)

exec(open("Modified_data/env_setup_offset_struc_frames.py").read())
exec(open("Modified_data/springs_aligned.py").read())

trick.add_read(1,"""
veh_action.detach.active = True
""")

trick.sim_services.exec_set_terminate_time(2)
