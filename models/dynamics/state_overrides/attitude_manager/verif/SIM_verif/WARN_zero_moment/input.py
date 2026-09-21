'''
This test checks that a warning is issued when 
    - enforcing lateral velocity requirements and moment arm x-axis
    component is close to zero.
    - moment-arm is not meeting requirements (rx larger than ry and rz)
'''
exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_perturbation()


################################################################
# Enforcing Lateral Velocity requirements with incorrect port name.
# Turning off enforceLatVel option
################################################################
test.att_manager.perturbation.set_enforce_lat_vel( True)
test.mass_init.get_mass_point(0).position = trick.attach_units("m", [0.0, 1.0, 0.0])


trick.stop(0.0)