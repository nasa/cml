# RUN_01_Geometric - Tests the pointing frame in a verieuty of orientations
# and configurations.
# Inputs with zero position and velocity vectors, or with aligned position and
# velocity vectors are avoided in this run, and explored instead in RUN_02

exec(open("Modified_data/env_setup.py").read())

frame.framework.data_file_name = "Unit_test_data/geom_data01.txt"
frame.framework.vars_file_name = "Unit_test_data/geom_variables.txt"

exec(open("Log_data/log_setup.py").read())

trick.add_read(0, '''
print("***************************************************************")
print("Expect 3 comments concerning unscheduled jobs above this line")
print("These comments can be ignored.")
print("***************************************************************")
''')

trick.stop(13)
