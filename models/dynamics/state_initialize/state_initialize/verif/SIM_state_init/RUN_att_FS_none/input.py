exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.EulerFreestream_RYP;

vehicle.state_initialize.angle_of_bank = 0.0
vehicle.state_initialize.angle_of_sideslip =0.0
vehicle.state_initialize.angle_of_attack = 0.0

new_wind = [0,0,0]       # no wind

read = 0
trick.add_read( read, """
vehicle.state_initialize.overwrite_attitude_from_free_stream(new_wind)
""")
