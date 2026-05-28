from Log_data.logging import log_no_int
log_no_int(0.1)

test_object.faults.enabled = True
test_object.faults.fault_file = "xml_files/white_noise.xml"

# Change white-noise parameters
trick.add_read(5.0, """
test_object.faults.set_fault_param("Gaussian White-Noise Fault", "mean", 0.3)
test_object.faults.set_fault_param("Gaussian White-Noise Fault", "std_dev", 0.02)
test_object.faults.set_fault_param("Uniform White-Noise Fault", "min", 0.45)
test_object.faults.set_fault_param("Uniform White-Noise Fault", "max", 0.55)
test_object.faults.set_fault_param("Uniform White-Noise Fault", "seed", 0)
""")
# Unrecognized parameter, for code coverage
print ("\n"
"********************************************************\n"
"ERROR - unrecognized parameter, for code coverage\n"
"********************************************************")
test_object.faults.set_fault_param("Gaussian White-Noise Fault", "Invalid", 0.0)

trick.stop(10.0)
