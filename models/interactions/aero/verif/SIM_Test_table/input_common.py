aero.interface.subscribe();

# Put some dummy values in for environment factors; the environment is not
# being computed and without these we would get div-0 warnings.
aero.geodetic_altitude = 100;
aero.atmos_exec.out.wind_velocity_eci[0] = 1.0;
aero.atmos_exec.out.speed_of_sound = 1.0;
aero.T_struc_body = [[1,0,0],[0,1,0],[0,0,1]]

# Block the warning about free-stream velocity and position vector
# being parallel -- aero frame does not have to be well defined.
aero.atmos_rel.warn_on_v_parallel_r = False

# Give environment some non-zero values
aero.atmos_exec.out.pressure = 10.0
aero.atmos_exec.out.density  =  1.0

# Test inputs:
aero.framework.data_file_name = "Unit_test_data/data.txt"
aero.framework.vars_file_name = "Unit_test_data/variables.txt"

# Dispersion settings
aero.interface.executive.random.CX = 0.1
aero.interface.executive.random.CY = 0.2
aero.interface.executive.random.CZ = 0.3

aero.interface.executive.random.CA = 0.4
aero.interface.executive.random.CN = 0.5

aero.interface.executive.random.CD = 0.6
aero.interface.executive.random.CS = 0.7
aero.interface.executive.random.CL = 0.8

aero.interface.executive.random.CN_sym = 0.15
aero.interface.executive.random.CL_sym = 0.25
aero.interface.executive.random.Cm_sym = 0.35

aero.interface.executive.random.Cl = 0.45
aero.interface.executive.random.Cm = 0.55
aero.interface.executive.random.Cn = 0.65

aero.interface.executive.random.dCl_dp = -0.01
aero.interface.executive.random.dCm_dq = -0.02
aero.interface.executive.random.dCn_dr = -0.03

aero.interface.executive.random.dCl_dq = -0.04
aero.interface.executive.random.dCl_dr = -0.05
aero.interface.executive.random.dCm_dp = -0.06
aero.interface.executive.random.dCm_dr = -0.07
aero.interface.executive.random.dCn_dp = -0.08
aero.interface.executive.random.dCn_dq = -0.09

trick.stop(6)
