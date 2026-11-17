exec(open("Log_data/log_data.py").read())
fd_obj.dt = 0.03
fd_obj.use_polynomials = True
fd_obj.vary_dt = True
trick.stop(140)
