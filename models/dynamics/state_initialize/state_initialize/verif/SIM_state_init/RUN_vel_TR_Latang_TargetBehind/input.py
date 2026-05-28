exec(open("RUN_vel_TR_Latang/input.py").read())

# Set flag that target is behind the position
# (i.e. velocity defined such that vehicle is moving away from target)
vehicle.state_initialize.TR_param.target_is_behind_vehicle = True
