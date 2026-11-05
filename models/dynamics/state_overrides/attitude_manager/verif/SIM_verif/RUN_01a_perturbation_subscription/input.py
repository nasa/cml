'''
This test verifies that the perturbation submodel is correcly subscribed and unsubscribed when
calling the appropriate methods.
It also checks that the attitude manager deactivation turns off the perturbation submodel.
'''

exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_perturbation()
trick.add_read(1.0, 'test.att_manager.unsubscribe_perturbation()')
trick.add_read(2.0, 'test.att_manager.subscribe_perturbation()')
trick.add_read(3.0, 'test.att_manager.unsubscribe()')
log_status(rate=1.0)
log_vehicle_state(rate=1.0)

trick.stop(3.0)
