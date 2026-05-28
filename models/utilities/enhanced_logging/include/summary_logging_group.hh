/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides the grouping of variables for summary-logging; the values of
  variables in this group will be recorded if they meet the constraints
  imposed by a summary evaluation-condition.)

LIBRARY DEPENDENCY:
   ((../src/summary_logging_group.cc)
   )

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/
#ifndef CML_ENHANCED_LOGGING_SUMMARY_GROUP_HH
#define CML_ENHANCED_LOGGING_SUMMARY_GROUP_HH

#include <string>
#include <list>
#include <vector>
#include <fstream> // ofstream

/*TODO Turner 2024/06
       Finish HDF5 formatting
#include "hdf5.h"
*/

#include "base_logging_group.hh"
#include "enhanced_logging_variable.hh"
#include "summary_logging_variable.hh"
#include "summary_logging_condition.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/vehicle_management/compound_events/include/compound_event.hh"

/*****************************************************************************
EnhancedLogging_SummaryGroup
Purpose:
  A EnhancedLogging_SummaryGroup manages a set of summary-logging-variables
  (EnhancedLogging_SummaryVariable), having these variables process and record
  data in response to a set of EnhancedLogging_SummaryConditionBase -- or
  derivative thereof -- instances representing evaluation conditions.

  Each SummaryLoggingVariable instance records a specific simulation variable,
  creating one value for each of the independent logging conditions.

  Each of the evaluation-condition (EnhancedLogging_SummaryConditionBase or
  derivative) instances, may have sub-conditions to arm, disarm, and trigger
  the "condition".

  An active and triggered evaluatation-condition will lead to the group
  processing its variables, evaluating the values of those variables against
  the corresponding recorded value, using comparison logic defined by the
  evaluation-condition. The "record-type" setting of the evaluation-condition
  will determine whether the current value of the variable gets recorded.
  At the end of the simulation, the last recorded value of each variable
  for each evaluation-condition can be logged.

  A group has an event internally that may be used to arm, disarm, or
  activate the entire group. The evaluation-conditions are only evaluated
  if the group's event is either triggered, or not populated at all.
*****************************************************************************/
class EnhancedLogging_SummaryGroup : public EnhancedLogging_BaseGroup
{
 protected:
  /* TODO Turner January 2024
   *   Here, we want STL-containers of pointers to access the variables and
   *   conditions associated with this group.
   *   Because these are group-specific instances (possibly group-specific
   *   copies of a generic instance), the instances used by this group are
   *   dynamically-allocated during the group configuration.
   *   As such, these could (should?) be containers of unique_pointers
   *   to the dynamically-allocated instances of SummaryLoggingVariableBase and
   *   EnhancedLogging_SummaryConditionBase respectively -- these instances
   *   will never *   be used anywhere other than within this group.
   *   However, Trick does not support STL-containers of unique pointers,
   *   balking in the checkpointing construction. Instead these must be
   *   STL-containers of raw pointers and have to be deleted in the destructor.
   *   Not a big deal, but in case anyone was wondering why, or looking for a
   *   neat little task if we are able to get past the Trick problem at some
   *   point.*/
  std::list< EnhancedLogging_SummaryConditionBase* > allocated_conditions;/* (--)
    List of pointers to the logging-condition instances dynamically-allocated
    by this class. This is necessary to suppoort memory cleanup at
    destruction because we cannot use containers of unique-pointers.*/
 public:
  std::vector< EnhancedLogging_SummaryCondition_GroupData > conditions; /* (--)
    List of instances containing references to the logging-conditions
    being used for determining whether and how the variables should record
    values this cycle, and summary data associated with each such condition.
    Note that these logging-conditions are independent of each other,
    and the variables will respond to each one independently.
    Do not confuse an event (which determines only whether values should
    be processed this cycle) and a logging-condition (which also includes
    how the values are to be considered).*/

  EnhancedLogging_SummaryGroup( std::string name,
                                const double & dyn_time);
  virtual ~EnhancedLogging_SummaryGroup();

  void initialize( const std::string &) override;
  void update() override;
  void shutdown() override;

  bool is_summary() override {return true;}
  void reset_summary_value( std::string, std::string);

  void log_group_csv(std::ofstream & fstream) override;
  void log_group_csv_fmt( std::ofstream & fstream) override;

  void add_variable( EnhancedLogging_Variable &) override;
  void add_variable( EnhancedLogging_VariableSet &) override;
  void add_variable_set( EnhancedLogging_VariableSet set) {add_variable(set);}
  void add_variable( EnhancedLogging_Variable &,
                     unsigned int ix_lo,
                     unsigned int ix_hi) override;
  void add_variable( const std::vector<EnhancedLogging_SummaryVariable *> &);
  void add_variable( const std::vector<EnhancedLogging_SummaryVariable *> &,
                     unsigned int ix_lo,
                     unsigned int ix_hi);
  void add_variable( EnhancedLogging_SummaryVariable * var);

  template <typename T>
  void add_variable( const T & var,
                     std::string name,
                     std::string units="--")
  {
    EnhancedLogging_BaseVariable * new_var =
        new EnhancedLogging_SummaryVariableT<T>( var,
                                                 name,
                                                 units);
    variables.push_back( new_var);
    allocated_variables.push_back( new_var);
  }
 protected:
  void add_variable_internal( EnhancedLogging_SummaryVariable * var);

 public:
  void add_condition( EnhancedLogging_SummaryConditionBase &);
  EnhancedLogging_SummaryConditionBase  & new_base_condition();
  EnhancedLogging_SummaryConditionEvent & new_event_condition();
  EnhancedLogging_SummaryConditionBool  & new_bool_condition( bool &);
  EnhancedLogging_SummaryConditionEventRef &
                                         new_event_condition( CompoundEvent &);
 protected:
  void check_conditions();
  unsigned int find_condition( const std::string&);

 public:

 // Method Templates
  /****************************************************************************
  add_variable method 3b: Clone and add a specific instance from direct
                          reference to an instance
  ****************************************************************************/
 public:
  template <typename T>
  void add_variable( const EnhancedLogging_SummaryVariableT<T> & variable)
  {
    if (check_alias( variable.name,
                     variable.alias)) {
      add_variable_internal( new EnhancedLogging_SummaryVariableT<T>(variable));
    }
  }

 private: // remove operator=, copy-constructor from SWIG accessibility
  EnhancedLogging_SummaryGroup( const EnhancedLogging_SummaryGroup&);
  EnhancedLogging_SummaryGroup operator=( const EnhancedLogging_SummaryGroup&);
};
#endif
