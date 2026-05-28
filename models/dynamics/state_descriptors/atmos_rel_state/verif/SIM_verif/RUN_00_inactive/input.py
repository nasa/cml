exec(open( "Log_data/log_data.py").read())

atmos_rel_verif.framework.data_file_name = "Unit_test_data/data.txt"
atmos_rel_verif.framework.vars_file_name = "Unit_test_data/variables.txt"

atmos_rel_verif.Lref_Re = 5.0292

trick.stop(11)
