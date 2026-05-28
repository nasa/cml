#Uses the SYM_ANm table which uses indep_generic as the only independent variable and populates
# CA, CN_sym, and Cm_sym
exec(open( "input_common.py").read())
exec(open( "Log_data/log_data.py").read())
drg2.add_variable("aero.interface.executive.coefficients.CA")
drg2.add_variable("aero.interface.executive.coefficients.CN_sym")
drg2.add_variable("aero.interface.executive.coefficients.Cm_sym")

aero.interface.executive.change_table(aero.table_SYM_ANm)
