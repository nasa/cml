import os


dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_Ephemerides

env.de4xx.set_model_number(421)
env.de4xx.set_model_directory(
    os.path.join(os.getenv("JEOD_HOME"), "lib_jeod_" + os.getenv("TRICK_HOST_CPU"), "de4xx_lib")
)

jeod_time.time_manager_init.initializer = "UTC"
jeod_time.time_manager_init.sim_start_format = trick.TimeEnum.calendar
jeod_time.time_utc.calendar_year = 2017
jeod_time.time_utc.calendar_month = 1
jeod_time.time_utc.calendar_day = 1
jeod_time.time_utc.calendar_hour = 0
jeod_time.time_utc.calendar_minute = 0
jeod_time.time_utc.calendar_second = 0

test.body.integ_frame_name = "Earth.inertial"
test.body.set_name("test_vehicle")
test.body.translational_dynamics = False
test.body.rotational_dynamics = False

# Mass
test.mass_init.set_subject_body(test.body)
test.mass_init.properties.mass = 1.0
test.mass_init.properties.position  = [0,0,0]
test.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
test.mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
test.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
test.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]


dynamics.dyn_manager.add_body_action( test.mass_init)

#State
test.state_initialize.set_subject_body(test.body)
test.state_initialize.body_frame_id = "composite_body"
test.state_initialize.planet_name = "Earth"

test.state_initialize.position_input_data_type = trick.StateInitialize.Inertial
test.state_initialize.velocity_input_data_type = trick.StateInitialize.Inertial
test.state_initialize.attitude_input_data_type = trick.StateInitialize.EulerInertial_YPR
test.state_initialize.att_rate_input_data_type = trick.StateInitialize.BodyInertialRate
test.state_initialize.yaw = 0.0
test.state_initialize.pitch = 0.0
test.state_initialize.roll = 0.0
test.state_initialize.roll_rate_body = 0.0
test.state_initialize.pitch_rate_body = 0.0
test.state_initialize.yaw_rate_body = 0.0
test.state_initialize.position = [2687, -13270, -5753]
test.state_initialize.velocity = [0.0, 0.0, 0.0]
test.state_initialize.active = True
dynamics.dyn_manager.add_body_action( test.state_initialize)


test.ephem_rotating_frame.set_originating_frame( earth.planet.inertial)
test.ephem_rotating_frame.set_target_frame( sun.planet.inertial)
test.ephem_rotating_frame.pointing_frame.set_name("EM-rotating-frame1")
# NOTE - DO NOT need to add the pointing frame the ephem-based-pointing-frame
#        will do that internally

test.non_ephem_rotating_frame.set_originating_frame( earth.planet.inertial)
test.non_ephem_rotating_frame.set_target_frame( sun.planet.inertial)
test.non_ephem_rotating_frame.pointing_frame.set_name("EM-rotating-frame2")
# NOTE - DO need to add the pointing frame the non-ephem-based-pointing-frame
#        has no access to the dyn-manager
dynamics.dyn_manager.add_ref_frame( test.non_ephem_rotating_frame.pointing_frame)

test.ephem_rel_state.subject_frame_name = "test_vehicle.composite_body"
test.ephem_rel_state.target_frame_name = "EM-rotating-frame1"
test.ephem_rel_state.direction_sense = trick.RelativeDerivedState.ComputeSubjectStateinTarget

test.non_ephem_rel_state.subject_frame_name = "test_vehicle.composite_body"
test.non_ephem_rel_state.target_frame_name = "EM-rotating-frame2"
test.non_ephem_rel_state.direction_sense = trick.RelativeDerivedState.ComputeSubjectStateinTarget

