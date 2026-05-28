dr_group = trick.DRAscii("trick_logging")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

dr_group.add_variable("verif_test.sim_data.dyn_time")
dr_group.add_variable("verif_test.sim_data.x1")
dr_group.add_variable("verif_test.sim_data.x2")
dr_group.add_variable("verif_test.sim_data.i1")
dr_group.add_variable("verif_test.sim_data.b1")
dr_group.add_variable("verif_test.sim_data.e1")

trick.add_data_record_group(dr_group, trick.DR_Buffer)
