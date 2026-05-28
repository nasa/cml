#Uses the AYN_unc table which uses indep_generic as the only independent variable and populates
# CA, CY, CN, Cl_mrc, Cm_mrc, Cn_mrc and the corresponding uncertainty values
exec(open( "input_common.py").read())
exec(open( "Log_data/log_data.py").read())
drg2.add_variable("aero.interface.executive.coefficients.CA")
drg2.add_variable("aero.interface.executive.coefficients.CY")
drg2.add_variable("aero.interface.executive.coefficients.CN")
drg2.add_variable("aero.interface.executive.coefficients.Cl_mrc")
drg2.add_variable("aero.interface.executive.coefficients.Cm_mrc")
drg2.add_variable("aero.interface.executive.coefficients.Cn_mrc")
drg2.add_variable("aero.interface.executive.uncertainty.CA")
drg2.add_variable("aero.interface.executive.uncertainty.CY")
drg2.add_variable("aero.interface.executive.uncertainty.CN")
drg2.add_variable("aero.interface.executive.uncertainty.Cl")
drg2.add_variable("aero.interface.executive.uncertainty.Cm")
drg2.add_variable("aero.interface.executive.uncertainty.Cn")

aero.interface.executive.change_table(aero.table_AYN_unc)
