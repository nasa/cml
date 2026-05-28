exec(open("Log_data/log_data.py").read())
log_range_model("range_to_pfix")
log_range_model("range_from_pfix")

exec(open("env_setup.py").read())

test.range_to_pfix.subscribe();
test.range_to_pfix.reference_data.position_type = trick.PfixReferencePoint.inertial
test.range_to_pfix.reference_data.direction_type = trick.PfixReferencePoint.inertial

test.range_from_pfix.subscribe();
test.range_from_pfix.reference_data.position_type = trick.PfixReferencePoint.inertial
test.range_from_pfix.reference_data.direction_type = trick.PfixReferencePoint.inertial
