exec(open("Log_data/log_default.py").read())
test_object.faults.fault_file = "xml_files/linear_fault2.xml"

trick.stop(3.0)
