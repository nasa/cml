'''
This test verifies that the maneuver and perturbation submodels can be applied sequentially to
reach a desired attitude and deadband about it.
No smoothing.
Desired attitude (RPY): Roll = 30deg ; Pitch = 0deg ; Yaw = 0deg
Resulting Quaternion: [0.96592583, -0.25881905, 0.0, 0.0]
'''
exec(open("Modified_data/nominalInput.py").read())

setTargetAttitude()
test.att_manager.subscribe_maneuver()
test.att_manager.subscribe_perturbation()

log_maneuver_attitude(rate=0.1)
log_perturbation_attitude(rate=0.1)
log_status(rate=0.1)

trick.stop(20.0)