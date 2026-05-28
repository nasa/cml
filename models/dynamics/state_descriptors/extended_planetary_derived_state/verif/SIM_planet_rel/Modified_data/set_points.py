vehicle.planet_rel_state.pt_to_pt.add_vehicle_point( "v_0",
                                                     [0, 0, 0])
vehicle.planet_rel_state.pt_to_pt.add_vehicle_point( "v_x+1",
                                                     [1, 0, 0])

# The initial pfix (and inertial) position of the CoM is
#    [  385237.8733026918, -5432401.03099659, 3338997.626559414]
# Place the planet points close to this location so the relative position
# between the v-pts and p-pts is initially small and nice round numbers.

vehicle.planet_rel_state.pt_to_pt.add_planet_point( "p_x-1",
                                                    [  385236.8733026918,
                                                      -5432401.03099659,
                                                       3338997.626559414])
vehicle.planet_rel_state.pt_to_pt.add_planet_point( "p_y+1",
                                                    [  385237.8733026918,
                                                      -5432400.03099659,
                                                       3338997.626559414])
vehicle.planet_rel_state.pt_to_pt.add_planet_point( "p_z-2",
                                                    [  385237.8733026918,
                                                      -5432401.03099659,
                                                       3338995.626559414])
vehicle.planet_rel_state.pt_to_pt.make_all_pairings()

vehicle.planet_rel_state.pt_to_pt.add_vehicle_point( "v_y-1",
                                                     [0, -1, 0])

# test the capability to create a new rel-pos, add it to the list, and link its
# position vector to a loggable array.  This is how the ultimate interface
# should appear
vehicle.add_relative_position( "v_y-1",
                               "p_x-1",
                                5)

# test removing one of the rel-pos instances from the list.
vehicle.planet_rel_state.pt_to_pt.remove_relative_position( "v_x+1",
                                                            "p_z-2")
#Should now have 6 relative-positions:
# v_0  with p_x-1, p_y+1, p_z-2
# v_x+1 with p_x-1, p_y+1
# v_y-1 with p_x-1

vehicle.assign_pt_to_pt_positions( "v_0",   "p_x-1", 0)
vehicle.assign_pt_to_pt_positions( "v_0",   "p_y+1", 1)
vehicle.assign_pt_to_pt_positions( "v_0",   "p_z-2", 2)
vehicle.assign_pt_to_pt_positions( "v_x+1", "p_x-1", 3)
vehicle.assign_pt_to_pt_positions( "v_x+1", "p_y+1", 4)
