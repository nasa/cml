exec(open("Log_data/log_data.py").read())
test.framework.vars_file_name = "Unit_test_data/vars.txt"
test.framework.data_file_name = "Unit_test_data/data.txt"
test.constraints.set.subscribe()
trick.stop(6.3)

test.constraints.interval_x2t.violation_condition = trick.ConstraintEnum.Out
test.constraints.interval_x2t_neg.violation_condition = trick.ConstraintEnum.Out
test.constraints.interval_i2t.violation_condition = trick.ConstraintEnum.Out
test.constraints.interval_x2i.violation_condition = trick.ConstraintEnum.Out


# Note: Deliberately leaving interval_x2t with default
#       violate_on_any_test=True to test warning message. Result
#       will be that the constraint is permanently violated becausee
#       variable will always be outisde either [10,20] or [30,40].
test.constraints.interval_x2t_neg.violate_on_any_test = False
test.constraints.interval_i2t.violate_on_any_test = False
test.constraints.interval_x2i.violate_on_any_test = False

log_constraint_t("interval_x2t", 2)
log_constraint_t("interval_x2t_neg", 2)
log_constraint_t("interval_i2t", 2)
log_constraint  ("interval_x2i", 2)
