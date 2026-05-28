# Purpose: Enable all data record groups and change the log cycle for all groups
# halfway through

# Exec disable all RUN to demonstrate enable capabilities
exec(open("RUN_disable_all/input.py").read())

trick_logging_verif.dr_groups.enable_all()

trick.add_read(3.0,"""
trick_logging_verif.dr_groups.set_cycle(0.5)
""")
