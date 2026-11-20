"""
This test verifies that an error message is published a warning when:
- the maximum delta-V is reached, resetting the high mode duration to match maximum delta-V
"""
exec(open("Modified_data/env.py").read())

two_level_vent_model_obj.vent2.dv_max  = 3.0

# Calculated delta-V is equal to 6.0 m/s > 3.0 m/s
# Resetting high duration to 1.0s
log_vent_param("vent2", ['high_duration','dv_mag','dv_max','high_venting','force_mag'])

trick.stop(1.0)
