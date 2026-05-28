exec(open("Log_data/log_data.py").read())
test.framework.vars_file_name = "Unit_test_data/vars.txt"
test.framework.data_file_name = "Unit_test_data/data.txt"
test.constraints.set.subscribe()
trick.stop(6.3)


log_constraint_t("threshold_x4t", 4)
log_constraint  ("threshold_x4i", 4)
log_constraint_t("threshold_i4t", 4)
log_constraint  ("threshold_i4i", 4)
log_constraint_t("threshold_x4s", 4)
log_constraint_t("threshold_x3s", 3)
log_constraint_t("threshold_x3s_neg", 3)
log_constraint_t("threshold_x5s", 5)

log_constraint_t("interval_x2t", 2)
log_constraint_t("interval_x2t_neg", 2)
log_constraint_t("interval_i2t", 2)
log_constraint  ("interval_x2i", 2)
