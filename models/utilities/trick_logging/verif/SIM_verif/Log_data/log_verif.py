##################################TRICK HEADER######################################
#PURPOSE: 
#   (To define the elements to be logged in the logging of trick_logging_verif)
####################################################################################

def log_group1_verif(log_cycle):
    recording_group_name = "group1_verif"
    dr_group = trick.DRAscii(recording_group_name)
    dr_group.thisown = 0
    dr_group.set_cycle(log_cycle)
    dr_group.freq = trick.DR_Always

    dr_group.add_variable("trick_logging_verif.group1_var")

    trick.add_data_record_group(dr_group, trick.DR_Buffer)
    trick_logging_verif.dr_groups.add_to_list(dr_group)

    return

def log_group2_verif(log_cycle):
    recording_group_name = "group2_verif"
    dr_group = trick.DRAscii(recording_group_name)
    dr_group.thisown = 0
    dr_group.set_cycle(log_cycle)
    dr_group.freq = trick.DR_Always

    dr_group.add_variable("trick_logging_verif.group2_var")

    trick.add_data_record_group(dr_group, trick.DR_Buffer)
    trick_logging_verif.dr_groups.add_to_list(dr_group)

    return
