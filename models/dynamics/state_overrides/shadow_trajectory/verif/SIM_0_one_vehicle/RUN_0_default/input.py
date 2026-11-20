exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())
vehicle.traj.data_filename = "../data/traj"

trick.stop(8)
