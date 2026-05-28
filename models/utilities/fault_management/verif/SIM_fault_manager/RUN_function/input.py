from Log_data.logging import log_default
log_default(0.1)

test_object.faults.enabled = True
test_object.faults.fault_file = "xml_files/function.xml"

trick.add_read(40.0,"""
test_object.faults.set_fault_param("Square Fault ULongLong",
                                   "frequency_rate",
                                   -0.01,
                                   True)
""")


# Unrecognized parameter, for code coverage
print ("\n"
"********************************************************\n"
"ERROR - unrecognized parameter, for code coverage\n"
"********************************************************")
test_object.faults.set_fault_param("Linear Fault Char", "Invalid", 0.0)

trick.stop(70.0)
