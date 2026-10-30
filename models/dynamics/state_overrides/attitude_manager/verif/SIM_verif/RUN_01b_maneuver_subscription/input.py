'''
This test verifies that the maneuver submodel is correcly subscribed and unsubscribed when
calling the appropriate methods.
It also checks that the attitude manager deactivation turns off the maneuver submodel.
'''

exec(open("Modified_data/nominalInput.py").read())
setTargetAttitude()
test.att_manager.subscribe_maneuver()
trick.add_read(1.0, 'test.att_manager.unsubscribe_maneuver()')
trick.add_read(2.0, 'test.att_manager.subscribe_maneuver()')
trick.add_read(3.0, 'test.att_manager.unsubscribe()')

log_status(rate=1.0)
log_vehicle_state(rate=1.0)
trick.stop(3.0)
