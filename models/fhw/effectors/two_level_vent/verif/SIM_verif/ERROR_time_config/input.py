"""
This test verifies that the initialization publishes a warning when:
No randomization
- the high mode duration is larger than active wait time
"""

exec(open("Modified_data/env.py").read())

two_level_vent_model_obj.vent1.high_duration = 2.0
two_level_vent_model_obj.vent1.high_wait_time = 1.0

trick.stop(0.0)
