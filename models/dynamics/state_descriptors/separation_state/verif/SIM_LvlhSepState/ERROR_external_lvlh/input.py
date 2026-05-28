exec(open("RUN_verif_name/input.py").read())

print("\n"
"**************************************************************************\n"
" Error: 3 Attempts to initialize the LvlhSepState using the external Lvlh\n"
"        frame. They all fail because the external Lvlh frame has not been\n"
"        configured for use.\n"
" Attempt 1: Uses the correct initialize method, but the LVLH is\n"
"            insufficiently configured. 1 Error message\n"
" Attempt 2: Uses one of the initialize methods for configuring the\n"
"            internal LVLH frame; passes on to the correct initialize\n"
"            method where initialization fails for the same reason as in\n"
"            Attempt 1. 2 error messages.\n"
" Attempt 3: Uses the other of the initialize methods for configuring the\n"
"            internal LVLH frame; passes on to the correct initialize\n"
"            method where initialization fails for the same reason as in\n"
"            Attempt 1. 2 error messages.\n"
"**************************************************************************\n")
test_sep.check_init_errors = True
#sep_state_3.initialize( dynamics.dyn_manager,
#                                 test_sep.subject.composite_body)
trick.stop(0)
