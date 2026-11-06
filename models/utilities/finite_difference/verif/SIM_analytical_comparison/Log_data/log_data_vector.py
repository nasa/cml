# Initialize ASCII (.csv) data reocrder
dr_group = trick.DRAscii("vector_data")
dr_group.set_cycle(0.1)
dr_group.freq = trick.DR_Always

#Variables to track

for ii in range(3):
    #First derivative
    dr_group.add_variable(f"fd_obj.an_deriv_vec[{ii}]", f"an_deriv[{ii}]")
    dr_group.add_variable(f"fd_obj.num_deriv_vec[{ii}]", f"Num_deriv[{ii}]")
    #Second derivative
    dr_group.add_variable(f"fd_obj.an_deriv2_vec[{ii}]", f"an_deriv2[{ii}]")
    dr_group.add_variable(f"fd_obj.num_deriv2_vec[{ii}]", f"Num_deriv2[{ii}]")  


#Enable logging in the trick sim
trick.add_data_record_group(dr_group, trick.DR_Buffer)


