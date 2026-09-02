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
print(f"mach {aero.atmos_rel.mach_number} {aero.interface.environment.get_mach()}")
print(f"dyn_pressure {aero.atmos_rel.dynamic_pressure} {aero.interface.environment.get_dynamic_pressure()}")
print(f"total_angle_of_attack {aero.atmos_rel.total_angle_of_attack} {aero.interface.environment.get_total_angle_of_attack()}")
print(f"angle_of_attack {aero.atmos_rel.angle_of_attack} {aero.interface.environment.get_angle_of_attack()}")
print(f"angle_of_sideslip {aero.atmos_rel.angle_of_sideslip} {aero.interface.environment.get_angle_of_sideslip()}")
print(f"phi_roll {aero.atmos_rel.phi_roll} {aero.interface.environment.get_phi_roll()}")
print(f"free_stream_vel_mag {aero.atmos_rel.free_stream_vel_mag} {aero.interface.environment.get_free_stream_vel_mag()}")
print(f"cg_position {aero.cg[0]} {aero.interface.environment.get_cg_position()}")
print(f"true_body_rates {aero.body_rates[0]} {aero.interface.environment.get_true_body_rates()}")

trick.stop(0)
