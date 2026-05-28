exec(open( "RUN_01c_XYZ/input.py").read())
log_damping_effects()

# increase the wind velocity to get a free-stream velocity of > 1m/s which is
# the minimum threshold for the damping coefficients to have an effect.
aero.atmos_exec.out.wind_velocity_eci[0] = 2.0;

# Add a body rate, otherwise there is nothing to damp.
aero.body_rates[0] = 1.0
aero.body_rates[1] = -1.0
aero.body_rates[2] = 2.0


# turn off dispersions on all non-damping components.  We have already seen
# their effect in RUN_04* and RUN_05*
aero.interface.executive.random.CX = 0.0
aero.interface.executive.random.CY = 0.0
aero.interface.executive.random.CZ = 0.0
aero.interface.executive.random.Cl = 0.0
aero.interface.executive.random.Cm = 0.0
aero.interface.executive.random.Cn = 0.

aero.interface.executive.l_over_v_scale = 2

# set a struc-to-body transformation to generate a non-zero body-to-aero-frame
# transformation
#aero.T_struc_body = [[-1,0,0],[0,1,0],[0,0,-1]]
aero.table_XYZ.T_struc_to_aero_frame = [[-1,0,0],[0,1,0],[0,0,-1]]

# the damping uncertainties have not been set in the definition of
# SimpleAeroTableSet_XYZ (see include/example_tables.hh), so set those now:
aero.table_XYZ.uncertainty.dCl_dp = 1
aero.table_XYZ.uncertainty.dCl_dq = 2
aero.table_XYZ.uncertainty.dCl_dr = 3
aero.table_XYZ.uncertainty.dCm_dp = 4
aero.table_XYZ.uncertainty.dCm_dq = 5
aero.table_XYZ.uncertainty.dCm_dr = 6
aero.table_XYZ.uncertainty.dCn_dp = 7
aero.table_XYZ.uncertainty.dCn_dq = 8
aero.table_XYZ.uncertainty.dCn_dr = 9

# Now reload the XYZ table to reconfigure it with these new settings.
# That requires switching to something else first -- can't switch to itself.
aero.interface.executive.change_table(aero.table_AYN)
aero.interface.executive.change_table(aero.table_XYZ)
