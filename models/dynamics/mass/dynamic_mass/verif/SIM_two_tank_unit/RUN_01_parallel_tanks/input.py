exec(open("Log_data/log_data.py").read())
log_test_data( 1.0 )

exec(open("Modified_data/configure_system.py").read())

# Insert specific initial conditions in the Unit_test files:
mass_test.framework.data_file_name = "Unit_test_data/string_data.txt" 
mass_test.framework.vars_file_name = "Unit_test_data/variables.txt" 

trick.stop(15)
