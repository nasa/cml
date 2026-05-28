exec(open("Log_data/log_default.py").read())
test_object.faults.fault_file = "xml_files/negative_periodic.xml"

trick.stop(10.0)
