exec(open("Log_data/log_default.py").read())
test_object.faults.fault_file = "xml_files/sinewave_var.xml"

test_object.faults.set_fault_trigger_enabled("Test Fault 1", "trigger 1", False)
test_object.faults.set_fault_trigger_enabled("Test Fault 1", "trigger 2", False)
test_object.faults.set_fault_trigger_enabled("Test Fault 2", "trigger 3", False)
test_object.faults.set_fault_trigger_enabled("Test Fault 2", "trigger 4", False)
test_object.faults.set_fault_trigger_enabled("Test Fault 3", "trigger 5", False)

trick.stop(30.0)
