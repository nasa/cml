drg = trick.DRAscii("lu_winds")
drg.freq = trick.DR_Always
drg.set_cycle(1)
trick.add_data_record_group(drg, trick.DR_Buffer)

# these two will not equal if there is an altitude bias
drg.add_variable("luwinds.altitude")
drg.add_variable("luwinds.lookup_table_winds.altitude")

drg.add_variable("luwinds.lookup_table_winds.u")
drg.add_variable("luwinds.lookup_table_winds.v")
drg.add_variable("luwinds.lookup_table_winds.w")
drg.add_variable("luwinds.lookup_table_winds.rho")
drg.add_variable("luwinds.lookup_table_winds.P")
drg.add_variable("luwinds.lookup_table_winds.T")
drg.add_variable("luwinds.lookup_table_winds.wind_number")
drg.add_variable("luwinds.lookup_table_winds.SOS")
for ii in range(3):
    drg.add_variable("luwinds.lookup_table_winds.wind_velocity_tc[{0}]".format(ii))
drg.add_variable("luwinds.lookup_table_winds.wind_angle_blowing_from")
drg.add_variable("luwinds.lookup_table_winds.wind_vmag")

