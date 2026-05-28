# Covers q_delta_s < 0.0 check in QuaternionSphericalInterpolator::update()
#    Defines two quaternions so that q_delta_s is negative
code_coverage.qsi.quat_0.scalar = 0.5;
code_coverage.qsi.quat_0.vector[0] = 0.5;
code_coverage.qsi.quat_0.vector[1] = 0.5;
code_coverage.qsi.quat_0.vector[2] = 0.5;
code_coverage.qsi.quat_1.scalar = 0.5;
code_coverage.qsi.quat_1.vector[0] = -0.5;
code_coverage.qsi.quat_1.vector[1] = -0.5;
code_coverage.qsi.quat_1.vector[2] = -0.5;
code_coverage.qsi.frac = 0.5
code_coverage.q_delta_s()

trick.stop(0)