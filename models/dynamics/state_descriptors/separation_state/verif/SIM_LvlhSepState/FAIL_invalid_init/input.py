exec(open("RUN_verif_name/input.py").read())

print("\n"
"**************************************************************************\n"
" Fatal Error, using the initialization routine intended for an instance\n"
" using an external LVLH frame on an instance using an internal LVLH frame\n"
"**************************************************************************\n")
test_sep.sep_state_1.initialize( dynamics.dyn_manager,
                                 test_sep.subject.composite_body)
trick.stop(0)
