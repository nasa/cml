# low rate data logging for verif comparison purposes only

def set_group(name, rate):
    recording_group_name = name
    dr_group = trick.sim_services.DRAscii(recording_group_name)
    dr_group.thisown = 0
    dr_group.set_cycle(rate)
    trick.add_data_record_group(dr_group, trick.DR_Buffer)

    return dr_group 

def log_vec3(dr_group, var):
    for idx in range(3):
        dr_group.add_variable(f"{var}[{idx}]")

def log_status(rate):
    dr_group = set_group("submodel_status", rate)

    dr_group.add_variable("test.att_manager.perturbation.active")
    dr_group.add_variable("test.att_manager.maneuver.active")
    dr_group.add_variable("test.att_manager.perturbation.initialized")
    dr_group.add_variable("test.att_manager.maneuver.initialized")
    dr_group.add_variable("test.att_manager.config")

def log_vehicle_state(rate):
    dr_group = set_group("vehicle_state", rate)
    dr_group.add_variable("test.body.composite_body.state.rot.Q_parent_this.scalar")
    log_vec3(dr_group, "test.body.composite_body.state.rot.Q_parent_this.vector")
    log_vec3(dr_group, "test.body.composite_body.state.rot.ang_vel_this")

def log_perturbation_attitude(rate):
    dr_group = set_group("perturbation_state", rate)
    dr_group.add_variable("test.att_manager.perturbation.randomRates")
    log_vec3(dr_group, "test.att_manager.perturbation.prescribedAng")
    log_vec3(dr_group, "test.att_manager.perturbation.driftRate")

def log_maneuver_attitude(rate):
    dr_group = set_group("maneuver_state", rate)
    dr_group.add_variable("test.att_manager.maneuver.active")
    dr_group.add_variable("test.att_manager.maneuver.maneuvering")
    dr_group.add_variable("test.att_manager.maneuver.currentEuler")
    dr_group.add_variable("test.att_manager.maneuver.eulerTarget")
    dr_group.add_variable("test.att_manager.quatBodyWrtRef.scalar")
    log_vec3(dr_group, "test.att_manager.quatBodyWrtRef.vector")
    log_vec3(dr_group, "test.att_manager.maneuver.mnvrRate")

def log_lat_vel(rate):
    dr_group = set_group("lateral_velocity", rate)
    log_vec3(dr_group, "test.att_manager.perturbation.maxDeltaLatVelDPInput")
    log_vec3(dr_group, "test.att_manager.perturbation.deltaLatVelDP")
    
def log_ref_frame_state(rate):
    dr_group = set_group("lvlh_frame", rate)
    log_vec3(dr_group, "test.ref_frame.frame.state.rot.ang_vel_this")
    dr_group.add_variable("test.ref_frame.frame.state.rot.Q_parent_this.scalar")
    log_vec3(dr_group, "test.ref_frame.frame.state.rot.Q_parent_this.vector")



