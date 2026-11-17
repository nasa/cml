import math
exec(open("Log_data/log_data.py").read())
fd_obj.vary_dt = True
fd_obj.dt = 0.01
fd_obj.time = -0.03


trick.stop(240)
