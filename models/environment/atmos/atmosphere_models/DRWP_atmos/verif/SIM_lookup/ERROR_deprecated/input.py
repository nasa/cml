exec(open("RUN_01_preload_5_nospec/input.py").read())

luwinds.lookup_table_winds.set_include_vertical_component(True)
luwinds.lookup_table_winds.test_for_reinitialize()

trick.stop(0)
