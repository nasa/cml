exec(open( "input_common.py").read())

print('*********** Construct a AeroExecutiveSimple with *****************')
print('************** invalid free-stream-velocity **********************')
aero.new_exec_with_null_fs_vec()

# try an update method; no output b/c it is not active.
aero.executive_NULL_free_stream.update()

trick.add_read(0, """
print('*********** NULL free-stream warning:**************')
aero.executive_NULL_free_stream.set_drag_type( trick.AeroExecutiveSimple.drag_coeff)

print('*******************  type CD: CD   = 0 warning: ***************')
print('*******************     and   Aref = 0 warning: ***************')
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.drag_coeff)

print('********** type constant-drag: mag = 0 warning:***************')
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.constant_mag)

print('*********** type constant-vector: force = 0 warning:**************')
aero.interface.output.force = [0,0,0]
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.constant_vec)

print('*********** type BC: BC small warning:**************')
aero.interface.executive.BC = 1.0E-12
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.ballistic_coeff)

""")

# Set BC to zero to get the failure case.  However, type was left at BC at the
# end of the previous cycle, so first need to switch it to something else
# (cannot "switch" to the same type).
# Also, this will result in  termination, so clean up that pesky "new" first.
trick.add_read(1, """
aero.delete_exec_with_null_fs_vec()

aero.interface.executive.drag_mag = 1.0
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.constant_mag)

print('*********** type BC: BC =0 failure:**************')
aero.interface.executive.BC = 0.0
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.ballistic_coeff)
""")

trick.stop(1)
