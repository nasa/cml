def log_test_data_base ( log_cycle ) :
  recording_group_name =  "test_data_base"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always
  for ii in range(0,3) :
    dr_group.add_variable("veh_reaction.body.composite_body.state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_reaction.body.composite_body.state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.composite_body.state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.composite_body.state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_reaction.body.core_body.state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_reaction.body.core_body.state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.core_body.state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.core_body.state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.core_body.state.rot.ang_vel_this[" + str(ii) + "]" )
    dr_group.add_variable("veh_reaction.body.core_body.state.rot.ang_vel_this[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.rel_state.rel_state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.rel_state.rel_state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.derivs.non_grav_accel[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.detach.springs.total_force_struc_action[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.detach.springs.total_force_struc_reaction[" + str(ii) + "]" )
  dr_group.add_variable("veh_action.detach.springs.spring_array[0].axial_force" )

  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  return


def log_test_data ( log_cycle ) :
  recording_group_name =  "test_data"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always
  for ii in range(0,3) :
    dr_group.add_variable("veh_reaction.body.composite_body.state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_reaction.body.composite_body.state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.composite_body.state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.composite_body.state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_reaction.body.core_body.state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_reaction.body.core_body.state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.core_body.state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.core_body.state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.core_body.state.rot.ang_vel_this[" + str(ii) + "]" )
    dr_group.add_variable("veh_reaction.body.core_body.state.rot.ang_vel_this[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.rel_state.rel_state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.rel_state.rel_state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.body.derivs.non_grav_accel[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.detach.springs.total_force_struc_action[" + str(ii) + "]" )
    dr_group.add_variable("veh_action.detach.springs.total_force_struc_reaction[" + str(ii) + "]" )
  dr_group.add_variable("veh_action.detach.springs.spring_array[0].axial_force" )
  dr_group.add_variable("veh_action.detach.springs.spring_array[1].axial_force" )

  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  return
