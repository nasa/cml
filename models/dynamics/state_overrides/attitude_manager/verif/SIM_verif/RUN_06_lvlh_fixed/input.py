'''
This test verifies that the vehicle attitude can be fixed with respect to some provided reference frame.
In that case, we use the E-LVLH frame. 
'''
exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_perturbation()
test.att_manager.set_ref_frame_state(test.ref_frame.frame.state)

log_vehicle_state(rate=0.1)
log_ref_frame_state(0.1)

trick.stop(20.0)