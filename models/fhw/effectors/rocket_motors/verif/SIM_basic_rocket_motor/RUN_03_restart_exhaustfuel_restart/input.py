# This unit test is used as a baseline to make sure the model works as intended
exec(open("Log_data/log_rocket_motor.py").read())

rocket_motor.mass_init.pt_orientation.data_source = trick.Orientation.InputQuaternion
rocket_motor.mass_init.mass = 30.0 # kg

rocket_motor.vehicle_com = [5.0, 0.0, 0.0] # m

rocket_motor.basic_motor.commanded = True
rocket_motor.basic_motor.motor_can_be_shutdown = True
trick.add_read(4.0, """rocket_motor.basic_motor.commanded = False""")
trick.add_read(6.0, """rocket_motor.basic_motor.commanded = True""")
trick.add_read(12.0, """rocket_motor.basic_motor.commanded = True""")

rocket_motor.basic_motor.position = [0.0, 0.0, 0.0]
rocket_motor.basic_motor.thrust_unit_motor = [1.0, 0.0, 0.0]
rocket_motor.basic_motor.thrust_magnitude = 1000.0
rocket_motor.basic_motor.mass_flow_rate = 5.0

rocket_motor.basic_motor.subscribe()

trick.stop(14.0)
