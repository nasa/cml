################TRICK HEADER#######################################
#PURPOSE:
#  (To define the elements to be logged in the logging of std76_unit_rec) 
####################################################################################

def log_std76_unit_rec ( log_cycle ) :
  recording_group_name =  "std76_unit"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always
  dr_group.add_variable(   "std76_unit.altitude")
  dr_group.add_variable(   "std76_unit.std1976.kinetic_temperature")
  dr_group.add_variable(   "std76_unit.std1976.molecular_temperature")
  dr_group.add_variable(   "std76_unit.std1976.pressure")
  dr_group.add_variable(   "std76_unit.std1976.density")
  dr_group.add_variable(   "std76_unit.std1976.speed_of_sound")
  dr_group.add_variable(   "std76_unit.std1976.dynamic_viscosity")
  dr_group.add_variable(   "std76_unit.std1976.molecular_weight")
  dr_group.add_variable(   "std76_unit.std1976.layer_number")
  dr_group.add_variable(   "std76_unit.std1976.mean_free_path")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  return
