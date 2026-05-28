exec(open("RUN_01_Earth_Moon/input.py").read())
test.em_rotating_frame.set_orientation(trick.EarthMoonRotatingFrame.EMB_Moon)
vehicle.rel_state.target_frame_name = "EMBary.em_rot"
