def log_test_data ( log_cycle ) :
  recording_group_name =  "test_data"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always
  for body in ['bodyA', 'bodyB']:
    dr_group.add_variable(f"vehicle.{body}.mass.composite_properties.mass")
    for ii in range(0,3) :
      dr_group.add_variable(f"vehicle.{body}.composite_body.state.trans.position[{ii}]" )
      dr_group.add_variable(f"vehicle.{body}.composite_body.state.trans.velocity[{ii}]" )
      dr_group.add_variable(f"vehicle.{body}.core_body.state.trans.position[{ii}]" )
      for jj in range (0,3) :
        dr_group.add_variable(f"vehicle.{body}.composite_body.state.rot.T_parent_this[{ii}][{jj}]" )
      dr_group.add_variable(f"vehicle.{body}.composite_body.state.rot.ang_vel_this[{ii}]" )

  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  return
