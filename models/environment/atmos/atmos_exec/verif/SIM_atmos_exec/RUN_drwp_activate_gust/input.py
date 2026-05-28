exec(open("RUN_drwp/input.py").read())

# Turn on Gust for veh1 and configure it with some data

veh1.atmos_exec.gust.gust_frame      = trick.GustModel.TD_FRAME  # Default frame
veh1.atmos_exec.gust.direction = [1.0, 0.0, 0.0]       # x-dir
veh1.atmos_exec.gust.maximum_magnitude = 200.0         # Strength of gust
veh1.atmos_exec.gust.period   = 50.0                   # Gust for 50 sec

# Start gust at 50 sec in
trick.add_read(50, "veh1.atmos_exec.gust.start = True")

trick.stop(200.0)
