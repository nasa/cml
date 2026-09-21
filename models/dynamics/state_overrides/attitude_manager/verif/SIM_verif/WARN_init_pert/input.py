'''
This test checks that a warning is issued when 
    - enforcing lateral velocity requirements in perturbation submodel
    with an incorrect port name 
    - perturbation smoothing with a zero acceleration
'''
exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_perturbation()


################################################################
# Enforcing Lateral Velocity requirements with incorrect port name.
# Turning off enforceLatVel option
################################################################
test.att_manager.perturbation.set_enforce_lat_vel( True)
test.att_manager.perturbation.portName = "not_a_port_name"

################################################################
# Smoothing with zero acceleration. Turning off smoothing option
################################################################
test.att_manager.perturbation.set_smoothing(True)
test.att_manager.perturbation.deadbandAccelInput[0] = 0.0


trick.stop(0.0)