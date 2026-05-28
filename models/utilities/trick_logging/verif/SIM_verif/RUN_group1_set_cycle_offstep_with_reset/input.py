exec(open("RUN_group1_set_cycle_offstep/input.py").read())
#In the base run, the logging frequency was changed at t=3.0s to 10Hz, but that
#does not get applied until the next scheduled logging operation, which does not
#occur until t=4.0s.

# This time, drop a log point at t=3.456. This will reset the next call time to be
# the next interval of the cycle-time.  Cycle-time = 0.1s, so the next interval
# is 3.5s.
trick.add_read(3.456,"trick_logging_verif.dr_groups.log_now('group1_verif')")

# reset back to 1.0s. Logging will resume at 5.0.
trick.add_read(4.2,"trick_logging_verif.dr_groups.set_cycle(1.0, 'group1_verif')")

trick.stop(8)