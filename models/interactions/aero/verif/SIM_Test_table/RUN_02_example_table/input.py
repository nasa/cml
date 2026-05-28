#Uses the ExampleTableSet1 table which uses mach, alpha, beta as the independent variables
# and populates CA, CY, CN, Cl_mrc, Cm_mrc, Cn_mrc as did RUN_01d_AYN
exec(open( "input_common.py").read())
exec(open( "Log_data/log_data.py").read())
drg2.add_variable("aero.interface.executive.coefficients.CA")
drg2.add_variable("aero.interface.executive.coefficients.CY")
drg2.add_variable("aero.interface.executive.coefficients.CN")
drg2.add_variable("aero.interface.executive.coefficients.Cl_mrc")
drg2.add_variable("aero.interface.executive.coefficients.Cm_mrc")
drg2.add_variable("aero.interface.executive.coefficients.Cn_mrc")

aero.interface.executive.change_table(aero.example_table)
trick.stop(8)
