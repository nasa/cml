exec(open("RUN_no_atmos/input.py").read())
veh1.atmos_exec.gust.start = True
veh1.atmos_exec.gust.direction = [1.0, 0.0, 0.0]
veh1.atmos_exec.gust.maximum_magnitude = 200.0
veh1.atmos_exec.gust.period   = 50.0

veh1.atmos_exec.gust.gust_frame = -1  # invalid option

# Just testing the invalid option, no need to run for very long
trick.stop(20.0)

# Set a trick event to check that gust is disabled since invalid option
# given, must check after initialization
trick.add_read(10.0,"""
if veh1.atmos_exec.use_gust == True:
    trick.exec_terminate_with_return(1, "RUN_invalid_gust/input.py", 16, "Gust should be disabled but is not!")
""")
