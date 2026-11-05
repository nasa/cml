'''
This test verifies that the maneuver submodel correctly prescribes attitude and rates to
reach the desired attitude.
No smoothing.

Desired attitude (RPY): Roll = 30deg ; Pitch = 0deg ; Yaw = 0deg
Resulting Quaternion: [0.96592583, -0.25881905, 0.0, 0.0]
'''
exec(open("Modified_data/nominalInput.py").read())

setTargetAttitude()

test.att_manager.subscribe_maneuver()

log_maneuver_attitude(rate=0.1)

trick.stop(10.0)