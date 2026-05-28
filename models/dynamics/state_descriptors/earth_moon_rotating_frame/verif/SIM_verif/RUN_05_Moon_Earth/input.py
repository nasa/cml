exec(open("RUN_01_Earth_Moon/input.py").read())
test.em_rotating_frame.set_orientation(trick.EarthMoonRotatingFrame.Moon_Earth)
vehicle.rel_state.target_frame_name = "Moon.em_rot"
