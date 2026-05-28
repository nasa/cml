dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("testing_model.independent_vars[0]")
dr_group.add_variable("testing_model.test_so_defined.indep0_ix")
dr_group.add_variable("testing_model.test_so_defined.indep0.fraction")
dr_group.add_variable("testing_model.test_so_defined.indep0_off_table")
dr_group.add_variable("testing_model.test_so_defined.indep0.prox_override")
dr_group.add_variable("testing_model.test_so_defined.indep0.index_prox")

for i in range(1, 4) : # [1, 2, 3]
   dr_group.add_variable("testing_model.independent_vars[%d]" %i)
   dr_group.add_variable("testing_model.test_so_defined.indep%d_ix" %i)
   dr_group.add_variable("testing_model.test_so_defined.indep%d.fraction" %i)

for i in range(3) :
   dr_group.add_variable("testing_model.test_so_defined.dependent_vars[%d]" %i)
for i in range(3) :
   dr_group.add_variable("testing_model.test_on_the_fly.dependent_vars[%d]" %i)
