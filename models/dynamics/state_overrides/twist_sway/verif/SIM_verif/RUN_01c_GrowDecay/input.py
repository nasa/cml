# Generates the profile for the Grow-Decay time-evolution pattern.
exec(open("RUN_01a_GrowHoldDecay/input.py").read())
ts_so.ts.perturb_algorithm = trick.TwistSway.GrowDecay
