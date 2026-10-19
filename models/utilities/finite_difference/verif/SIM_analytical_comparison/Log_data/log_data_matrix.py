# Initialize ASCII (.csv) data reocrder
dr_group = trick.DRAscii("matrix_data")
dr_group.set_cycle(0.1)
dr_group.freq = trick.DR_Always

#Variables to track

#First derivative
for ii in range(3):
    for jj in range(3):
        dr_group.add_variable(f"fd_obj.an_deriv_mat[{ii}][{jj}]", f"an_deriv[{ii}][{jj}]")
        dr_group.add_variable(f"fd_obj.num_deriv_mat[{ii}][{jj}]", f"Num_deriv[{ii}][{jj}]")        

#Enable logging in the trick sim
trick.add_data_record_group(dr_group, trick.DR_Buffer)


