"""
This test verifies that the event-action resets the vent schedOnTime parameter and allow
a new "high" mode to start by resetting the vent to low mode:
"""

exec(open("Modified_data/env.py").read())

# The sched_onTime (next high venting) is reset at 8.0 seconds and becomes 13.0 seconds.
# The start_time_high (time when the current high venting started) is reset at 8.0 seconds and becomes 8.0 seconds
trick.add_read(8.0, 
                "two_level_vent_model_obj.vent1.action_reset.specific_execution()")

log_vent_param("vent1", ["sched_onTime", "start_time_high", "high_venting", "force_mag"])

trick.stop(10.0)
