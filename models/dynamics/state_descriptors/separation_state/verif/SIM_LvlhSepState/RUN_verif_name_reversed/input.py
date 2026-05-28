# This test case repeats RUN_verif_name except that it moves the source around
# isntead of the subject.
exec(open("RUN_verif_name/input.py").read())
test_sep.subject_init.position = [0, 0.5, 0]
test_sep.source_init.position =  [0, 0,  -0.7]
test_sep.framework.vars_file_name = "Unit_test_data/variables_source.txt"

