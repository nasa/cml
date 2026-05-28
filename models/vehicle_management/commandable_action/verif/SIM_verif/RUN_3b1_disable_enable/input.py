exec(open("RUN_3a_multi_shot/input.py").read())

trick.add_read(2,"""
test.direct_set.act1.disable()
test.indirect_set.act1.disable()
test.direct_set.act4.disable()
test.indirect_set.act4.disable()
""")

trick.add_read(6,"""
test.direct_set.act4.enable()
test.indirect_set.act4.enable()
""")
