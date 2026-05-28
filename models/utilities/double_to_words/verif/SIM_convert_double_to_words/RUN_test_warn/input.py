exec(open("RUN_test/input.py").read())
# change the data
convert_double_words.framework.data_file_name = "Unit_test_data/data_warn.txt"
# make inform messages visible
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)
trick.add_read(3,"""
print('*******************************************')
print('Error at t=3; number cannot be represented')
print('*******************************************')
""")

trick.add_read(4,"""
print('**********************************************************')
print('Error at t=4; resolution too small, words can be skipped')
print('**********************************************************')
""")

trick.add_read(7,"""
print('**********************************************************')
print('Error at t=7; resolution too small, words can be skipped')
print('**********************************************************')
""")
