# Setup to execute the same as RUN_log_now_sim_stop
exec(open("RUN_log_now_sim_stop/input.py").read())

# But disable the sim_stop event
events_manager_verif.manager.base_double_verif.add_to_disable(events_manager_verif.manager.sim_stop_verif)

# Should result in:
# - an error message -- the base_bool_verif event will still try to
#                       subscribe to the now-disabled sim_stop_verif event
# - execution continuing to t=3.0 because sim_stop_verif event will not stop it.

print( '\n'
'*****************************************************************\n'
'ERROR at T=2.0:\n'
'Cannot subscribe to a disabled model\n'
'*****************************************************************\n')

trick.stop(3)
