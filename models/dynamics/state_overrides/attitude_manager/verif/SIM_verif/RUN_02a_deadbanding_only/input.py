'''
This test verifies that the perturbation submodel correctly prescribes the deadbanding attitude and rates.
No smoothing. Whenever the attitude deadband limit is reached, switch to opposite rates.
'''
exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_perturbation()

log_perturbation_attitude(rate=0.1)
log_vehicle_state(rate=0.1)

trick.stop(20.0)