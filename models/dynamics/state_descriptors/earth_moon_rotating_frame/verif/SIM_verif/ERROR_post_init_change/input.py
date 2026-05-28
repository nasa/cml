# This tests the error handling for the case where the EarthMoonRotatingFrame::set_orientation(...)
# gets called after the rotating frame has been initialized.
exec(open("RUN_01_Earth_Moon/input.py").read())
trick.add_read(1, "test.em_rotating_frame.set_orientation(test.em_rotating_frame.Moon__Earth_to_Moon)")
vehicle.rel_state.target_frame_name = "Earth.em_rot"
