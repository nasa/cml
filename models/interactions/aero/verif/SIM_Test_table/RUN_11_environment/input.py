exec(open("RUN_02_example_table/input.py").read())

# Set some nonsense values for the atmosphere-state
aero.atmos_rel.mach_number =  1.12
aero.atmos_rel.dynamic_pressure =  2.12
aero.atmos_rel.total_angle_of_attack =  3.12
aero.atmos_rel.angle_of_attack =  4.12
aero.atmos_rel.angle_of_sideslip =  5.12
aero.atmos_rel.phi_roll =  6.12
aero.atmos_rel.free_stream_vel_mag =  7.12
aero.cg[0] =  8.12
aero.body_rates[0] =  9.12


print("\n\n**** Printing the values just assigned and the return from the")
print("**** getter methods to verify connectivity.")
print("mach {0} {1}".format(aero.atmos_rel.mach_number,aero.interface.environment.get_mach()))
print("dyn_pressure {0} {1}".format(aero.atmos_rel.dynamic_pressure,aero.interface.environment.get_dynamic_pressure()))
print("total_angle_of_attack {0} {1}".format(aero.atmos_rel.total_angle_of_attack, aero.interface.environment.get_total_angle_of_attack()))
print("angle_of_attack {0} {1}".format(aero.atmos_rel.angle_of_attack, aero.interface.environment.get_angle_of_attack()))
print("angle_of_sideslip {0} {1}".format(aero.atmos_rel.angle_of_sideslip, aero.interface.environment.get_angle_of_sideslip()))
print("phi_roll {0} {1}".format(aero.atmos_rel.phi_roll, aero.interface.environment.get_phi_roll()))
print("free_stream_vel_mag {0} {1}".format(aero.atmos_rel.free_stream_vel_mag, aero.interface.environment.get_free_stream_vel_mag()))
print("cg_position {0} {1}".format(aero.cg[0], aero.interface.environment.get_cg_position()))
print("true_body_rates {0} {1}".format(aero.body_rates[0], aero.interface.environment.get_true_body_rates()))

trick.stop(0)
