def log_data():
    dr_group = trick.DRAscii("iir_filter_test")
    dr_group.set_cycle(1.0)
    dr_group.freq = trick.DR_Always

    dr_group.add_variable("test.current_index")
    dr_group.add_variable("test.current_x")
    dr_group.add_variable("test.current_y_in")
    dr_group.add_variable("test.current_y_out")
    dr_group.add_variable("test.done")

    trick.add_data_record_group(dr_group, trick.DR_Buffer)