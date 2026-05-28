exec(open("RUN_sim_stop_var_adjust_logging/input.py").read())
# Repeat the RUN_sim_stop_var_adjust_logging case, but set the adjust_log_verif
# to log-now, which also results in an immediate application of the change
# to the calling frequency.
events_manager_verif.manager.action_adjust_logging.log_now = True
