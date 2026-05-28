# This unit test is used as a baseline to make sure the model works as intended
exec(open("Log_data/log_rocket_motor.py").read())
rocket_motor.basic_motor.commanded = True

rocket_motor.mass_init.pt_orientation.data_source = trick.Orientation.InputQuaternion

rocket_motor.vehicle_com = [3.0, 2.0, 1.0] # m
rocket_motor.mass_init.mass = 100.0 # kg

rocket_motor.basic_motor.dispersions.motor_tolerance = trick.attach_units("degree", [10.0, 15.0, 5.0])

# Set motor mass props for single nozzle methodology
rocket_motor.basic_motor.position = [1.0, 2.0, 3.0] # m

# Set thrusting direction
rocket_motor.basic_motor.thrust_unit_motor = [1.0, 0.0, 0.0]

# Set Thrust Magnitude
rocket_motor.basic_motor.thrust_magnitude = 500000.0   # N

# Set Mass flow rate
rocket_motor.basic_motor.mass_flow_rate = 5.0 # kg/s

rocket_motor.basic_motor.subscribe()
trick.stop(5.0)
