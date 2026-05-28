test.wake_with_force.subscribe();
test.wake_no_force.subscribe()

test.wake_no_force.rev_flow.disable()
test.wake_no_force.prf_model.disable()

test.framework.data_file_name = "Unit_test_data/test1_data.txt"
test.framework.vars_file_name = "Unit_test_data/test1_vars.txt"


trick.add_read(1,"""
print('************************************************')
print('Terminal fault -- set freestream velocity to 0')
print('************************************************')
test.freestream_vel_bodyA = [0,0,0]
test.objectA.compute_freestream_vel_mag()
""")


trick.stop(5)
