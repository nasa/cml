exec(open("Log_data/log_vec_and_mat.py").read())
exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())

veh1.atmos_exec.subscribe()
veh2.atmos_exec.subscribe()

# See the Inform messages
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)

trick.stop(1000.0)
