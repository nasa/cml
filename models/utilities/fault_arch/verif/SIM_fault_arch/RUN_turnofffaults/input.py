exec(open("Log_data/log_default.py").read())
test_object.faults.fault_file = "xml_files/sinewave_var.xml"

test_object.faults.set_fault_enabled("Test Fault 1", False)
test_object.faults.set_fault_enabled("Test Fault 2", False)
test_object.faults.set_fault_enabled("Test Fault 3", False)

trick.stop(30.0)
