from Log_data.logging import log_bool
log_bool(0.1)

test_object.faults.enabled = True
test_object.faults.fault_file = "xml_files/overwrite.xml"

# Change an overwrite value
trick.add_read(2.0, 'test_object.faults.set_fault_param("Overwrite Fault ULongLong", "value", 40)')
# Unrecognized parameter, for code coverage
print ("\n"
"********************************************************\n"
"ERROR - unrecognized parameter, for code coverage\n"
"********************************************************")
test_object.faults.set_fault_param("Overwrite Fault Short", "Invalid", 0.0)

trick.stop(3.0)
