# Purpose: Fire off logging for all data record groups at simulation termination time

exec(open("RUN_common/input.py").read())

trick_logging_verif.dr_groups.disable("group2_verif")

trick.add_read(5.5,"""
trick_logging_verif.dr_groups.log_now()
""")
