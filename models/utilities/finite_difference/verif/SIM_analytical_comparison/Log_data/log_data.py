# Initialize ASCII (.csv) data reocrder
dr_group = trick.DRAscii("scalar_data")
dr_group.set_cycle(0.1)
dr_group.freq = trick.DR_Always

#Variables to track
dr_group.add_variable("fd_obj.f", "Function")
dr_group.add_variable("fd_obj.d1", "Anal. Deriv")
dr_group.add_variable("fd_obj.d1_f", "Num. Deriv")
dr_group.add_variable("fd_obj.d2", "Anal. Deriv 2")
dr_group.add_variable("fd_obj.d2_f", "Num. Driv 2")

#Enable logging in the trick sim
trick.add_data_record_group(dr_group, trick.DR_Buffer)


