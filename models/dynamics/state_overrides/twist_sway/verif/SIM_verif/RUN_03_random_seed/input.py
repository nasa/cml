# Used to compare against RUN_00, using a different seed for the
# random number generator.
exec(open("RUN_00_baseline/input.py").read())
ts_so.ts.params.seed = 1
