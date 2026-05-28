#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_1f_predefined_individual
Purpose: Use only predefined group, keep summary data in individual files
*****************************************************************************/
extern "C"
int run_me()
{
  // use a pre-packaged group
  verif_test.logging_manager.prepare_group_verif1();
  verif_test.logging_manager.prepare_group_verif2( verif_test.sim_data);
  verif_test.logging_manager.prepare_group_verif3();

  verif_test.logging_manager.add_group(verif_test.logging_manager.group_verif1); // testing skip of duplicate.
  verif_test.logging_manager.consolidate_summary_group_data = false;

  CMLMessage::set_publish_level( CMLMessage::Inform);
  exec_set_terminate_time(9);
  return 0;
}
