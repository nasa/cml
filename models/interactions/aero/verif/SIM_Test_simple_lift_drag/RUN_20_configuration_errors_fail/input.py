exec(open("input_common.py").read())

trick.add_read(
    0,
    """
print('*******************  type CD: CD   = 0 warning: ***************')
print('*******************     and   Aref = 0 warning: ***************')
aero.interface.executive.set_aero_type( trick.AeroExecutiveSimpleLiftDrag.lift_drag_coeff)

print('********** type constant-drag: mag = 0 warning:***************')
aero.interface.executive.set_aero_type( trick.AeroExecutiveSimpleLiftDrag.constant_mag)

print('*********** type BC: BC small warning:**************')
aero.interface.executive.BC = 1.0E-12
aero.interface.executive.set_aero_type( trick.AeroExecutiveSimpleLiftDrag.ballistic_coeff_and_LoD)

""",
)

# Set BC to zero to get the failure case.  However, type was left at BC at the
# end of the previous cycle, so first need to switch it to something else
# (cannot "switch" to the same type).
trick.add_read(
    1,
    """
aero.interface.executive.drag_mag = 1.0
aero.interface.executive.set_aero_type( trick.AeroExecutiveSimpleLiftDrag.constant_mag)

print('*********** type BC: BC =0 failure:**************')
aero.interface.executive.BC = 0.0
aero.interface.executive.set_aero_type( trick.AeroExecutiveSimpleLiftDrag.ballistic_coeff_and_LoD)
""",
)

trick.stop(1)
