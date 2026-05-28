/********************************* TRICK HEADER ********************************
PURPOSE: (
  Provides an implementation of the JIT-input files that can be compiled
  directly into the sim for purposes of obtaining code-coverage metrics)

LIBRARY DEPENDENCY:
   ((../src/input_files.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (initial)))

Notes:
- The contents of this file should mostly match with the contents of
  Log_config/log_conig.cc, the difference being: FIXME
  - Log_config/log
*******************************************************************************/
#include "../include/input_files.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
ProjectInputFiles::ProjectInputFiles(
  VerifTestStruc & data,
  ProjectLoggingManager & manager)
  :
  sim_data(data),
  logging_manager(manager)
{}

/*****************************************************************************
run_1a_predefined
Purpose:
  Use only predefined groups, and summarize the summary-data into a
  consolidated file.
*****************************************************************************/
void
ProjectInputFiles::run_1a_predefined()
{
  // use a pre-packaged group
  logging_manager.prepare_group_verif1();
  logging_manager.prepare_group_verif2( sim_data);
  logging_manager.prepare_group_verif3();

  logging_manager.add_group(logging_manager.group_verif1); // testing skip of duplicate.
  CMLMessage::set_publish_level( CMLMessage::Inform);

  exec_set_terminate_time(9);
}

/*****************************************************************************
run_1b_modified_predefined
Purpose: Add modifications to a predefined group
*****************************************************************************/
void
ProjectInputFiles::run_1b_modified_predefined()
{
  // use a pre-packaged group
  logging_manager.prepare_group_verif1();

  // add another variable to an existing group, using the most recently added
  // group.
  EnhancedLogging_BaseGroup * group_ref = logging_manager.get_last_group();
  logging_manager.add_variable( *group_ref, "verif_test.sim_data.b1");
  group_ref->remove_variable("verif_test.sim_data.x1");

  exec_set_terminate_time(9);
}

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
void
ProjectInputFiles::run_1c_new_group()
{
  // create a new group, log with CSV
  EnhancedLogging_SummaryGroup & group1 =
      logging_manager.new_summary_group( "data_group1",true);
  logging_manager.add_variable( group1,"verif_test.sim_data.x1","alias_x1");
  logging_manager.add_variable( group1,"verif_test.sim_data.x_vec");
  logging_manager.add_variable( group1,"verif_test.sim_data.i1");
  logging_manager.add_variable( group1,"verif_test.sim_data.b1");
  logging_manager.add_variable( group1,"verif_test.sim_data.s1");
  logging_manager.add_variable( group1,"verif_test.sim_data.e1");
  group1.logging_format = EnhancedLogging_BaseGroup::CSV;

  EnhancedLogging_SummaryGroup & group2 =
      logging_manager.new_summary_group( "data_group2",true);
  logging_manager.add_variable( group2,"verif_test.sim_data.x1","alias_x1");
  logging_manager.add_variable( group2,"verif_test.sim_data.x_vec");
  logging_manager.add_variable( group2,"verif_test.sim_data.i1");
  group2.add_variable( sim_data.b1,
                       "bool-1");
  group2.add_variable( sim_data.s1,
                       "string-1");
  logging_manager.add_variable( group2,"verif_test.sim_data.e1");
  group2.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;

  exec_set_terminate_time(2);
}

/*****************************************************************************
run_2_two_groups
Purpose: Add two groups to the manager
*****************************************************************************/
void
ProjectInputFiles::run_2_two_groups()
{
  // use a pre-packaged group
  logging_manager.prepare_group_verif1();

  // create a new group, add a pre-packaged set of variables.
  EnhancedLogging_SummaryGroup & group2 =
                     logging_manager.new_summary_group( "group2",true);
  group2.add_variable( logging_manager.variable_set_verif1);

  // add another variable to an existing group
  EnhancedLogging_BaseGroup * sample_group =
                     logging_manager.get_group( "verif_group1");
  logging_manager.add_variable( *sample_group,
                                           "verif_test.sim_data.b1");

  // add another variable to an existing group, using the most recently added
  // group.
  EnhancedLogging_BaseGroup * group_ref =
                                    logging_manager.get_last_group();
  logging_manager.add_variable(*group_ref, "verif_test.sim_data.x1");
  logging_manager.add_variable(*group_ref, "verif_test.sim_data.i1");

  exec_set_terminate_time(9);
}

