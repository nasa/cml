dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(0.1)
dr_group.freq = trick.sim_services.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable( "test.sim_speed[0].sim_speed")
dr_group.add_variable( "test.cycle_time[0]")
dr_group.add_variable( "test.sim_speed[1].sim_speed")
dr_group.add_variable( "test.cycle_time[1]")
dr_group.add_variable( "test.sim_speed[2].sim_speed")
dr_group.add_variable( "test.cycle_time[2]")
dr_group.add_variable( "test.sim_speed[3].sim_speed")
dr_group.add_variable( "test.cycle_time[3]")

test.sim_speed[0].subscribe()
test.sim_speed[1].subscribe()
test.sim_speed[2].subscribe()
test.sim_speed[3].subscribe()

trick.stop(3)
