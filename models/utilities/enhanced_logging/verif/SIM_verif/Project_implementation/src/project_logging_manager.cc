/*******************************TRICK HEADER******************************
PURPOSE: (A derivative of EnhancedLoggingManager to illustrate how to
          create project-specific configurations)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Jan 2024) (ANTARES) (initial)))
**********************************************************************/
#include "../include/project_logging_manager.hh"
#include "cml/models/utilities/enhanced_logging/include/summary_logging_group.hh"

/*****************************************************************************
Constructor
Notes:
- The variable sets do not need to be mutually exclusive of content.
*****************************************************************************/
ProjectLoggingManager::ProjectLoggingManager(
  const double         & time,
  const VerifTestStruc & sim_data,
  CompoundEvent  & sample_event)
  :
  EnhancedLogging_Manager(time),

  //*************************
  group_verif1(  "verif_group1",time),
  group_verif1a( "verif_group1a",time),
  group_verif1b( "verif_group1b",time),
  group_verif2(  "verif_group2",time),
  group_verif3(  "verif_group3",time),
  group_verif4(  "verif_group4",time),


  //*************************
  var_x1( sim_data.x1,
          "x1_standalone",
          "kg_plus"), // using non-standard units to track where they appear.
  var_x2( sim_data.x2,
          "x2_standalone",
          "N_plus"),

  //*************************
  variable_set_verif1({ {"verif_test.sim_data.x1", "x1_alias1"},
                        {"verif_test.sim_data.x2", "x2_alias1"}}),

  variable_set_verif2({ {"verif_test.sim_data.x1"},
                        {"verif_test.sim_data.i1"},
                        {"verif_test.sim_data.x1", "x1_alias2"}}),

  variable_set_verif3({ {"verif_test.sim_data.i1"},
                       {"verif_test.sim_data.b1"},
                       {"verif_test.sim_data.s1"},
                       {"verif_test.sim_data.e1"}}),
  variable_set_verif4({ { "verif_test.sim_data.x1"},
                       { "verif_test.sim_data.x_vec"},
                       { "verif_test.sim_data.x_arr5"}}),
  variable_set_verif5({ {"verif_test.sim_data.x_arr5[1][0][1]"},
                       {"verif_test.sim_data.x_vec[1]"}}),

  //*************************
  condition_base(),
  condition_bool(      sample_event.event_triggered),
  condition_event(     sim_data.dyn_time),
  condition_event_ref( sample_event),

  //*************************
  // Note: 1st argument is to the delay-variable, not to the watch-variable.
  trigger_time_gt_2( time,
                     "time > 2"),
  trigger_time_gt_5( time,
                     "time > 5"),
  trigger_x1_gt_5(   time,
                     "x1 > 5"),
  trigger_i1_lt_n9 ( time,
                     "i1 < -9")


{
  consolidate_summary_group_data = true;
  logging_format = EnhancedLogging_BaseGroup::CSV_FMT;

  variable_set_verif_A.add_variable( var_x1);
  variable_set_verif_A.add_variable( var_x2);

  trigger_time_gt_2.set_watch( time, 2.0);
  trigger_time_gt_2.comparison_logic = EventTriggerBase::GT;
  trigger_time_gt_2.message = "Time > 2 trigger detected.";

  trigger_time_gt_5.set_watch( time, 5.0);
  trigger_time_gt_5.comparison_logic = EventTriggerBase::GT;
  trigger_time_gt_5.message = "Time > 5 trigger detected.";

  trigger_x1_gt_5.set_watch( sim_data.x1, 5.0);
  trigger_x1_gt_5.comparison_logic = EventTriggerBase::GT;
  trigger_x1_gt_5.message = "x1 > 5 trigger detected.";

  trigger_i1_lt_n9.set_watch( sim_data.i1, -9.0);
  trigger_i1_lt_n9.comparison_logic = EventTriggerBase::LT;
  trigger_i1_lt_n9.message = "i1 < -9 trigger detected.";
}



