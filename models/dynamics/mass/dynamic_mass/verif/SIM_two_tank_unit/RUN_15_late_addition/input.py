exec(open("FAIL_late_addition_separate/input.py").read())
# activating the attach mechanism will prevent the failure resulting from adding
# an unattached mass to a group and will instead trigger warnings about adding
# a mass post-initialization.
mass_test.tank_attach[2].active = True
