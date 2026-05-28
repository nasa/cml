# The amplitudes of the perturbations grow to be larger than the hight of the
# rocket, which is not physically possible.
exec(open("RUN_00_baseline/input.py").read())
print( "\n"
"*********************************************************************\n"
"Error: Twist sway larger than reference length.\n"
"*********************************************************************")
ts_so.ts.params.RocketHeight = 0.1
trick.stop(17)
