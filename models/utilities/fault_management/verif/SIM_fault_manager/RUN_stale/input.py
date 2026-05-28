from Log_data.logging import log_bool
log_bool(0.1)

test_object.faults.enabled = True
test_object.replace_values = False
test_object.faults.fault_file = "xml_files/stale.xml"

# Test whether the fault resets properly when disabled an re-enabled
trick.add_read(2.0, 'test_object.faults.set_fault_enabled("Stale Fault UShort", False)')
trick.add_read(3.0, 'test_object.faults.set_fault_enabled("Stale Fault UShort", True)')

# Re-enable a fire-limited fault
trick.add_read(2.5, 'test_object.faults.set_fault_enabled("Stale Fault Int", True)')

# Unrecognized parameter, for code coverage
print ("\n"
"********************************************************\n"
"ERROR - unrecognized parameter, for code coverage\n"
"********************************************************")
test_object.faults.set_fault_param("Stale Fault Short", "Invalid", 0.0)

trick.stop(4.0)
