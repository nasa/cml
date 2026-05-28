exec(open("RUN_1a_simple_sequence/input.py").read())
test.indirect_set.disable_all()
test.direct_set.disable_all()

trick.add_read(4,"""
test.indirect_set.enable_all()
test.direct_set.enable_all()
""")

