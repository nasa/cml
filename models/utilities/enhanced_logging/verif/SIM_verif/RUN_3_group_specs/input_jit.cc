#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_3_group_specs
Purpose: Test the summary-logging-specification capabilities
*****************************************************************************/
extern "C"
int run_me()
{
  EnhancedLogging_SummaryGroup & group =
                verif_test.logging_manager.new_summary_group( "JIT-group",true);
  group.add_variable( verif_test.logging_manager.variable_set_verif1);
  group.logging_format =  EnhancedLogging_BaseGroup::CSV_FMT;

  // Conditions:
  EnhancedLogging_SummaryConditionBase & action_1 =
                              group.new_bool_condition( verif_test.sim_data.b1);
  action_1.name = "JIT-group: First hit";
  action_1.record_type = EnhancedLogging_SummaryConditionBase::DISCRETE;

  EnhancedLogging_SummaryConditionBase & action_2 =
                              group.new_bool_condition( verif_test.sim_data.b1);
  action_2.name = "JIT-group: Maxima";
  action_2.record_type = EnhancedLogging_SummaryConditionBase::MAX;

  EnhancedLogging_SummaryConditionBase & action_3 =
                              group.new_bool_condition( verif_test.sim_data.b1);
  action_3.name = "JIT-group: Minima";
  action_3.record_type = EnhancedLogging_SummaryConditionBase::MIN;

  EnhancedLogging_SummaryConditionBase & action_4 =
                              group.new_bool_condition( verif_test.sim_data.b1);
  action_4.name = "JIT-group: Maxima-Absolute";
  action_4.record_type = EnhancedLogging_SummaryConditionBase::ABS_MAX;

  EnhancedLogging_SummaryConditionBase & action_5 =
                              group.new_bool_condition( verif_test.sim_data.b1);
  action_5.name = "JIT-group: Minima-Absolute";
  action_5.record_type = EnhancedLogging_SummaryConditionBase::ABS_MIN;

  EnhancedLogging_SummaryConditionBase & action_6 =
                              group.new_bool_condition( verif_test.sim_data.b1);
  action_6.name = "JIT-group: Average";
  action_6.record_type = EnhancedLogging_SummaryConditionBase::AVG;

  EnhancedLogging_SummaryConditionBase & action_7 =
                              group.new_bool_condition( verif_test.sim_data.b1);
  action_7.name = "JIT-group: Average_Absolute";
  action_7.record_type = EnhancedLogging_SummaryConditionBase::ABS_AVG;

  EnhancedLogging_SummaryConditionBase & action_8 =
                              group.new_bool_condition( verif_test.sim_data.b1);
  action_8.name = "JIT-group: Average-Non-zero";
  action_8.record_type = EnhancedLogging_SummaryConditionBase::NZ_AVG;

  exec_set_terminate_time(9);
  return 0;
}
