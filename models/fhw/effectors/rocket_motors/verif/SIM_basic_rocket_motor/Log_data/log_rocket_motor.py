dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.sim_services.DR_Always

def log_add_3vec(var):
  for ii in range(3):
    dr_group.add_variable(var + "[%d]" %ii)

dr_group.add_variable("rocket_motor.motor_prop.dynamic_properties.consumable_mass")
dr_group.add_variable("rocket_motor.basic_motor.mass_flow_rate")
log_add_3vec("rocket_motor.basic_motor.position")
log_add_3vec("rocket_motor.basic_motor.thrust")
log_add_3vec("rocket_motor.basic_motor.thrust_unit_struc")
log_add_3vec("rocket_motor.basic_motor.moment")
log_add_3vec("rocket_motor.basic_motor.dispersions.motor_tolerance")
for ii in range(3) :
  for jj in range(3) :
    dr_group.add_variable("rocket_motor.basic_motor.T_struc_to_motor_frame[%d][%d]" %(ii, jj))

trick.add_data_record_group(dr_group, trick.DR_Buffer)
