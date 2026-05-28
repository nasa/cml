exec(open("Modified_data/common_input.py").read())

# Insert specific initial conditions in the Unit_test files:
verif.framework.enabled = True
verif.framework.data_file_name = "Unit_test_data/data.txt"
verif.framework.vars_file_name = "Unit_test_data/a_rotation_variables.txt"

verif.corr_state.pv_covar_frame = trick.CorrelatedStateDispersion.PV_COVAR_LVLH
verif.corr_state.corr_option = trick.CorrelatedStateDispersion.CORRELATED_PV_ATT
verif.disperse_pva_diff_ref_diff_att = True

trick.stop(3)
log_att()