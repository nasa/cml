# A test that more realistically sets the frequency of slow perturbations.
# Because the slow parallel magnitude dominates over the fast parallel
# magnitude, the total perturbation follows a tight oscillation about the
# gradual changes of the slow perturbation.
# By having the slow perturbations spaced further apart, having a large
# difference ebtween the two periods provides a fast beating on top of a
# slow underlying perturbation.
exec(open("RUN_01c_GrowDecay/input.py").read())
ts_so.ts.params.taul = 60
ts_so.ts.params.delta_T2_max = 150
ts_so.ts.params.delta_T2_min = 120
