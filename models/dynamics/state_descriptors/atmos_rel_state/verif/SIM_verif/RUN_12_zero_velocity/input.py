exec(open( "RUN_02_complete_calc/input.py").read())

atmos_rel_verif.framework.data_file_name = "Unit_test_data/data_RUN_12.txt"
print("***********************")
print("R parallel to V message")
print("***********************")
trick.add_read(1.0,'''
print("*********************")
print("Zero velocity message")
print("*********************")
''')
trick.stop(1)
