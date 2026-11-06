exec(open("Log_data/log_data.py").read())
fd_obj.use_polynomials = True
fd_obj.nice_polynomial = False
fd_obj.vary_dt = True
trick.stop(12)
