# This run tests the case where composite-properties are set on body_1 by
# adjusting the core properties of body_0.
# Because body_1 is a child of body_0, adjusting properties of body_0 cannot
# affect composite-properties of body_1.  This should result in an error
# message, abort of the process, and data that matches the nominal case.
exec(open("RUN_nominal/input.py").read())
trick.add_read(1.0, "so.adjust_0_1.update( so.new_properties)")
