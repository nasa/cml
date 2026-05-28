def log_test_data ( log_cycle ) : 
  recording_group_name =  "test_data"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0 
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always
  
  dr_group.add_variable("double_words.convert_value" )
  dr_group.add_variable("double_words.resolution" )
  dr_group.add_variable("double_words.bit_size" )

  for ii in range(0,4) :
       dr_group.add_variable("double_words.words[%d]" %ii)
  
  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  return

