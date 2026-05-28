def log_non_root_test_data ( log_cycle ) :
    recording_group_name = "non_root_test_data"
    dr_group = trick.DRAscii(recording_group_name)
    dr_group.thisown = 0
    dr_group.set_cycle(log_cycle)
    dr_group.freq = trick.DR_Always

    dr_group.add_variable("vehicle.bodyE.mass.composite_properties.mass")
    for ii in range(0,3) :
        dr_group.add_variable(f"vehicle.bodyE.composite_body.state.trans.position[{ii}]")
        dr_group.add_variable(f"vehicle.bodyE.composite_body.state.trans.velocity[{ii}]" )
        dr_group.add_variable(f"vehicle.bodyE.core_body.state.trans.position[{ii}]" )
        for jj in range(0,3) :
            dr_group.add_variable(f"vehicle.bodyE.composite_body.state.rot.T_parent_this[{ii}][{jj}]" )
        dr_group.add_variable(f"vehicle.bodyE.composite_body.state.rot.ang_vel_this[{ii}]" )
    
    trick.add_data_record_group(dr_group, trick.DR_Buffer)

    return
