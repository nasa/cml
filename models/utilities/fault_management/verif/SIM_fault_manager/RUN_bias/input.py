from Log_data.logging import log_default
log_default(0.1)

test_object.faults.enabled = True
test_object.faults.fault_file = "xml_files/bias.xml"

# Change a trigger value
trick.add_read(3.0, 'test_object.faults.set_trigger_value("bias trigger", 4.0)')

# Enable a fault using set_fault_enabled
trick.add_read(5.0, 'test_object.faults.set_fault_enabled("Bias Fault Float", True)')

# Disable and re-enable a trigger
test_object.faults.set_fault_trigger_enabled("Bias Fault UInt", "bias trigger", False)
trick.add_read(5.0, """
test_object.faults.set_fault_trigger_enabled("Bias Fault UInt", "bias trigger", True)
""")

# Change a bias value
trick.add_read(5.0, 'test_object.faults.set_fault_param("Bias Fault Short", "bias", 100)')
# Unrecognized parameter, for code coverage
print ("\n"
"********************************************************\n"
"ERROR - unrecognized parameter, for code coverage\n"
"********************************************************")
test_object.faults.set_fault_param("Bias Fault Short", "Invalid", 0.0)

trick.stop(6.0)
