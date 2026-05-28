# Just a debug RUN that we can use to compare with SET_reverse data
exec(open("SET_forward_8x8_Earth_only/RUN_baseline/input.py").read())
jeod_time.time_manager.dyn_time.scale_factor = -1.0
