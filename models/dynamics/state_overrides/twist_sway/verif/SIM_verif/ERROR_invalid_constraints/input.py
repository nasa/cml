# Sets the upper bounds to be lower than the lower bounds for:
#  - the periods of the slow and fast perturbations and
#  - the oscillatory freequency.
exec(open("RUN_00_baseline/input.py").read())
print( "\n"
"*********************************************************************\n"
"Error Invalid bounds on distributions.\n"
"2 instances follow.\n"
"*********************************************************************")
ts_so.ts.params.delta_T1_max = -20
ts_so.ts.params.mode_freq_high = -2
