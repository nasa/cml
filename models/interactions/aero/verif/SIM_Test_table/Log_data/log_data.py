drg1 = trick.DRAscii("comprehensive_data")
drg1.set_cycle(1.0)
drg1.freq = trick.DR_Always

drg1.add_variable("aero.indep_generic")
drg1.add_variable("aero.atmos_rel.mach_number")
drg1.add_variable("aero.atmos_rel.angle_of_sideslip")
drg1.add_variable("aero.atmos_rel.angle_of_attack")

for cf in ["CX", "CY", "CZ", "CA", "CN", "CD", "CS", "CL",
           "CN_sym", "CL_sym", "Cm_sym", "Cl_mrc", "Cm_mrc", "Cn_mrc"]:
  drg1.add_variable("aero.interface.executive.coefficients." + cf)

for ii in range(3):
  drg1.add_variable("aero.interface.output.force[%d]" %ii)
for ii in range(3):
  drg1.add_variable("aero.interface.output.torque[%d]" %ii)

trick.add_data_record_group(drg1, trick.DR_Buffer)


drg2 = trick.DRAscii("specific_data")
drg2.set_cycle(1.0)
drg2.freq = trick.DR_Always
trick.add_data_record_group(drg2, trick.DR_Buffer)


# optional logging group for aero-damping effects.
def log_damping_effects():
  drg3 = trick.DRAscii("damping_effects")
  drg3.set_cycle(1.0)
  drg3.freq = trick.DR_Always
  trick.add_data_record_group(drg3, trick.DR_Buffer)

  drg3.add_variable("aero.indep_generic")
  for id1 in ["l", "m", "n"]:
    for id2 in ["p", "q", "r"]:
      drg3.add_variable("aero.interface.executive.coefficients.dC%s_d%s" %(id1,id2))
  for id1 in ["l", "m", "n"]:
    drg3.add_variable("aero.interface.executive.coefficients.C%s_cg" %id1)
  for id1 in ["l", "m", "n"]:
    drg3.add_variable("aero.interface.executive.coefficients.C%s_mrc" %id1)
  for ii in range(3):
    drg3.add_variable("aero.interface.output.torque[%d]" %ii)
