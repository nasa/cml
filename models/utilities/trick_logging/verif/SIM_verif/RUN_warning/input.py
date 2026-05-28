# Purpose: Elicit a warning with a NULL input to one of the functions
exec(open("RUN_common/input.py").read())

trick_logging_verif.dr_groups.add_to_list(None)
'''
FIXME Turner 2026/02
  These methods were intended to test the receipt of null char pointers
  However, SWIG is getting confused and trying to pass null into
  the std::string reference signature.
  Commenting these options out, leaving this particular code untested.

trick_logging_verif.dr_groups.set_cycle(1.0,None)
trick_logging_verif.dr_groups.log_now(None)
trick_logging_verif.dr_groups.disable(None)
trick_logging_verif.dr_groups.enable(None)
'''
