from Log_data.logging import log_no_int
log_no_int(0.1)

test_object.faults.enabled = True
test_object.faults.fault_file = "xml_files/trigger_limited.xml"

trick.add_read(0.7, "test_object.faults.unset_trigger_count('trigger')")

trick.stop(0.8)
