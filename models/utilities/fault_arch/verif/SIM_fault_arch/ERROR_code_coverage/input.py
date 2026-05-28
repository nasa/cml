exec(open("Log_data/log_default.py").read())
test_object.faults.fault_file = "xml_files/error.xml"

test_object.faults.Injection(7)

trick.stop(0.0)
