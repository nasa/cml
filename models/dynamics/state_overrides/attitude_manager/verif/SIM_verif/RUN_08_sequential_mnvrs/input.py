'''
This test verifies that the maneuver submodel can change the target attitude while an ongoing maneuver.
Smoothing option is on. The maneuvering state is reset, driving the rates back to zero before maneuvering
again to the new target attitude.

The second target attitude quaternion is not normalized beforehand to demonstrate the normalization happens
in the method tested. 
'''

exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_maneuver()
test.att_manager.maneuver.set_smoothing(True)
setTargetAttitude()

trick.add_read(5.0, """
test.quatTargetRefToNominal.scalar =  0.25
test.quatTargetRefToNominal.vector[0] = 0.25
test.quatTargetRefToNominal.vector[1] = 0.25
test.quatTargetRefToNominal.vector[2] = 0.25
test.att_manager.maneuver.set_target_attitude(
test.quatTargetRefToNominal
)""")

log_maneuver_attitude(rate=0.1)

trick.stop(50.0)
