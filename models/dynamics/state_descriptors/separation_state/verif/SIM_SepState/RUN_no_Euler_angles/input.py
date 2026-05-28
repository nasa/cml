# This run should produce results identical to RUN_case_04 except that the Euler
# angles remain at 0.
exec(open("RUN_case_04/input.py").read())
test_sep.sep_state.generate_euler_angles = False
