from Log_data.logging import log_no_int
log_no_int(0.1)

test_object.faults.enabled = True
test_object.faults.fault_file = "xml_files/errors.xml"

test_object.faults.set_fault_enabled("Invalid", True)
test_object.faults.set_fault_trigger_enabled("Invalid", "trigger", True)
test_object.faults.set_fault_trigger_enabled("Trigger Errors", "Invalid", True)
test_object.faults.set_fault_param("Invalid", "param", 0.0)
test_object.faults.set_trigger_value("Invalid", 0.0)
test_object.faults.set_trigger_value("string trigger", 0.0)

trick.add_read(0.0, """
print ("\\n"
"********************************************************\\n"
"ERROR - unrecognized parameter, for code coverage\\n"
"********************************************************")
test_object.faults.set_fault_param("Valid Fault", "Invalid", 0.0)
""")

trick.stop(0.1)
