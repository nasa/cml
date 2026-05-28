#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_1b_modified_predefined
Purpose: Add modifications to a predefined group
*****************************************************************************/
extern "C"
int run_me()
{
  // use a pre-packaged group
  verif_test.logging_manager.prepare_group_verif1();

  // add another variable to an existing group, using the most recently added
  // group.
  EnhancedLogging_BaseGroup * group_ref = verif_test.logging_manager.get_last_group();
  verif_test.logging_manager.add_variable( *group_ref,
                                           "verif_test.sim_data.b1");
  group_ref->remove_variable("verif_test.sim_data.x1");

  exec_set_terminate_time(9);
  return 0;
}
