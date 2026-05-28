exec(open("Log_data/log_data.py").read())
log_test_data( 1.0 )

mass_test.mass_init.mass = 1000.0
mass_test.mass_init.inertia = [[1.0,0.0,0.0],[0.0,1.0,0.0],[0.0,0.0,1.0]]
mass_test.mass_init.position = [0.0,0.0,0.0]
mass_test.mass_init.pt_orientation.data_source = trick.Orientation.InputMatrix
mass_test.mass_init.pt_orientation.trans = [[1,0,0],[0,0,1],[0,-1,0]]

mass_test.tank.residual_mass = 50.0

# Unit_test files:
mass_test.framework.data_file_name = "Unit_test_data/data.txt" 
mass_test.framework.vars_file_name = "Unit_test_data/variables.txt" 

trick.stop(6)
