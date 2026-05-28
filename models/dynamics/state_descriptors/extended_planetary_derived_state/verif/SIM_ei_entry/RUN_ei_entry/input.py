set_units = trick.sim_services.attach_units

dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

#exec(open("Log_data/prs_verif_rec.py").read())
exec(open("Log_data/log_planet_rel.py").read())

prs_verif.framework.data_file_name = "Unit_test/ei_entry.txt"
prs_verif.framework.vars_file_name = "Unit_test/ei_entry_variables.txt"

prs_verif.earth.name = "Earth"
prs_verif.earth.r_eq = 6378137.0
prs_verif.earth.e_ellip_sq = 0.006694379990141
prs_verif.gravity_source.name = "Earth"

prs_verif.body.set_name("Vehicle")
prs_verif.body.integ_frame_name = "Earth.inertial"

prs_verif.earth_rel.reference_name = "Earth"

# Set constant rotation rate of Earth, and generate related values:
prs_verif.earth.pfix.state.rot.ang_vel_this = [0.0, 0.0, 0.000072921151467]
prs_verif.earth.pfix.state.rot.compute_quaternion()
prs_verif.earth.pfix.state.rot.compute_ang_vel_products()

# Set the body frame to be 180-degree pitch relative to structure
prs_verif.body.mass.composite_properties.T_parent_this[0] = [-1,    0,    0]
prs_verif.body.mass.composite_properties.T_parent_this[1] = [ 0,    1,    0]
prs_verif.body.mass.composite_properties.T_parent_this[2] = [ 0,    0,   -1]

#set the hang-angle vectors:
prs_verif.earth_rel.hang_angle_body_vec             = [-1.0,  0.0, 0.0]
prs_verif.earth_rel.roll_wrt_hdg_body_vec           = [ 0.0,  0.0, 1.0]


# Set the landing site
# {lat, lon, az} = {32.75, -120.75, -89.45172108} degrees; conversion from
# degrees to radians differs between Trick15 and Trick17 so just using radians.
# Note - very excessive use of sig figs, this is to match original data
prs_verif.earth_rel.landing_range.reference_data.latitude  = 0.571595330028143
prs_verif.earth_rel.landing_range.reference_data.longitude = -2.1074850717831533
prs_verif.earth_rel.landing_range.reference_data.azimuth   = -1.5612270544216182
prs_verif.earth_rel.landing_range.reference_data.altitude = 0.0
prs_verif.earth_rel.landing_range.reference_data.position_type  = trick.PfixReferencePoint.topodetic
prs_verif.earth_rel.landing_range.reference_data.direction_type = trick.PfixReferencePoint.topodetic

# Set the Boost-reference frame, not sure this is used:
prs_verif.earth_rel.br_ref_longitude = -0.838712736058119
prs_verif.earth_rel.br_ref_geodetic_latitude = -0.2260724980108255

# Set the plumbline frame, not sure this is used:
prs_verif.earth_rel.pl_ref_longitude = -0.838712736058119
prs_verif.earth_rel.pl_ref_geodetic_latitude = -0.2260724980108255
prs_verif.earth_rel.pl_ref_azimuth = 1.570796326794897

# Configure the parameters of the motion model
prs_verif.earth_rel.subscribe_rel_vel()
prs_verif.earth_rel.subscribe_topocentric()
prs_verif.earth_rel.subscribe_topodetic()
prs_verif.earth_rel.subscribe_hang_roll()

# To get the ranges, need to set the range-origin reference data
# See, for examples, SIM_planet_rel where this functionality is tested.
#TODO Turner 6/16 I do not have the settings for the entry range values
prs_verif.earth_rel.subscribe_landing_range()
prs_verif.earth_rel.subscribe_entry_range()

prs_verif.earth_rel.subscribe()

trick.sim_services.exec_set_terminate_time(1036.0)
