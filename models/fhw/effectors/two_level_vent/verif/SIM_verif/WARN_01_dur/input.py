"""
This test verifies that a warning message is published when:
    - minimum high mode duration is larger than the maximum high mode duration
when randomizing high duration, resetting the flag to False
"""
exec(open("Modified_data/env.py").read())

two_level_vent_model_obj.vent1.high_duration_min = 2.0
two_level_vent_model_obj.vent1.high_duration_max = 1.0
two_level_vent_model_obj.vent1.high_duration_randFlag = True

log_vent_param("vent1", ["high_duration_randFlag", "high_duration_max", "high_duration_min", 'high_duration'])

trick.stop(0.0)