#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_2_two_groups
Purpose: Add two groups to the manager
*****************************************************************************/
extern "C"
int run_me()
{
  // use a pre-packaged group
  verif_test.logging_manager.prepare_group_verif1();

  // create a new group, add a pre-packaged set of variables.
  EnhancedLogging_SummaryGroup & group2 =
                     verif_test.logging_manager.new_summary_group( "group2",true);
  group2.add_variable( verif_test.logging_manager.variable_set_verif1);

  // add another variable to an existing group
  EnhancedLogging_BaseGroup * sample_group =
                     verif_test.logging_manager.get_group( "verif_group1");
  verif_test.logging_manager.add_variable( *sample_group,
                                           "verif_test.sim_data.b1");

  // add another variable to an existing group, using the most recently added
  // group.
  EnhancedLogging_BaseGroup * group_ref =
                                    verif_test.logging_manager.get_last_group();
  verif_test.logging_manager.add_variable(*group_ref, "verif_test.sim_data.x1");
  verif_test.logging_manager.add_variable(*group_ref, "verif_test.sim_data.i1");

  exec_set_terminate_time(9);
  return 0;
}
