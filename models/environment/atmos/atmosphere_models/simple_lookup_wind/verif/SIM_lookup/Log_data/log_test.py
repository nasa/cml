drg = trick.DRAscii("test_data")
drg.set_cycle(1.0)
drg.freq = trick.DR_Always

drg.add_variable("test.altitude")
drg.add_variable("test.wind.wind_blowing_from")
drg.add_variable("test.wind.wind_vertical_up")
drg.add_variable("test.wind.wind_magnitude_horizontal")
drg.add_variable("test.wind.wind_magnitude")
for ii in range(3):
  drg.add_variable("test.wind.wind_component[" + str(ii) + "]")

trick.add_data_record_group(drg, trick.DR_Buffer)
