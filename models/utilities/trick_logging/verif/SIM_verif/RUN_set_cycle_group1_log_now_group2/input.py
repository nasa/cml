# Purpose:
# - Change the log cycle of a group
#     change group1 to 0.5s
# - Insert an unscheduled logging event for an active group
#     insert group1 logging at t = 3.75 s
# - Insert a one-of-a-kind logging event for a disabled group
#     insert group2 logging at t = 5.5 s
# - Verify behavior when log_now is called twice at the same timestamp
#     insert another group2 logging at t = 5.5 s
exec(open("RUN_common/input.py").read())

trick_logging_verif.dr_groups.set_cycle(0.5,"group1_verif")
trick.add_read(3.75,"""
trick_logging_verif.dr_groups.log_now("group1_verif")
""")

trick_logging_verif.dr_groups.disable("group2_verif")
trick.add_read(5.5,"""
trick_logging_verif.dr_groups.log_now("group2_verif")
trick_logging_verif.dr_groups.log_now("group2_verif")
""")
