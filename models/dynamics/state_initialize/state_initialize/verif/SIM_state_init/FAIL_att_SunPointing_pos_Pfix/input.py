exec(open("RUN_att_SunPointing/input.py").read())

# Attitude input of SunPointing does not work with Pfix
# Set position input to Pfix to show incompatibility
vehicle.state_initialize.position_input_data_type = trick.StateInitialize.Pfix

vehicle.state_initialize.velocity_input_data_type = trick.StateInitialize.Off
