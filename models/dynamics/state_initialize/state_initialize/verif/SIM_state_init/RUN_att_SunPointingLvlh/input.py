# This is a verification of using the StateInitialize model in conjunction with
# the JEOD DynBodyInitLvlhRotState for purposes of initializing a vehicle's
# attitude in solar-LVLH.
exec(open("Modified_data/common_input.py").read())
# switch default frame to sun.inertial, this will put the body's pos and vel
# into sun-inertial, allowing 
vehicle.body.integ_frame_name = "Sun.inertial"

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.Off


vehicle.rot_init_slvlh.set_subject_body( vehicle.body )
vehicle.rot_init_slvlh.body_frame_id = "composite_body"
vehicle.rot_init_slvlh.action_name = "cev_cm_rot_slvlh_init"
vehicle.rot_init_slvlh.planet_name = "Sun"
vehicle.rot_init_slvlh.orientation.data_source = trick.Orientation.InputQuaternion

dynamics.dyn_manager.add_body_action( vehicle.rot_init_slvlh)



# Verification:
# pos          = [-26263253427,   132801524308,   57571289867]
# vel          = [      -29808,           3016,         -2161]
# T_inrtl_body = [[-0.9780574769721689, -0.1194170698753911, -0.1707136056710512],
#                 [ 0.1073778742058963,  0.4132471128568098, -0.9042658988630137],
#                 [ 0.1785316886993957, -0.9027548876289109, -0.3913566774596479]]

# pos_hat = [ -0.178531688697324    0.902754887629862    0.3913566774584]
# -pos_hat = T[2][0:2] CONFIRMED

# ang_mom_hat = [ -0.107377874205896    -0.41324711285681    0.904265898863014]
# -ang_mom_hat = T[1][0:2]   CONFIRMED

# -ang_mom_hat x -pos_hat = [ -0.978057476972169    -0.119417069875391    -0.170713605671051]
# -ang_mom_hat x -pos_hat = T[0][0:2} CONFIRMED
