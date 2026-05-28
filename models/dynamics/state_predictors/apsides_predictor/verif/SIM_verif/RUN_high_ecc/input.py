exec(open("common_input.py").read())
exec(open("Log_data/log_state.py").read())
log_state(1800.0)

vehicle.trans_init.velocity = [0.0, 10000.0, 0.0]

trick.stop(36000.0)
