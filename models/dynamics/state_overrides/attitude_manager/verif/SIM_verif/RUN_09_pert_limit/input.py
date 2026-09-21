'''
This test verifies that the perturbation submodel correctly resets the minimum drift rate if it exceeds the
values allowed based on acceleration and deadband limit.
Smoothing option is on.

Setting driftRateMax[0] = 0.25 rad/s : too large
Maximum value allowed is sqrt(2 * 0.01 * 0.01) = 0.0141 rad/s
'''

exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_perturbation()
test.att_manager.perturbation.set_smoothing(True)

test.att_manager.perturbation.driftRateMaxInput[0] = 0.25
################################################################
# This will trigger a warning flag and reset the value.
################################################################

log_perturbation_attitude(rate=0.1)

trick.stop(50.0)