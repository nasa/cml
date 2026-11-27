"""
This test verifies that the nominal use case of the two level vents.
Vent 1 has its two time parameters randomized while Vent 2 keeps them fixed.
"""
exec(open("Modified_data/env.py").read())
# Use Default Values in env.py

two_level_vent_model_obj.vent1.wait_time_randFlag     = True
two_level_vent_model_obj.vent1.high_duration_randFlag = True
two_level_vent_model_obj.vent1.wait_time_max          = 5.0
two_level_vent_model_obj.vent1.wait_time_min          = 3.0
two_level_vent_model_obj.vent1.high_duration_min      = 1.0
two_level_vent_model_obj.vent1.high_duration_max      = 2.0

log_all()

trick.stop(10)
