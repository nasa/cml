# This unit test is tests the position dispersion and thrust_unit_motor vector
exec(open("Log_data/log_rocket_motor.py").read())

rocket_motor.mass_init.pt_orientation.data_source = trick.Orientation.InputQuaternion
rocket_motor.mass_init.mass = 100.0 # kg

rocket_motor.vehicle_com = [10, 0, 0] # m

rocket_motor.basic_motor.dispersions.motor_tolerance = trick.attach_units("degree", [0.0, 0.0, 30.0])

#  simulates an offset in the motor frame
rocket_motor.basic_motor.dispersions.position_dispersion = [1,2,3]

rocket_motor.basic_motor.commanded = True

rocket_motor.basic_motor.position = [5, 4, 1]

rocket_motor.basic_motor.thrust_unit_motor = [0.8, 0.0, 0.6]  # Set thrusting direction to be along x- and z-axis

rocket_motor.basic_motor.thrust_magnitude = 1000.0   # N # Set Thrust Magnitude
rocket_motor.basic_motor.mass_flow_rate = 5.0 # kg/s # Set Mass flow rate

rocket_motor.basic_motor.subscribe()

trick.stop(5.0)
