# Purpose: Enable data record group1, set with a large logging period, adjust
# logging period mid-step. Logging frequency changes are not applied until
# the next scheduled logging operation, which will be at t=4.0s.
exec(open("RUN_common/input.py").read())

trick_logging_verif.dr_groups.disable("group2_verif")
trick_logging_verif.dr_groups.set_cycle(2.0)

trick.add_read(3.0,"trick_logging_verif.dr_groups.set_cycle(0.1, 'group1_verif')")
