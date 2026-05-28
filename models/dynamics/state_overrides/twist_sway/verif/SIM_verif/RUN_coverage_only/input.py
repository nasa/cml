# Purpose:
# - test the model getter methods
# - test the removal of the slow perturbations after their decay.
exec(open("RUN_00_baseline/input.py").read())

# log perturbation magnitudes, but only want the end point for comparison with
# the values sent to stdout from within the trick-add-read statement below.
log_add_mags()
dr_group.set_cycle(10)

# dial down the slow-perturbation time constants to allow the slow
# perturbations to decay away and be removed. The other runs have the slow
# perturbations with too long of a time constant to decay away in the time-frame
# of the run.
ts_so.ts.params.taul = 0.1
ts_so.ts.params.delta_T2_max = 1.5
ts_so.ts.params.delta_T2_min = 1

stop_time = float(10+ts_so.call_period)
trick.add_read(stop_time,"""
active = ts_so.ts.get_active()
print('\\n'
'*****************************************************************************\\n'
' Test outputs:\\n'
' (check mags with run logs, mags are not available from the input processor)\\n'
' active: %d (1)' %active)


ts_so.test_mags = ts_so.ts.get_fast_mag()
print(
' fast-mags: %f %f %f ' %(ts_so.test_mags.parallel,  ts_so.test_mags.normal, ts_so.test_mags.twist))

ts_so.test_mags = ts_so.ts.get_slow_mag()
print(
' slow-mags: %f %f %f ' %(ts_so.test_mags.parallel,  ts_so.test_mags.normal, ts_so.test_mags.twist))

print(
' net result: ts_so.ts.dp_enu : %f %f %f ' %(ts_so.ts.dp_enu[0], ts_so.ts.dp_enu[1], ts_so.ts.dp_enu[2]))
print(
' net result: ts_so.ts.dv_enu : %f %f %f ' %(ts_so.ts.dv_enu[0], ts_so.ts.dv_enu[1], ts_so.ts.dv_enu[2]))

print('******* Unsubscribe ********')
ts_so.ts.unsubscribe()

print(
' net result: ts_so.ts.dp_enu : %f %f %f ' %(ts_so.ts.dp_enu[0], ts_so.ts.dp_enu[1], ts_so.ts.dp_enu[2]))
print(
' net result: ts_so.ts.dv_enu : %f %f %f ' %(ts_so.ts.dv_enu[0], ts_so.ts.dv_enu[1], ts_so.ts.dv_enu[2]))
ts_so.evaluate_ecef = True
print(
'***********************************************************************')
""")
trick.stop(stop_time+1)
