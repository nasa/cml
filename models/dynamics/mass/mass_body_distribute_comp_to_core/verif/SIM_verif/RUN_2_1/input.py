# This run tests the case where composite-properties are set on body_1 by
# adjusting the core properties of body_2.
# Specifically, this tests confirms that composite-properties further up the tree
# (i.e. body_0, body_1's parent) are affected by manipulation further down the
# tree.
exec(open("RUN_nominal/input.py").read())
trick.add_read(1.0, "so.adjust_2_1.update( so.new_properties)")
