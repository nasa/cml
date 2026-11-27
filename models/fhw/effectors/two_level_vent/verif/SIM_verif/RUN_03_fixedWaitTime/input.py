"""
This test verifies that the vent can be configured with non-randomized wait time.
The log data shall output periodic high mode firings every 4.0 seconds.
"""
exec(open("Modified_data/env.py").read())

two_level_vent_model_obj.vent1.high_duration_randFlag = True
two_level_vent_model_obj.vent1.high_duration_max = 3.0
two_level_vent_model_obj.vent1.high_duration_min = 1.0
two_level_vent_model_obj.vent1.high_wait_time = 4.0
two_level_vent_model_obj.vent1.dv_max = two_level_vent_model_obj.vent1.accel_mag_high*two_level_vent_model_obj.vent1.high_duration_max;

log_vent_dur("vent1")

trick.stop(10)
