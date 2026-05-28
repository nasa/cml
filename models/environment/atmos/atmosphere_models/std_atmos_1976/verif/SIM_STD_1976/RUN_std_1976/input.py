#############  TRICK HEADER  ########################################
#
# PURPOSE:
#  (To define the input data for simulation RUN_entry_traj/ 
#
#####################################################################

#Auto-generated from the data file with this header:
#/*
# * Author : Brian Bihari
# * Date   : Oct 2014
# */


trick.sim_services.exec_set_trap_sigfpe(1)

#############  NUMERICAL DEFAULT DATA  #################
LOG_CYCLE = 1.0

#############  LOG DATA  #################
exec(open( "Log_data/log_std76_unit_rec.py").read())
log_std76_unit_rec( LOG_CYCLE )

#############  ASSIGNMENTS, CALLS, and CONTROL  #################

std76_unit.framework.data_file_name = "Unit_test/std1976.txt" 
std76_unit.framework.vars_file_name = "Unit_test/variables.txt" 


trick.sim_services.exec_set_terminate_time(1122)
