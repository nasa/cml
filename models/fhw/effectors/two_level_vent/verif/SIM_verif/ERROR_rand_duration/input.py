"""
This test verifies that the initialization publishes a warning when:
- Randomizing high_duration
- If:
    - maximum high mode duration is larger than the high mode wait time
"""

exec(open("Modified_data/env.py").read())

two_level_vent_model_obj.vent1.high_duration_randFlag = True
two_level_vent_model_obj.vent1.high_duration_min = 1.0
two_level_vent_model_obj.vent1.high_duration_max = 6.0
two_level_vent_model_obj.vent1.dv_max = two_level_vent_model_obj.vent1.accel_mag_high*two_level_vent_model_obj.vent1.high_duration_max;

trick.stop(0.0)
