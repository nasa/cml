'''
Same run as RUN_02c_db_and_mnvr with smoothing.

Desired attitude (RPY): Roll = 30deg ; Pitch = 0deg ; Yaw = 0deg
Resulting Quaternion: [0.96592583, -0.25881905, 0.0, 0.0]
'''
exec(open("Modified_data/nominalInput.py").read())

setTargetAttitude()
test.att_manager.subscribe_perturbation()
trick.add_read(20.0, "test.att_manager.subscribe_maneuver()")
test.att_manager.maneuver.set_smoothing(True)
test.att_manager.perturbation.set_smoothing(True)

log_maneuver_attitude(rate=0.05)
log_perturbation_attitude(rate=0.05)

trick.stop(50.0)