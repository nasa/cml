#Uses the DSL table which uses indep_generic as the only independent variable and populates
# CD, CS, CL, Cl_mrc, Cm_mrc, Cn_mrc.  Note the data for this run should have "nice" values for
# CD, CS, and CL making the other coefficients take on calculated values.
exec(open( "input_common.py").read())
exec(open( "Log_data/log_data.py").read())
drg2.add_variable("aero.interface.executive.coefficients.CD")
drg2.add_variable("aero.interface.executive.coefficients.CS")
drg2.add_variable("aero.interface.executive.coefficients.CL")
drg2.add_variable("aero.interface.executive.coefficients.Cl_mrc")
drg2.add_variable("aero.interface.executive.coefficients.Cm_mrc")
drg2.add_variable("aero.interface.executive.coefficients.Cn_mrc")

aero.interface.executive.change_table(aero.table_DSL)
