from Log_data.logging import log_default
log_default(0.1)

test_object.faults.enabled = True
test_object.faults.fault_file = "xml_files/scale.xml"

# Change a scale value
trick.add_read(2.0, 'test_object.faults.set_fault_param("Scale Fault Short", "scale_factor", 100)')
# Unrecognized parameter, for code coverage
print ("\n"
"********************************************************\n"
"ERROR - unrecognized parameter, for code coverage\n"
"********************************************************")
test_object.faults.set_fault_param("Scale Fault Short", "Invalid", 0.0)

trick.stop(3.0)
