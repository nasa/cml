/*******************************TRICK HEADER******************************
PURPOSE: (A derivative of EnhancedLoggingManager to illustrate how to
          create project-specific configurations)

LIBRARY DEPENDENCY:
  ((../src/project_logging_manager.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Jan 2024) (ANTARES) (initial)))
**********************************************************************/
#ifndef ENHANCED_LOGGING_MANAGER_PROJECT_EXTENSION_HH
#define ENHANCED_LOGGING_MANAGER_PROJECT_EXTENSION_HH

#include "cml/models/utilities/enhanced_logging/include/enhanced_logging_manager.hh"
#include "cml/models/utilities/enhanced_logging/include/summary_logging_group.hh"
#include "cml/models/utilities/enhanced_logging/include/summary_logging_variable.hh"
#include "cml/models/utilities/enhanced_logging/include/summary_logging_condition.hh"
#include "cml/models/utilities/enhanced_logging/include/enhanced_logging_variable.hh"
#include "cml/models/vehicle_management/compound_events/include/event_trigger.hh"


#include <list>

// include any datas tructures being passed in for direct asssignment
#include "../../include/data_struc.hh"

struct ProjectLoggingManager : public EnhancedLogging_Manager
{
 public:

  EnhancedLogging_SummaryGroup group_verif1; /* (--) example group 1 */
  EnhancedLogging_SerialGroup  group_verif1a; /* (--) example group 1, serial, event */
  EnhancedLogging_SerialGroup  group_verif1b; /* (--) example group 1, serial, always */
  EnhancedLogging_SummaryGroup group_verif2; /* (--) example group 2 */
  EnhancedLogging_SummaryGroup group_verif3; /* (--) example group 3 */
  EnhancedLogging_SummaryGroup group_verif4; /* (--) example group 4 */

  EnhancedLogging_SummaryVariableT<double> var_x1; /* (--)
    An example of a standalone variable; this does not rely on the sim's
    ref-attributes to populate its members. This variable can be assigned
    (replicated) to as many logging-groups as desired.*/

  EnhancedLogging_SummaryVariableT<double> var_x2; /* (--)
    An example of a standalone variable; this does not rely on the sim's
    ref-attributes to populate its members. This variable can be assigned
    (replicated) to as many logging-groups as desired.*/

//    example set 1*/
  EnhancedLogging_VariableSet variable_set_verif1; /* (--)
    example set 1*/
  EnhancedLogging_VariableSet variable_set_verif2; /* (--)
    example set 2*/
  EnhancedLogging_VariableSet variable_set_verif3; /* (--)
    example set 3*/
  EnhancedLogging_VariableSet variable_set_verif4; /* (--)
    example set 4, used to test array-logging.*/
  EnhancedLogging_VariableSet variable_set_verif5; /* (--)
    example set 5, used to test partial array-logging.*/



  EnhancedLogging_VariableSet variable_set_verif_A; /* (--)
    example set of variables, being a set of variable instances (pointers)
    rather than a set of strings waiting to be turned into variable instances.*/

  EnhancedLogging_SummaryConditionBase     condition_base; /* (--)
    An example of a multi-use standard condition using its internal active
    flag to indicate condition satisfaction.*/
  EnhancedLogging_SummaryConditionBool     condition_bool; /* (--)
    An example of a multi-use standard condition using an external boolean
    reference to indicate condition satisfaction.*/
  EnhancedLogging_SummaryConditionEvent    condition_event; /* (--)
    An example of a multi-use standard condition using the triggering status
    of an internal compound-event to inddicate condition satisfaction.*/
  EnhancedLogging_SummaryConditionEventRef condition_event_ref; /* (--)
    An example of a multi-use standard condition using the triggering status
    of an external compound-event to inddicate condition satisfaction.*/

  EventTrigger<double> trigger_time_gt_2;
  EventTrigger<double> trigger_time_gt_5;
  EventTrigger<double> trigger_x1_gt_5;
  EventTrigger<int>    trigger_i1_lt_n9;


  ProjectLoggingManager( const double         & time,
                         const VerifTestStruc & sim_data,
                         CompoundEvent        & sample_event);
  virtual ~ProjectLoggingManager(){};


 // Methods to populate pre-instantiated groups in a single call:
  void prepare_group_verif1();
  void prepare_group_verif2( VerifTestStruc & sim_data);
  void prepare_group_verif3();
  void prepare_group_verif4();
};
#endif
