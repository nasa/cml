exec(open("Log_data/log_data.py").read())
test.framework.vars_file_name = "Unit_test_data/vars.txt"
test.framework.data_file_name = "Unit_test_data/data.txt"
test.constraints.set.subscribe()
trick.stop(6.3)

test.constraints.threshold_x3s.use_linear_interpolation = False
test.constraints.threshold_x5s.use_linear_interpolation = False

log_constraint_t("threshold_x3s", 3)
log_constraint_t("threshold_x5s", 5)
