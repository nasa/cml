exec(open("Modified_data/env_setup.py").read())
exec(open("Modified_data/set_points.py").read())
exec(open("Log_data/log_base.py").read())
vehicle.planet_rel_state.subscribe()
vehicle.planet_rel_state.subscribe_pt_to_pt()

print(
'****************************************************************************\n'
' Adding a point with NULL position:\n'
'****************************************************************************')
vehicle.add_null_point()

print(
'****************************************************************************\n'
' Adding a veh-pt with empty name:\n'
'****************************************************************************')
vehicle.planet_rel_state.pt_to_pt.add_vehicle_point( "",
                                                     [1, 1, 1])
print(
'****************************************************************************\n'
' Adding a veh-pt with duplicated name:\n'
'****************************************************************************')
vehicle.planet_rel_state.pt_to_pt.add_vehicle_point( "v_0",
                                                     [1, 1, 1])
print(
'****************************************************************************\n'
' Adding a planet-pt with empty name:\n'
'****************************************************************************')
vehicle.planet_rel_state.pt_to_pt.add_planet_point( "",
                                                     [1, 1, 1])
print(
'****************************************************************************\n'
' Adding a planet-pt with duplicated name:\n'
'****************************************************************************')
vehicle.planet_rel_state.pt_to_pt.add_planet_point( "p_x-1",
                                                     [1, 1, 1])
print(
'****************************************************************************\n'
' Adding a rel-pos with duplicated names:\n'
'****************************************************************************')
vehicle.planet_rel_state.pt_to_pt.add_relative_position( "v_0",
                                                         "p_x-1")
print(
'****************************************************************************\n'
' Adding a rel-pos with unmatched names:\n'
'****************************************************************************')
vehicle.planet_rel_state.pt_to_pt.add_relative_position( "v_5",
                                                         "p_x-1")
print(
'****************************************************************************\n'
' Getting a rel-pos with unmatched names:\n'
'****************************************************************************')
vehicle.planet_rel_state.pt_to_pt.get_relative_position( "v_5",
                                                         "p_x-1")
print(
'****************************************************************************\n'
' Removing a rel-pos with unmatched names:\n'
'****************************************************************************')
vehicle.planet_rel_state.pt_to_pt.remove_relative_position( "v_5",
                                                            "p_x-1")


print(
'****************************************************************************\n'
'Checking make_all_pairings()')
print( 'Starting with %d relative positions'
       %vehicle.planet_rel_state.pt_to_pt.get_num_rel_pos_instances())
vehicle.planet_rel_state.pt_to_pt.make_all_pairings()
print( 'Run make_all_pairings():\n'
       '    Should have all available relative positions:\n'
       '    Have ** %d of 9 **'
       %vehicle.planet_rel_state.pt_to_pt.get_num_rel_pos_instances())
vehicle.planet_rel_state.pt_to_pt.make_all_pairings()
print( 'Rerun make_all_pairings():\n'
       '    Should still have all available relative positions, no duplicates\n'
       '    Have ** %d of 9 **'
       %vehicle.planet_rel_state.pt_to_pt.get_num_rel_pos_instances())
print( 'Should have passed over duplicates silently\n'
'****************************************************************************\n')


trick.stop(0)
