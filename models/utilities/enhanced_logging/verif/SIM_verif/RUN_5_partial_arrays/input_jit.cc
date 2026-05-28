#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_5_partial_arrays
Purpose: Test the assignment of partial arrays for logging)
*****************************************************************************/
extern "C"
int run_me()
{
  EnhancedLogging_SummaryGroup & group =
                    verif_test.logging_manager.new_summary_group( "JIT-group", true);
  group.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;
  group.add_variable( verif_test.logging_manager.variable_set_verif5);

  exec_set_terminate_time(9);
  return 0;
}
