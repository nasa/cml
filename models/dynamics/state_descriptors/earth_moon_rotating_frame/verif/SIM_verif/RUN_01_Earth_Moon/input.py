# RUN_01 through RUN_06 test a static vehicle while the Earth-Moon complete
# a complete rotation.
# The unit-test-data provide the state of Moon and Earth; these values were
# generated in advance from a simple sim that simply logged the planet states
# at intervals of 100,000 s
exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_data.py").read())

test.em_rotating_frame.subscribe()
test.em_rotating_frame.set_orientation(trick.EarthMoonRotatingFrame.Earth_Moon)
vehicle.rel_state.target_frame_name = "Earth.em_rot"

test.framework.data_file_name = "Unit_test_data/ephem_data.txt"
test.framework.vars_file_name = "Unit_test_data/ephem_variables.txt"

trick.stop(26)
