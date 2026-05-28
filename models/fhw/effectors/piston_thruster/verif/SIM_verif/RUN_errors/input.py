exec(open("RUN_mode1/input.py").read())

trick.add_read(0.0,"pistons_unit_test.pistons.unsubscribe()")

trick.add_read(1.0,"""
pistons_unit_test.pistons.add_piston_thruster( pistons_unit_test.pistons.thrusterA)
pistons_unit_test.pistons.add_piston_thruster( pistons_unit_test.extra_piston)
""")

trick.add_read(2.0,"""
pistons_unit_test.pistons.subscribe()
pistons_unit_test.pistons.add_piston_thruster( pistons_unit_test.pistons.thrusterA)
""")

trick.add_read(3.0,"pistons_unit_test.pistons.set_mode(trick.PistonThrusterGroupParams.TimeTableLookup)")
