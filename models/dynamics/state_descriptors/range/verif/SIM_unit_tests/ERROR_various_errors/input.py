exec(open("Log_data/log_data.py").read())
log_range_model("range_to_pfix")
log_range_model("range_from_pfix")

exec(open("env_setup.py").read())

test.range_from_pfix.subscribe()
test.range_from_pfix.reference_data.position_type = trick.PfixReferencePoint.inertial
test.range_from_pfix.reference_data.direction_type = trick.PfixReferencePoint.inertial
# Call set_reference_data() before initialization, triggering the "Range Computation
# initialization sequence" error in set_reference_data() (at t = 0.0)
test.range_from_pfix.set_reference_data()
# position_type and direction_type are set before initialization, so set_reference_data() will be
# called during initialization. However, direction is still the 0 vector, triggering an error in
# PfixReferencePoint::set_direction() (still at t = 0.0)

# Trigger the "Invalid reference direction" warning in
# RangeFromPfixReference::set_reference_data() (at t = 0.1)
test.trans.position = [10000.0, 0.0, 0.0]
test.trans.velocity = [1.0, 0.0, 0.0]
trick.add_read(0.1, "test.range_from_pfix.set_reference_data_from_inrtl_state(test.trans)")

test.range_to_pfix_internal_ref.subscribe()
test.range_to_pfix_internal_ref.reference_data.position_type = trick.PfixReferencePoint.topocentric
# "Forget" to set direction_type, triggering the error in PfixReferencePoint::set_direction
# (at t = 0.2)
trick.add_read(0.2, "test.range_to_pfix_internal_ref.set_reference_data()")

test.range_to_pfix.subscribe()
# "Forget" to set position_type, triggering the error in PfixReferencePoint::set_point()
# (at t = 0.3)
trick.add_read(0.3, "test.range_to_pfix.set_reference_data()")

# This test was intended to hit the warning at the end of PfixReferencePoint::set_point(), but it
# triggers a failure in PlanetFixedPosition first. Because of the checks in PlanetFixedPosition,
# it is impossible to hit the warning in set_point(). (at t = 0.4)
trick.add_read(0.4, "test.trans.position = [0.0, 0.0, 0.0]")

# NOTE: Removed "trick.add_read(0.4, "test.range_to_pfix.set_reference_data_from_inrtl_state(test.trans)")".
#       This line was intended to add coverage to this model. For the reason described in the comment above,
#       it does not increase coverage. It also causes the sim to hang indefinitely. Post removal, the sim
#       runs to completion and has been added to default.yml to increase coverage in the GCOVR report.
#

trick.stop(1.0)
