exec(open("input_common.py").read())

trick.add_read(0.0, """
print('******** Testing zero-dsl **************')
aero.interface.output.drag_force = 1.0
aero.interface.output.lift_force = 2.0
aero.interface.output.side_force = 3.0
aero.interface.output.LoD = 4.0
print(aero.interface.output.drag_force, aero.interface.output.lift_force, aero.interface.output.side_force, aero.interface.output.LoD)
aero.interface.output.zero_dsl()
print(aero.interface.output.drag_force, aero.interface.output.lift_force, aero.interface.output.side_force, aero.interface.output.LoD)
print('******** Testing zero-torque **************')
aero.interface.output.torque = [1.0, 2.0, 3.0]
aero.interface.output.moment_mrc = [11.0, 12.0, 13.0]
print(aero.interface.output.torque, aero.interface.output.moment_mrc)
aero.interface.output.zero_torque()
print(aero.interface.output.torque, aero.interface.output.moment_mrc)
""")

trick.stop(0)
