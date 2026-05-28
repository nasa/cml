#include "../S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_test1a_predefined
Purpose: Use only a predefined group
*****************************************************************************/
extern "C"
void run_test1a_predefined() {
  // use a pre-packaged group
  verif_test.logging_manager.prepare_group_verif1();
  verif_test.logging_manager.prepare_group_verif2( verif_test.sim_data);
  verif_test.logging_manager.prepare_group_verif3();

  verif_test.logging_manager.add_group(verif_test.logging_manager.group_verif1); // testing skip of duplicate.
}

/*****************************************************************************
run_test1b_modified_predefined
Purpose: Use a modified predefined group
*****************************************************************************/
extern "C"
void run_test1b_modified_predefined() {
  // use a pre-packaged group
  verif_test.logging_manager.prepare_group_verif1();

  // add another variable to an existing group, using the most recently added
  // group.
  EnhancedLogging_BaseGroup * group_ref = verif_test.logging_manager.get_last_group();
  verif_test.logging_manager.add_variable( *group_ref,
                                           "verif_test.sim_data.b1");
  group_ref->remove_variable("verif_test.sim_data.x1");
}

/*****************************************************************************
run_test1c_all_new
Purpose:
  Create a new empty group and populate it with all new variables.
  Note that this is not recommended; if variables already exist, they should
  be used rather than duplicating with new variables. But there are no
  rules against doing it, so this tests that capability.
*****************************************************************************/
extern "C"
void run_test1c_all_new() {
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
//  verif_test.logging_manager.add_variable( group2,"verif_test.sim_data.b1");
//  verif_test.logging_manager.add_variable( group2,"verif_test.sim_data.s1");
  group2.add_variable( verif_test.sim_data.b1,
                       "bool-1");
  group2.add_variable( verif_test.sim_data.s1,
                       "string-1");
  verif_test.logging_manager.add_variable( group2,"verif_test.sim_data.e1");
  group2.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;

}



/*****************************************************************************
run_test2_two_groups
Purpose: Add two groups to the manager
*****************************************************************************/
extern "C"
void run_test2_two_groups() {
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
}


/*****************************************************************************
run_test3_group_specs
Purpose: Test the group-specification capabilities (LoggingSpecification)
*****************************************************************************/
extern "C"
void run_test3_group_specs() {

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
}

/*****************************************************************************
run_test4_arrays
Purpose: Test the assignment of arrays for logging)
*****************************************************************************/
extern "C"
void run_test4_arrays() {
  // use a pre-packaged group
  verif_test.logging_manager.prepare_group_verif4();
}

/*****************************************************************************
run_test5_partial_arrays
Purpose: Test the assignment of partial arrays for logging)
*****************************************************************************/
extern "C"
void run_test5_partial_arrays() {
  EnhancedLogging_SummaryGroup & group =
                    verif_test.logging_manager.new_summary_group( "JIT-group", true);
  group.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;
  group.add_variable( verif_test.logging_manager.variable_set_verif5);
}
