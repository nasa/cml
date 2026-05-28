exec(open("RUN_case_00/input.py").read())
test_sep.initialization_option  = 4

# code coverage for 'getters'
trick.add_read(0.6, '''
print('source_frame  =',test_sep.sep_state.get_source().get_name())
print('subject_frame =',test_sep.sep_state.get_subject().get_name())
''')
