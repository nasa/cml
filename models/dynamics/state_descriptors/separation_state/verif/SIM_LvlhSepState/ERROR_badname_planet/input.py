exec(open("RUN_verif_name/input.py").read())

test_sep.planet_name = "bad"
print("\n"
"***************************************************************************\n"
" Error: Attempt to initialize the LvlhSepState using an invalid planet name\n"
"        Both default sep-states will generate the same error\n"
"***************************************************************************\n")

trick.stop(0)