/*****************************************************************************
prepare_group_verif1
Purpose:
  An example of how groups may be populated by pre-compiled data sets.
  All conditions in this group cue off the same signal; the set of conditions
  tests the evaluates the 8 record-types.
  The variables added to this group are from the _NameAlias predefined sets.
*****************************************************************************/
void ProjectLoggingManager::prepare_group_verif1()
{
  add_group(group_verif1a);
  add_group(group_verif1b);
  add_group(group_verif1);

  add_variable( group_verif1,
                variable_set_verif1);
  add_variable( group_verif1,
                variable_set_verif2);

  add_variable( group_verif1a,
                variable_set_verif1);
  add_variable( group_verif1a,
                variable_set_verif2);

  add_variable( group_verif1b,
                variable_set_verif1);
  add_variable( group_verif1b,
                variable_set_verif2);
  add_variable( group_verif1b,
                variable_set_verif3);

  group_verif1.event.arming_triggers.add_trigger( trigger_time_gt_2);
  group_verif1.event.arming_triggers.add_trigger( trigger_x1_gt_5);
  group_verif1.event.arming_triggers.require_all = true;
  group_verif1.event.arming_triggers.message = "Group Verif1 armed.";

  group_verif1.event.action_triggers.add_trigger( trigger_time_gt_5);
  group_verif1.event.action_triggers.message = "Group Verif1 ready to evaluate conditions.";
  group_verif1.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;

  group_verif1a.event.arming_triggers.add_trigger( trigger_time_gt_2);
  group_verif1a.event.arming_triggers.add_trigger( trigger_x1_gt_5);
  group_verif1a.event.arming_triggers.require_all = true;
  group_verif1a.event.arming_triggers.message = "Group Verif1a armed.";

  group_verif1a.event.action_triggers.add_trigger( trigger_time_gt_5);
  group_verif1a.event.action_triggers.message = "Group Verif1a ready to evaluate conditions.";
  group_verif1a.set_log_spec( EnhancedLogging_SerialGroup::EVENT);
  group_verif1a.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;

  group_verif1b.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;

  // Conditions:
  EnhancedLogging_SummaryConditionBase & action_1 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_1.name = "Group1: First hit";
  action_1.record_type = EnhancedLogging_SummaryConditionBase::DISCRETE;

  EnhancedLogging_SummaryConditionBase & action_2 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_2.name = "Group1: Maxima";
  action_2.record_type = EnhancedLogging_SummaryConditionBase::MAX;

  EnhancedLogging_SummaryConditionBase & action_3 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_3.name = "Group1: Minima";
  action_3.record_type = EnhancedLogging_SummaryConditionBase::MIN;

  EnhancedLogging_SummaryConditionBase & action_4 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_4.name = "Group1: Maxima-Absolute";
  action_4.record_type = EnhancedLogging_SummaryConditionBase::ABS_MAX;

  EnhancedLogging_SummaryConditionBase & action_5 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_5.name = "Group1: Minima-Absolute";
  action_5.record_type = EnhancedLogging_SummaryConditionBase::ABS_MIN;

  EnhancedLogging_SummaryConditionBase & action_6 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_6.name = "Group1: Maxima-Absolute (Record ABS)";
  action_6.record_type = EnhancedLogging_SummaryConditionBase::ABS_MAX_RECORD_ABS;

  EnhancedLogging_SummaryConditionBase & action_7 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_7.name = "Group1: Minima-Absolute (Record ABS)";
  action_7.record_type = EnhancedLogging_SummaryConditionBase::ABS_MIN_RECORD_ABS;

  EnhancedLogging_SummaryConditionBase & action_8 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_8.name = "Group1: Average";
  action_8.record_type = EnhancedLogging_SummaryConditionBase::AVG;

  EnhancedLogging_SummaryConditionBase & action_9 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_9.name = "Group1: Average_Absolute";
  action_9.record_type = EnhancedLogging_SummaryConditionBase::ABS_AVG;

  EnhancedLogging_SummaryConditionBase & action_10 = group_verif1.new_bool_condition(
                             group_verif1.event.action_triggers.event_triggered);
  action_10.name = "Group1: Average-Non-zero";
  action_10.record_type = EnhancedLogging_SummaryConditionBase::NZ_AVG;

  subscribe_group(group_verif1);
  subscribe_group(group_verif1a);
  subscribe_group(group_verif1b);
}

/*****************************************************************************
prepare_group_verif2
Purpose:
  An example of how a group may be populated by pre-compiled data sets.
  In this example, we use conditions that are themselves more complicated
  events.
*****************************************************************************/
void ProjectLoggingManager::prepare_group_verif2(
  VerifTestStruc & sim_data)
{
  add_group(group_verif2);
  add_variable( group_verif2,
                variable_set_verif_A);
  subscribe_group(group_verif2);

  group_verif2.event.action_triggers.add_trigger( trigger_time_gt_5);
  group_verif2.event.action_triggers.message = "Group Verif2 ready";
  group_verif2.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;

  /* give this group conditions that use CompoundEvent instances.
   * Condition 1 has its own dedicated CompoundEvent instance
   * Condition 2 utilizes a CompoundEvent instance from the sim.*/
  EnhancedLogging_SummaryConditionEvent & action_1 =
                                               group_verif2.new_event_condition();
  action_1.name = "Group2: i1 <  -9: First hit";
  action_1.record_type = EnhancedLogging_SummaryConditionBase::DISCRETE;
  action_1.event.action_triggers.add_trigger( trigger_i1_lt_n9);
  action_1.event.action_triggers.message = "Group 2: Action 1: triggered";

  //*******
  EnhancedLogging_SummaryConditionEventRef & action_2 =
                                 group_verif2.new_event_condition( sim_data.event);
  action_2.name = "Group2: i1 != -9: First hit";
  action_2.record_type = EnhancedLogging_SummaryConditionBase::DISCRETE;
}

/*****************************************************************************
prepare_group_verif3
Purpose:
  An example of how a group may be populated by pre-compiled data sets.
*****************************************************************************/
void ProjectLoggingManager::prepare_group_verif3()
{
  add_group(group_verif3);

  add_variable( group_verif3,
                variable_set_verif3);
  add_variable( group_verif3,
                variable_set_verif1);
  add_group(group_verif3);
  subscribe_group(group_verif3);

  group_verif3.event.arming_triggers.add_trigger( trigger_time_gt_2);
  group_verif3.event.arming_triggers.message = "Group Verif3 armed";

  group_verif3.event.disarming_triggers.add_trigger( trigger_i1_lt_n9);
  group_verif3.event.disarming_triggers.message = "Group Verif3 disarmed";

  EnhancedLogging_SummaryConditionBase & action_1 =
                                                group_verif3.new_base_condition();
  action_1.name = "Group3: Max from t=2 to i1 < -9";
  action_1.record_type = EnhancedLogging_SummaryConditionBase::ABS_MAX;

  group_verif3.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;
//  group_verif3.logging_format = EnhancedLogging_BaseGroup::CSV;
}


/*****************************************************************************
prepare_group_verif4
Purpose:
  An example of how a group may be populated by pre-compiled data sets.
*****************************************************************************/
void ProjectLoggingManager::prepare_group_verif4()
{
  group_verif4.logging_format = EnhancedLogging_BaseGroup::CSV_FMT;
  add_group(group_verif4);
  add_variable( group_verif4,
                variable_set_verif4);
  subscribe_group(group_verif4);
}
