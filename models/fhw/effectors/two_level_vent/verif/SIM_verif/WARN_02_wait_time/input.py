"""
This test verifies that a warning message is published a warning when:
    - minimum high mode wait time is larger than the maximum high mode wait time
when randomizing wait time, resetting the flag to False
"""
exec(open("Modified_data/env.py").read())

two_level_vent_model_obj.vent1.wait_time_min = 5.0
two_level_vent_model_obj.vent1.wait_time_max = 3.0
two_level_vent_model_obj.vent1.wait_time_randFlag = True

log_vent_param("vent1", ["wait_time_randFlag", "wait_time_max", "wait_time_min", "high_wait_time"])

trick.stop(0.0)