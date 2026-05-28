dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("convert_double_words.convert_value" )
dr_group.add_variable("convert_double_words.resolution" )
dr_group.add_variable("convert_double_words.bit_size" )
for ii in range(4) :
  dr_group.add_variable("convert_double_words.warray[%d]" %ii)