/*****************************************************************************
run_3_group_specs
Purpose: Test the summary-logging-specification capabilities
*****************************************************************************/
void
ProjectInputFiles::run_3_group_specs()
{
  EnhancedLogging_SummaryGroup & group =
                logging_manager.new_summary_group( "JIT-group",true);
  group.add_variable( logging_manager.variable_set_verif1);
  group.logging_format =  EnhancedLogging_BaseGroup::CSV_FMT;

  // Conditions:
  EnhancedLogging_SummaryConditionBase & action_1 =
                              group.new_bool_condition( sim_data.b1);
  action_1.name = "JIT-group: First hit";
  action_1.record_type = EnhancedLogging_SummaryConditionBase::DISCRETE;

  EnhancedLogging_SummaryConditionBase & action_2 =
                              group.new_bool_condition( sim_data.b1);
  action_2.name = "JIT-group: Maxima";
  action_2.record_type = EnhancedLogging_SummaryConditionBase::MAX;

  EnhancedLogging_SummaryConditionBase & action_3 =
                              group.new_bool_condition( sim_data.b1);
  action_3.name = "JIT-group: Minima";
  action_3.record_type = EnhancedLogging_SummaryConditionBase::MIN;

  EnhancedLogging_SummaryConditionBase & action_4 =
                              group.new_bool_condition( sim_data.b1);
  action_4.name = "JIT-group: Maxima-Absolute";
  action_4.record_type = EnhancedLogging_SummaryConditionBase::ABS_MAX;

  EnhancedLogging_SummaryConditionBase & action_5 =
                              group.new_bool_condition( sim_data.b1);
  action_5.name = "JIT-group: Minima-Absolute";
  action_5.record_type = EnhancedLogging_SummaryConditionBase::ABS_MIN;

  EnhancedLogging_SummaryConditionBase & action_6 =
                              group.new_bool_condition( sim_data.b1);
  action_6.name = "JIT-group: Maxima-Absolute (Record ABS)";
  action_6.record_type = EnhancedLogging_SummaryConditionBase::ABS_MAX_RECORD_ABS;

  EnhancedLogging_SummaryConditionBase & action_7 =
                              group.new_bool_condition( sim_data.b1);
  action_7.name = "JIT-group: Minima-Absolute (Record ABS)";
  action_7.record_type = EnhancedLogging_SummaryConditionBase::ABS_MIN_RECORD_ABS;

  EnhancedLogging_SummaryConditionBase & action_8 =
                              group.new_bool_condition( sim_data.b1);
  action_8.name = "JIT-group: Average";
  action_8.record_type = EnhancedLogging_SummaryConditionBase::AVG;

  EnhancedLogging_SummaryConditionBase & action_9 =
                              group.new_bool_condition( sim_data.b1);
  action_9.name = "JIT-group: Average_Absolute";
  action_9.record_type = EnhancedLogging_SummaryConditionBase::ABS_AVG;

  EnhancedLogging_SummaryConditionBase & action_10 =
                              group.new_bool_condition( sim_data.b1);
  action_10.name = "JIT-group: Average-Non-zero";
  action_10.record_type = EnhancedLogging_SummaryConditionBase::NZ_AVG;

  exec_set_terminate_time(9);
}

/*****************************************************************************
run_4_arrays
Purpose: Test the assignment of arrays for logging)
*****************************************************************************/
void
ProjectInputFiles::run_4_arrays()
{
  // use a pre-packaged group
  logging_manager.prepare_group_verif4();

  exec_set_terminate_time(9);
}

/*****************************************************************************
run_5_partial_arrays
Purpose: Test the assignment of partial arrays for logging)
*****************************************************************************/
void
ProjectInputFiles::run_5_partial_arrays()
{
  EnhancedLogging_SummaryGroup & group =
                    logging_manager.new_summary_group( "JIT-group", true);
  group.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;
  group.add_variable( logging_manager.variable_set_verif5);

  exec_set_terminate_time(9);
}
