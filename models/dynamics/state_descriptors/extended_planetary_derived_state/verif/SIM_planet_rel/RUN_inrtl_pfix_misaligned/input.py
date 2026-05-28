exec(open("RUN_all_on/input.py").read())

earth.planet.pfix.state.rot.Q_parent_this.scalar = 0
earth.planet.pfix.state.rot.Q_parent_this.vector[2] = 1
earth.planet.pfix.state.rot.T_parent_this[0][0] = -1
earth.planet.pfix.state.rot.T_parent_this[1][1] = -1
vehicle.state_initialize.T_inrtl_body = [[-1,0,0],[0,-1,0],[0,0,1]]
vehicle.planet_rel_state.entry_range.reference_data.position = [-385236.87330269179, 5432401.0309965899, 3338997.6265594144]
vehicle.planet_rel_state.entry_range.reference_data.direction = [258.81402345400585, -3649.6452329420877, 5967.6716544165402]


trick.stop(20)
