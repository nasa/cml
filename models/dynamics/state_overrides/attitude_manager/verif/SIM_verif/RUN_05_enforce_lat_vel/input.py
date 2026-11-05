'''
This test verifies that the perturbation submodel can meet lateral velocity requirements 
when specified. 
No smoothing.
'''
exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_perturbation()
test.att_manager.perturbation.set_enforce_lat_vel( True)

log_perturbation_attitude(rate=0.1)
log_lat_vel(rate=0.1)

trick.stop(10.0)