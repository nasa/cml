"""
This test verifies that the model fails when:
- the user tries to get a two-level vent from a simple vent index.
"""

exec(open("Modified_data/env.py").read())

venting_dir = [1.0, 0.0, 0.0]
two_level_vent_model_obj.vent3.set_direction(venting_dir)
two_level_vent_model_obj.vent3.set_force_magnitude(0.0)
two_level_vent_model_obj.vent3.set_duration(1.0)

two_level_vent_model_obj.vent3.location[0] = 0.0
two_level_vent_model_obj.vent3.location[1] = 1.0
two_level_vent_model_obj.vent3.location[2] = 0.0

two_level_vent_model_obj.two_level_vent_set1.add_vent(two_level_vent_model_obj.vent3)

two_level_vent_model_obj.two_level_vent_set1.get_two_level_vent(2)

trick.stop(0)
