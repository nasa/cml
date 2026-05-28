# This tests the late addition of a second string.  It should pass silently and
# produce no effect on output data, so data should match that from
# RUN_01_parallel_tanks.
# It also calls DynamicMassString's get_body_collection_size() method for code
# coverage.
exec(open("RUN_01_parallel_tanks/input.py").read())

trick.add_read(1.0,"""
mass_test.group.add_string_to_group( mass_test.second_string)
mass_test.second_string.add_mass_to_string( mass_test.tank_a)
# code coverage for DynamicMassString::get_body_collection_size() method
ret = mass_test.second_string.get_body_collection_size()
print("second_string.get_body_collection_size() returned "+str(ret))
""")
