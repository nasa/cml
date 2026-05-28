# This run tests the case where composite-properties are set on body_0 by
# adjusting the core properties of the same body.
exec(open("RUN_nominal/input.py").read())
trick.add_read(1.0, "so.adjust_0_0.update( so.new_properties)")
