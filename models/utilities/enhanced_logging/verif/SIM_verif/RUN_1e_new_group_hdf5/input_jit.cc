#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_1e_new_group_hdf5
Purpose:
  Repeat RUN_1c, and create a HD5 summary file.
Notes:
  See notes for RUN_1c
*****************************************************************************/
extern "C"
int run_me()
{
  verif_test.logging_manager.logging_format = EnhancedLogging_BaseGroup::HDF5;

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
