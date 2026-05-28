#For expansion of code coverage to hit code at line 288, and 310.
#For correlated state dispersion model.

exec(open("Modified_data/common_input.py").read())
verif.disperse_pv = True
log_prm()

# Use target relative dispersions with 3-sigma values applied from defined covariance
verif.corr_state.pv_covar_frame = trick.CorrelatedStateDispersion.PV_COVAR_TR_PARAM
verif.corr_state.dispersion_distribution = trick.CorrelatedStateDispersion.Constant
verif.corr_state.sigma_limit = 3.0

# Target position is on equator at 90 deg longitude
verif.corr_state.TR_param.target_point_altitude = 0.0
verif.corr_state.TR_param.target_point_latitude = 0.0
verif.corr_state.TR_param.target_point_longitude = trick.attach_units("degree", 90.0)

# Zero out covariance matrix
for i in range(0,6):
   for j in range(0,6):
      verif.corr_state.covariance[i][j] = 0.0

# All dispersed states will have same geodetic altitude
verif.corr_state.TR_geodetic_altitude_disp = -299943.0

# Define non-correlated dispersions
verif.corr_state.covariance[0][0] = 0.030461741978671       #Range Angle:        30 deg 3-sigma
verif.corr_state.covariance[1][1] = 3.046174197867086e-04   #Cross Angle:         3 deg 3-sigma
verif.corr_state.covariance[2][2] = 1.0                     #Inertial Speed:      3 m/s 3-sigma
verif.corr_state.covariance[3][3] = 0.030461741978671       #Flight Path Angle:  30 deg 3-sigma
verif.corr_state.covariance[4][4] = 0.030461741978671       #Lateral Angle:      30 deg 3-sigma

verif.planet.pfix.state.rot.ang_vel_this = [0,0,1]
verif.planet.r_eq =  6378137
verif.planet.r_pol = 6356752

# FIXME Turner 2020/11
#       The TR-params code is rather fragile, and if the errors get added a second time, it will fail.
#       Need to figure out why.

# Nominal parameter values:
#   Range Angle       = 90 deg
#   Cross Angle       = 0 deg
#   Inertial Speed    = 8000 m/s
#   Flight Path Angle = 0 deg
#   Lateral Angle     = 0 deg
# Dispersed parameter values after application of 3-sigma values from covariance:
#   Range Angle       = (  90 + 30) deg
#   Cross Angle       = (   0 +  3) deg
#   Inertial Speed    = (8000 + 3)  m/s
#   Flight Path Angle = (   0 + 30) deg
#   Lateral Angle     = (   0 + 30) deg
#
# From an off-line Matlab script, the position and velocity output should be:
#   position = [+5.618875061513194e+06, -3.249999069279792e+06, -3.401836181588708e+05]
#   velocity = [+6.525203314205090e+03, +2.900066602873446e+03, +3.613771493493324e+03]
#
