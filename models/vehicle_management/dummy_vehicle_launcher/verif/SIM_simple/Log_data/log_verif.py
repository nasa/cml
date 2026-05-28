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

    launch_body = "dummy_veh_launcher_verif.launch_body"
    state_body = "dummy_veh_launcher_verif.state_body"
    mass_body = "dummy_veh_launcher_verif.mass_body"

    # Mass properties
    dr_group.add_variable(mass_body+".composite_properties.mass")
    dr_group.add_variable(launch_body+".mass.composite_properties.mass")
    dr_group.add_variable(launch_body+".mass.core_properties.mass")
    
    # Mass body state
    log_add_3vec(mass_body+".composite_properties.position")
    log_add_3vec(launch_body+".mass.composite_properties.position")
    log_add_3vec(launch_body+".mass.core_properties.position")
    log_add_3x3mat(mass_body+".composite_properties.inertia")
    log_add_3x3mat(launch_body+".mass.composite_properties.inertia")
    log_add_3x3mat(launch_body+".mass.core_properties.inertia")
    
    # State body state
    log_add_3vec(state_body+".composite_body.state.trans.position")
    log_add_3vec(state_body+".composite_body.state.trans.velocity")
    log_add_3vec(state_body+".derivs.trans_accel")
    log_add_3vec(state_body+".derivs.non_grav_accel")
    log_add_3vec(launch_body+".composite_body.state.trans.position")
    log_add_3vec(launch_body+".core_body.state.trans.position")
    log_add_3vec(launch_body+".composite_body.state.trans.velocity")
    log_add_3vec(launch_body+".core_body.state.trans.velocity")
    log_add_3vec(launch_body+".derivs.trans_accel")
    log_add_3vec(launch_body+".derivs.non_grav_accel")

    # Rotational state properties
    log_add_3vec(state_body+".composite_body.state.rot.ang_vel_this")
    log_add_3vec(launch_body+".composite_body.state.rot.ang_vel_this")
    log_add_3vec(state_body+".core_body.state.rot.ang_vel_this")
    log_add_3vec(launch_body+".core_body.state.rot.ang_vel_this")
    log_add_3vec(state_body+".composite_body.state.rot.Q_parent_this.vector")
    log_add_3vec(launch_body+".composite_body.state.rot.Q_parent_this.vector")
    dr_group.add_variable(state_body+".composite_body.state.rot.Q_parent_this.scalar")
    dr_group.add_variable(launch_body+".composite_body.state.rot.Q_parent_this.scalar")
    log_add_3vec(state_body+".core_body.state.rot.Q_parent_this.vector")
    log_add_3vec(launch_body+".core_body.state.rot.Q_parent_this.vector")
    dr_group.add_variable(state_body+".core_body.state.rot.Q_parent_this.scalar")
    dr_group.add_variable(launch_body+".core_body.state.rot.Q_parent_this.scalar")
    log_add_3x3mat(state_body+".composite_body.state.rot.T_parent_this")
    log_add_3x3mat(state_body+".core_body.state.rot.T_parent_this")
    log_add_3x3mat(launch_body+".composite_body.state.rot.T_parent_this")
    log_add_3x3mat(launch_body+".core_body.state.rot.T_parent_this")
    log_add_3vec(state_body+".derivs.rot_accel")
    log_add_3vec(launch_body+".derivs.rot_accel")
