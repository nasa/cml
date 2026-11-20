"""
This test verifies that the vent can be configured with non-randomized high mode duration.
The log data shall output randomized high mode firings with equal 2.0 seconds duration.
"""
exec(open("Modified_data/env.py").read())

two_level_vent_model_obj.vent1.wait_time_randFlag = True
two_level_vent_model_obj.vent1.wait_time_max = 5.0
two_level_vent_model_obj.vent1.wait_time_min = 3.0
two_level_vent_model_obj.vent1.high_duration = 2.0
two_level_vent_model_obj.vent1.dv_max = two_level_vent_model_obj.vent1.accel_mag_high*two_level_vent_model_obj.vent1.high_duration


log_vent_waitTime("vent1")

trick.stop(10)
