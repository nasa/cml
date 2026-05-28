#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_1a_predefined
Purpose:
  Use only predefined groups, and summarize the summary-data into a
  consolidated file.
*****************************************************************************/
extern "C"
int run_me()
{
  // use a pre-packaged group
  verif_test.logging_manager.prepare_group_verif1();
  verif_test.logging_manager.prepare_group_verif2( verif_test.sim_data);
  verif_test.logging_manager.prepare_group_verif3();

  verif_test.logging_manager.add_group(verif_test.logging_manager.group_verif1); // testing skip of duplicate.
  CMLMessage::set_publish_level( CMLMessage::Inform);

  exec_set_terminate_time(9);
  return 0;
}
