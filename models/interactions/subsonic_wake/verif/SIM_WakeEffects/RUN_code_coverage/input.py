test.wake_with_force.subscribe();
# don't subscribe test.wake_no_force

test.framework.data_file_name = "Unit_test_data/test1_data.txt"
test.framework.vars_file_name = "Unit_test_data/test1_vars.txt"

# turn off force-generation on the wake model
trick.add_read(1,"""
print('*********************************************************************************')
print('Error message -- trying to start force computation on a WakeFollowingBodyNoForce')
print('*********************************************************************************')

test.wake_no_force.objectB.set_generate_force(True)
test.wake_with_force.objectB.set_generate_force(False)
""")


#drop the mach threshold to a high value to trigger the "not fast enough" shutdown
trick.add_read(2,"""
test.params.mach_off = 1.0
""")

trick.add_read(3,"""
print ( 'total-alpha:', test.objectA.get_total_alpha())
print ( 'drag-area:', test.wake_no_force.objectB.get_drag_area())
test.wake_no_force.objectB.compute_force(1.0)
""")


trick.stop(5)
