def log_verif(log_cycle):
    recording_group_name = "verif"
    dr_group = trick.DRAscii(recording_group_name)
    dr_group.thisown = 0
    dr_group.set_cycle(log_cycle)
    dr_group.freq = trick.DR_Always
    trick.add_data_record_group(dr_group, trick.DR_Buffer)

    def log_add_3vec(var):
        for i in range(3):
            dr_group.add_variable(var + "[%d]" %i)

    def log_add_3x3mat(var):
        for i in range(3):
           log_add_3vec(var + "[%d]" %i)

    # Mass properties
    veh1_dynbody = "vehicle.dyn_body"
    veh2_dynbody = "vehicle2.dyn_body"
    veh1_massbody = "dummy_veh_launcher_verif.mass_body" 
    dr_group.add_variable(veh1_massbody+".composite_properties.mass")
    dr_group.add_variable(veh2_dynbody+".mass.composite_properties.mass")
    dr_group.add_variable(veh2_dynbody+".mass.core_properties.mass")
    
    log_add_3vec(veh1_massbody+".composite_properties.position")
    log_add_3vec(veh2_dynbody+".mass.composite_properties.position")
    log_add_3vec(veh2_dynbody+".mass.core_properties.position")

    log_add_3x3mat(veh1_massbody+".composite_properties.inertia")
    log_add_3x3mat(veh2_dynbody+".mass.core_properties.inertia")
    log_add_3x3mat(veh2_dynbody+".mass.composite_properties.inertia")
    
    # State properties
    log_add_3vec(veh1_dynbody+".composite_body.state.trans.position")
    log_add_3vec(veh2_dynbody+".composite_body.state.trans.position")
    log_add_3vec(veh2_dynbody+".core_body.state.trans.position")
    log_add_3vec(veh1_dynbody+".composite_body.state.trans.velocity")
    log_add_3vec(veh2_dynbody+".composite_body.state.trans.velocity")
    log_add_3vec(veh2_dynbody+".core_body.state.trans.velocity")
    log_add_3vec(veh1_dynbody+".derivs.trans_accel")
    log_add_3vec(veh2_dynbody+".derivs.trans_accel")
    log_add_3vec(veh1_dynbody+".derivs.non_grav_accel")
    log_add_3vec(veh2_dynbody+".derivs.non_grav_accel")

    return
