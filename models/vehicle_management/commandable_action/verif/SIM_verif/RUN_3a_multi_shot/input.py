exec(open("RUN_1b_reset_sequence/input.py").read())
test.framework.data_file_name = "Unit_test_data/data3.txt"

test.direct_set.act1.multi_shot = True
test.indirect_set.act1.multi_shot = True
test.direct_set.act4.multi_shot = True
test.indirect_set.act4.multi_shot = True
