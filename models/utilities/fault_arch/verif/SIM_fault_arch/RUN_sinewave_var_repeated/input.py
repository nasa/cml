exec(open("Log_data/log_default.py").read())
test_object.faults.fault_file = "xml_files/sinewave_var_repeated.xml"

trick.stop(60.0)
