from Log_data.logging import log_no_int
log_no_int(1)

test_object.faults.enabled = True
test_object.replace_values = False
test_object.faults.fault_file = "xml_files/initial_bias.xml"

trick.stop(2)
