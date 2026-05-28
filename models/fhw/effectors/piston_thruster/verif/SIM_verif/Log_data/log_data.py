def log_test_data ( log_cycle ) :
  recording_group_name =  "test_data"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always

  def log_3vec( variable ) :
    for ii in range(3) :
      dr_group.add_variable("%s[%d]" %(variable, ii))

  dr_group.add_variable("pistons_unit_test.time" )
  dr_group.add_variable("pistons_unit_test.pistons.param.mode" )

  log_3vec("pistons_unit_test.pistons.out_A.force")
  log_3vec("pistons_unit_test.pistons.out_A.moment")
  log_3vec("pistons_unit_test.pistons.out_B.force")
  log_3vec("pistons_unit_test.pistons.out_B.moment")

  log_3vec("pistons_unit_test.pistons.thrusterA.sideA.out.force")
  log_3vec("pistons_unit_test.pistons.thrusterA.sideA.out.moment")
  log_3vec("pistons_unit_test.pistons.thrusterA.sideB.out.force")
  log_3vec("pistons_unit_test.pistons.thrusterA.sideB.out.moment")

  log_3vec("pistons_unit_test.pistons.thrusterB.sideA.out.force")
  log_3vec("pistons_unit_test.pistons.thrusterB.sideA.out.moment")
  log_3vec("pistons_unit_test.pistons.thrusterB.sideB.out.force")
  log_3vec("pistons_unit_test.pistons.thrusterB.sideB.out.moment")

  log_3vec("pistons_unit_test.pistons.thrusterC.sideA.out.force")
  log_3vec("pistons_unit_test.pistons.thrusterC.sideA.out.moment")
  log_3vec("pistons_unit_test.pistons.thrusterC.sideB.out.force")
  log_3vec("pistons_unit_test.pistons.thrusterC.sideB.out.moment")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  return
