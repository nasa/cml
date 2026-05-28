exec(open("RUN_3b1_disable_enable/input.py").read())
test.direct_set.act1.disable()
test.indirect_set.act1.disable()

trick.add_read(3,"""
test.direct_set.force_command( test.direct_set.act1)
test.indirect_set.force_command( test.indirect_set.act1)
""")
