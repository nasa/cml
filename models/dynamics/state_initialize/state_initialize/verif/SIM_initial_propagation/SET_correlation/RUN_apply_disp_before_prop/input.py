exec(open("SET_forward_8x8_Earth_only/RUN_verif_6000/input.py").read())
test.state_init.correlation.dispersion_distribution = trick.CorrelatedStateDispersion.Constant
test.state_init.correlation.corr_option = trick.CorrelatedStateDispersion.CORRELATED_PV_ATT
test.state_init.correlation.sigma_limit = 2.0
test.state_init.correlation.covariance[0][0] = 100
test.state_init.correlation.covariance[1][1] = 1
test.state_init.correlation.covariance[2][2] = 0.01

test.state_init.correlation.covariance[3][3] = 100
test.state_init.correlation.covariance[4][4] = 1
test.state_init.correlation.covariance[5][5] = 0.01

test.state_init.correlation.covariance[6][6] = 100
test.state_init.correlation.covariance[7][7] = 1
test.state_init.correlation.covariance[8][8] = 0.01

test.state_init.correlation.covariance[0][1] = 9
test.state_init.correlation.covariance[1][0] = 9
