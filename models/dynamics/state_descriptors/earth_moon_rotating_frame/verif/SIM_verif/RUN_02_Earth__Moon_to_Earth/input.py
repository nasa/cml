exec(open("RUN_01_Earth_Moon/input.py").read())
test.em_rotating_frame.set_orientation(trick.EarthMoonRotatingFrame.Earth__Moon_to_Earth)
vehicle.rel_state.target_frame_name = "Earth.em_rot"
