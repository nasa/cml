def log_data ( log_cycle ) :
  dr_group = trick.DRAscii("oe_verif")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always

  def log_add_3vec(variable) :
    for ii in range(3) :
      dr_group.add_variable(variable + "[%d]" %ii)

  log_add_3vec("oe_verif.body.composite_body.state.trans.position")
  log_add_3vec("oe_verif.body.composite_body.state.trans.velocity")
  log_add_3vec("oe_verif.orb_elems.position")
  log_add_3vec("oe_verif.orb_elems.velocity")
  log_add_3vec("oe_verif.orb_elems.position_unit")
  log_add_3vec("oe_verif.orb_elems.vel_vertical")
  log_add_3vec("oe_verif.orb_elems.vel_horizontal_unit")
  log_add_3vec("oe_verif.orb_elems.ang_momentum")
  log_add_3vec("oe_verif.orb_elems.node_line")

  dr_group.add_variable("oe_verif.orb_elems.position_mag" )
  dr_group.add_variable("oe_verif.orb_elems.vel_vertical_scalar" )

  dr_group.add_variable("oe_verif.orb_elems.semi_major_axis" )
  dr_group.add_variable("oe_verif.orb_elems.semi_parameter" )
  dr_group.add_variable("oe_verif.orb_elems.ecc_mag" )
  dr_group.add_variable("oe_verif.orb_elems.ecc_anomaly" )
  dr_group.add_variable("oe_verif.orb_elems.true_anomaly" )
  dr_group.add_variable("oe_verif.orb_elems.arg_periapsis" )
  dr_group.add_variable("oe_verif.orb_elems.mean_motion" )
  dr_group.add_variable("oe_verif.orb_elems.orbit_type" )
  dr_group.add_variable("oe_verif.orb_elems.spec_energy" )

  dr_group.add_variable("oe_verif.orb_elems.cos_true_anomaly" )
  dr_group.add_variable("oe_verif.orb_elems.node_angle" )
  dr_group.add_variable("oe_verif.orb_elems.sinI_sinL" )
  dr_group.add_variable("oe_verif.orb_elems.sinI_cosL" )
  dr_group.add_variable("oe_verif.orb_elems.e_sinE" )
  dr_group.add_variable("oe_verif.orb_elems.e_cosE" )
  dr_group.add_variable("oe_verif.orb_elems.use_pfix_z_values" )

  dr_group.add_variable("oe_verif.orb_elems.j2" )
  dr_group.add_variable("oe_verif.orb_elems.j2_r_eq_2" )
  dr_group.add_variable("oe_verif.orb_elems.j2_correction" )
  dr_group.add_variable("oe_verif.orb_elems.j2_delta" )

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return
