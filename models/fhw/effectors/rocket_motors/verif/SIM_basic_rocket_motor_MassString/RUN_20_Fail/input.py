# This unit test does not add any bodies to the mass string
#   This run will fail
exec(open("Log_data/log_rocket_motor.py").read())
rocket_motor.basic_motor.commanded = True

rocket_motor.mass_init.pt_orientation.data_source = trick.Orientation.InputQuaternion
rocket_motor.mass_init.mass = 10

rocket_motor.vehicle_com = [3.0, 2.0, 1.0]

# No bodies attached
rocket_motor.motor_prop_string.flow_down = False

# Set motor mass props for single nozzle methodology
rocket_motor.basic_motor.position = [1.0, 2.0, 3.0]

# Set thrusting direction
rocket_motor.basic_motor.thrust_unit_motor = [1.0, 0.0, 0.0]

# Set Thrust Magnitude
rocket_motor.basic_motor.thrust_magnitude = 50.0   # N

# Set Mass flow rate
rocket_motor.basic_motor.mass_flow_rate = 3.0

rocket_motor.basic_motor.subscribe()
trick.stop(8)
