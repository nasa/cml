# This run is based on its equivalent FAIL case.
# Instead of loading the corrupt data file prior to initialziation (which
# causes a terminal fault), the corrupt file is loaded post initialization,
# with a good file already loaded. This changes the fault signature from
# terminal failure to error.
exec(open( "input_common.py").read())
luwinds.lookup_table_winds.load_DRWP_file("Binaries/DRWP_no_w_comp.bin", False, 1771)
trick.CMLMessage.set_publish_level( trick.CMLMessage.Error)


trick.add_read(0,"""
luwinds.lookup_table_winds.load_DRWP_file("Binaries/corrupt_binaries/no_number_of_profiles.bin", True, 11)
""")

trick.stop(0)
