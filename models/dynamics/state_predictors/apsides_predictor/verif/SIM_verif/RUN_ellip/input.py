exec(open("common_input.py").read())
exec(open("Log_data/log_state.py").read())
log_state( 300.0 )

vehicle.trans_init.velocity = [0.0, 4000.0, 7000.0]

trick.stop(6000.0)
