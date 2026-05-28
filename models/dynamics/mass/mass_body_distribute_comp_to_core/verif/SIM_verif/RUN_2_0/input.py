# This run tests the case where composite-properties are set on body_0 by
# adjusting the core properties of body_2.
# Specifically, this tests the ability to navigate the mass tree beyond a
# simple parent-child relation.
exec(open("RUN_nominal/input.py").read())
trick.add_read(1.0, "so.adjust_2_0.update(  so.new_properties)")
