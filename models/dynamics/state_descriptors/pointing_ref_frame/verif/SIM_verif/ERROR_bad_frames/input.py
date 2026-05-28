# RUN_02_ErrorChecking - These inputs test the cases of relative position and
# velocity vector alignment.
# Also tests the handling of a zero relative position or velocity vectors.

exec(open("RUN_01_Geometric/input.py").read())
frame.framework.data_file_name = "Unit_test_data/geom_data02.txt"
trick.stop(4)

print("***************************************************************")
print("Set originating frame to NULL")
print("***************************************************************")
frame.pointing_ref_frame.set_originating_frame(None)

print("***************************************************************")
print("Set target frame to NULL")
print("***************************************************************")
frame.pointing_ref_frame.set_target_frame(None)


print("\n\n***************************************************************")
print("Unscheduled jobs warnings, ignore.")
print("***************************************************************")

trick.add_read(0,'''
print("")
print("")
print("***************************************************************")
print("t=0.0 Reset originating frame specification")
print("***************************************************************")
frame.pointing_ref_frame.set_originating_frame(None)

print("")
print("***************************************************************")
print("t=0.0 Reset target frame specification")
print("***************************************************************")
frame.pointing_ref_frame.set_target_frame(None)

print("")
print("***************************************************************")
print("t=1.0 Proximity warning (orientation retained, ang rate zeroed)")
print("***************************************************************")
''')

trick.add_read(2.5,'''
print("")
print("***************************************************************")
print("t=3.0 Alignment warning (y-axis retained from t=2.0)")
print("***************************************************************")
''')

trick.add_read(3.5,'''
print("")
print("***************************************************************")
print("t=4.0 Alignment warning (z-axis retained)")
print("***************************************************************")
''')
