exec(open("common_input.py").read())

vehicle.test_vel.subscribe()

log_add_3vec( dr_group, "vehicle.body.composite_body.state.trans.velocity")
log_add_3vec( dr_group, "vehicle.test_vel.variable")

trick.add_read(0,"""
vehicle.test_vel.variable[0] = float(vehicle.body.composite_body.state.trans.velocity[0])
vehicle.test_vel.variable[1] = float(vehicle.body.composite_body.state.trans.velocity[1])
vehicle.test_vel.variable[2] = float(vehicle.body.composite_body.state.trans.velocity[2])
""")
