# The close-out time is set after it has already passed
exec(open("RUN_00_baseline/input.py").read())
print( "\n"
"*********************************************************************\n"
"Error - step-function with immediate shutdown.\n"
"*********************************************************************")
trick.add_read(3,"ts_so.ts.set_end_time(1)")
dr_group.set_cycle(1)
trick.stop(5)
