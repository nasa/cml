exec(open("Modified_data/env_setup.py").read())
exec(open("Modified_data/set_points.py").read())
exec(open("Log_data/log_base.py").read())
log_control_flags()
log_pt_to_pt()
vehicle.planet_rel_state.subscribe()
vehicle.planet_rel_state.subscribe_pt_to_pt()

# Offset the CoM from the structure origin and rotate the body frame by 90
# degrees
vehicle.mass_init.properties.position  = [0,1,0]
vehicle.mass_init.properties.pt_orientation.trans = [[0,0,1],[0,1,0],[-1,0,0]]

# Rotate the body frame wrt inertial
vehicle.state_initialize.T_inrtl_body = [[1,0,0],[0,0,1],[0,-1,0]]

#                                    [0 1 0]                         [0 0 1]
# That should make inertial-to-struc [0 0 1]  and struc-to-inertial  [1 0 0]
#                                    [1 0 0]                         [0 1 0]
# Thus:
# - positions of v-pts relative to struc, expressed in inertial are:
#      v_0 at [0, 0, 0];   v_x+1 at [0, 1, 0];   v_y-1 at [0, 0, -1]
# - position of CoM wrt Struc in inrtl is [0 0 1]
# - positions of v-pts relative to CoM, expressed in inertial are:
#      v_0 at [0, 0, -1];   v_x+1 at [0, 1, -1];   v_y-1 at [0, 0, -2]

trick.stop(5)
