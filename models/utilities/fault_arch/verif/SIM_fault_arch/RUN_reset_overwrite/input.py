exec(open("Log_data/log_default.py").read())
test_object.faults.fault_file = "xml_files/reset_overwrite.xml"

test_object.faults.set_fault_param("Test Fault 1", "value", 40)

trick.stop(10.0)
