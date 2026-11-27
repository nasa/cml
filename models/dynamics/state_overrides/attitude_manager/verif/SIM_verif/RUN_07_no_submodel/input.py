'''
This test verifies that the vehicle attitude maintain its rotational state if no submodel is subscribed
'''
exec(open("Modified_data/nominalInput.py").read())

log_vehicle_state(rate=1.0)

trick.stop(1.0)