from Log_data.logging import log_no_int
log_no_int(0.1)

test_object.faults.enabled = True
test_object.faults.fault_file = "xml_files/random_walk.xml"

print ("\n"
"********************************************************\n"
"WARN - unspecified mean\n"
"********************************************************")


# Change random-walk parameters
trick.add_read(6.0, """
test_object.faults.set_fault_param("Gaussian Random-Walk Fault", "mean", 0.5)
test_object.faults.set_fault_param("Gaussian Random-Walk Fault", "std_dev", 0)
test_object.faults.set_fault_param("Uniform Random-Walk Fault", "min", 0.45)
test_object.faults.set_fault_param("Uniform Random-Walk Fault", "max", 0.55)
test_object.faults.set_fault_param("Uniform Random-Walk Fault", "seed", 0)
""")
# Unrecognized parameter, for code coverage
trick.add_read(7, """
print ("\\n"
"********************************************************\\n"
"ERROR - unrecognized parameter, for code coverage\\n"
"********************************************************")
test_object.faults.set_fault_param("Gaussian Random-Walk Fault", "Invalid", 0.0)
""")

trick.stop(10.0)
