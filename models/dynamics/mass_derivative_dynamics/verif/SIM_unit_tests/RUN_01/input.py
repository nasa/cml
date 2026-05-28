# This run tests the effect of releasing the mass along the x-axis while it is
# spinning about the z-axis.  Consequential torque should be about the z-axis.
# The propellant CoM is offset from the vehicle CoM to ensure that the
# propellant offset is accounted for.  delta-Inertia ~ (10^2) - (1^2) = 99.
exec(open( "Log_data/log_data.py").read())
add_mass_outputs()

# Vary mass only
test.framework.data_file_name = "Unit_test_data/RUN_01.txt"
test.framework.vars_file_name = "Unit_test_data/variables.txt"

test.inertia = [[1,0,0],[0,1,0],[0,0,1]]
test.R_nozzle = [10,0,0]
test.R_propellant = [1,0,0]
test.body.mass.composite_properties.position = [0,0,0]
test.body.mass.composite_properties.T_parent_this = [[1,0,0],[0,1,0],[0,0,1]]
test.body.composite_body.state.rot.ang_vel_this = [0,0,1]

test.test_dyn.subscribe()

trick.stop(10)
