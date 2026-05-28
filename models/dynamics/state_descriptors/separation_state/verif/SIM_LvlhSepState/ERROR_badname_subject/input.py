exec(open("RUN_verif_name/input.py").read())

test_sep.subject_init.name = "bad-point"
print("\n"
"****************************************************************************\n"
" Error: Attempt to initialize the LvlhSepState using an invalid subject name\n"
"****************************************************************************\n")
trick.stop(0)
