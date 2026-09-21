'''
This test verifies that the perturbation submodel prescribes the deadbanding attitude and 
with random rates.
With smoothing. Whenever the attitude deadband limit is reached, randomize rate with
opposite sign and switch.
'''
exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_perturbation()
test.att_manager.perturbation.randomRates = True
test.att_manager.perturbation.set_smoothing(True)

log_perturbation_attitude(rate=0.1)

trick.stop(40.0)