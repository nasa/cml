from Log_data.logging import log_bool
log_bool(0.1)

test_object.faults.enabled = True
test_object.faults.fault_file = "xml_files/triggers.xml"

trick.add_read(0.3, 'test_object.command = "go"')
trick.add_read(1.7, 'test_object.faults.set_trigger_value("ullong trigger", 10.0)')
trick.add_read(1.9, 'test_object.command = "stop"')

trick.stop(2.0)
