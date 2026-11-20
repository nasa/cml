"""
This test verifies that the initialization publishes a warning when:
- Randomizing high_duration
- Randomizing high_wait_time
- If:
    - maximum high mode duration is larger than minimum wait time
"""

exec(open("Modified_data/env.py").read())

two_level_vent_model_obj.vent1.high_duration_randFlag = True
two_level_vent_model_obj.vent1.high_duration_min = 1.0
two_level_vent_model_obj.vent1.high_duration_max = 3.0
two_level_vent_model_obj.vent1.dv_max = two_level_vent_model_obj.vent1.accel_mag_high*two_level_vent_model_obj.vent1.high_duration_max;

two_level_vent_model_obj.vent1.wait_time_randFlag = True
two_level_vent_model_obj.vent1.wait_time_min = 2.0
two_level_vent_model_obj.vent1.wait_time_max = 5.0

trick.stop(0.0)
