#Uses the XYZ table which uses indep_generic as the only independent variable and populates
# CX, CY, CZ, Cl_mrc, Cm_mrc, Cn_mrc
exec(open( "input_common.py").read())
exec(open( "Log_data/log_data.py").read())
drg2.add_variable("aero.interface.executive.coefficients.CX")
drg2.add_variable("aero.interface.executive.coefficients.CY")
drg2.add_variable("aero.interface.executive.coefficients.CZ")
drg2.add_variable("aero.interface.executive.coefficients.Cl_mrc")
drg2.add_variable("aero.interface.executive.coefficients.Cm_mrc")
drg2.add_variable("aero.interface.executive.coefficients.Cn_mrc")

aero.interface.executive.change_table(aero.table_XYZ)
