# Purpose: Enable only group2 data record group

# Disable all to show off enabling only group
exec(open("RUN_disable_all/input.py").read())

trick_logging_verif.dr_groups.enable("group2_verif")
