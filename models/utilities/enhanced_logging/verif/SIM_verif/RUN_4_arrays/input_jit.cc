#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_4_arrays
Purpose: Test the assignment of arrays for logging)
*****************************************************************************/
extern "C"
int run_me()
{
  // use a pre-packaged group
  verif_test.logging_manager.prepare_group_verif4();

  exec_set_terminate_time(9);
  return 0;
}
