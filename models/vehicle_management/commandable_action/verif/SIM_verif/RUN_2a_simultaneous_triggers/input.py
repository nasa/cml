exec(open("RUN_1a_simple_sequence/input.py").read())
test.framework.data_file_name = "Unit_test_data/data2.txt"

print ('\n'
'************************************************************************\n'
'ERROR messages (x3):\n'
' act2 cannot process because of act1\n'
' act4 cannot process because of act2\n'
' act3 cannot process because of act4\n'
'************************************************************************')

