# Tests the correlated dispersion of sensor error state (3x3 covariance)

exec(open("Modified_data/common_input.py").read())

# Set correlation option to CORRELATED_SENSOR_ERROR
verif.corr_state.corr_option = trick.CorrelatedStateDispersion.CORRELATED_SENSOR_ERROR

# Enable sensor error dispersion
verif.disperse_sensor_error = True

# Initialize true sensor error state to zeros
verif.true_sensor_error_state = [0.0, 0.0, 0.0]

# Set up a 3x3 covariance matrix for sensor errors
# Using different values for each axis to verify correct dispersion
verif.corr_state.covariance[0][0] = 100   # X-axis variance
verif.corr_state.covariance[1][1] = 1     # Y-axis variance
verif.corr_state.covariance[2][2] = 0.01  # Z-axis variance

# Log sensor error data
log_sensor_error()
