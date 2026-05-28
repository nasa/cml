# Common Baseline for comparison purposes.
# Generates the profile for the Grow-Hold-Decay time-evolution pattern.
exec(open("Log_data/log_data.py").read())

ts_so.ts.params.RocketHeight = 30
ts_so.ts.params.tau =  5
ts_so.ts.params.taul = 50

ts_so.ts.params.Parallel_Motion_Fast = 0.2
ts_so.ts.params.Normal_Motion_Fast = 0.05
ts_so.ts.params.Parallel_Motion_Slow = 0.4
ts_so.ts.params.Normal_Motion_Slow = 0.2
ts_so.ts.params.Twist_Mag = 0.002

ts_so.ts.params.seed = 30

ts_so.ts.params.delta_T1_max = 30
ts_so.ts.params.delta_T1_min = 10
ts_so.ts.params.delta_T2_max = 60
ts_so.ts.params.delta_T2_min = 50

ts_so.ts.params.mode_freq_high = 0.3
ts_so.ts.params.mode_freq_low = 0.1

ts_so.ts.params.decay_const = 2

trick.stop(200)
