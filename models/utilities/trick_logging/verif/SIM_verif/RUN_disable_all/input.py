# Purpose: disable all the data record groups
exec(open("RUN_common/input.py").read())

trick_logging_verif.dr_groups.disable_all()
