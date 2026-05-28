exec(open("RUN_equatorial/input.py").read())
# Re-set the velocity to change the orbit from equatorial to polar
vehicle.state_initialize.velocity = [7692.92895761759, 0, 0]
# Set the direction to be pointing in the same direction the vehicle will be traveling, due north
vehicle.range_from_pfix.reference_data.azimuth = 0.0
