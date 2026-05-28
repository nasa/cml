#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_1c_new_group
Purpose:
  Create a new empty group and populate it with all new variables.
  Create a new empty group and populate it with a combination of new
  variables and existing variables.
Notes:
- The create-and-add paradigm is not recommended; if variables already exist,
  they should be used directly rather than instructing the manager to process
  that specification again.
- here are no
  rules against doing it, so this tests that capability.
- Creating new variables can only be done via the manager, hence the
  "manager, add this variable to that group" syntax, rather than a "group,
  add this variable" syntax. The latter is only available for existing
  variables.
- The new variables being "created" may be recognized by the manager, in
  which case it will recycle its existing variable anyway and avoid
  replicating the creation effort.
*****************************************************************************/
extern "C"
int run_me()
{
  // create a new group, log with CSV
  EnhancedLogging_SummaryGroup & group1 =
      verif_test.logging_manager.new_summary_group( "data_group1",true);
  verif_test.logging_manager.add_variable( group1,"verif_test.sim_data.x1","alias_x1");
  verif_test.logging_manager.add_variable( group1,"verif_test.sim_data.x_vec");
  verif_test.logging_manager.add_variable( group1,"verif_test.sim_data.i1");
  verif_test.logging_manager.add_variable( group1,"verif_test.sim_data.b1");
  verif_test.logging_manager.add_variable( group1,"verif_test.sim_data.s1");
  verif_test.logging_manager.add_variable( group1,"verif_test.sim_data.e1");
  group1.logging_format = EnhancedLogging_BaseGroup::CSV;

  EnhancedLogging_SummaryGroup & group2 =
      verif_test.logging_manager.new_summary_group( "data_group2",true);
  verif_test.logging_manager.add_variable( group2,"verif_test.sim_data.x1","alias_x1");
  verif_test.logging_manager.add_variable( group2,"verif_test.sim_data.x_vec");
  verif_test.logging_manager.add_variable( group2,"verif_test.sim_data.i1");
  group2.add_variable( verif_test.sim_data.b1,
                       "bool-1");
  group2.add_variable( verif_test.sim_data.s1,
                       "string-1");
  verif_test.logging_manager.add_variable( group2,"verif_test.sim_data.e1");
  group2.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;

  exec_set_terminate_time(2);
  return 0;
}
