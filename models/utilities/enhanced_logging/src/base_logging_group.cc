/*******************************TRICK HEADER******************************
PURPOSE: (Provides the abstract base-class for the serial-logging group
          and summary-=logging-group.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/

#include <algorithm>

#include "cml/models/utilities/env_utils/include/env_utils.h"

#include "../include/base_logging_group.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh"
/*****************************************************************************
Constructor
*****************************************************************************/
EnhancedLogging_BaseGroup::EnhancedLogging_BaseGroup(
  std::string name_,
  const double & dyn_time_)
  :
  dyn_time(dyn_time_),
  logging_format( CSV_FMT),
  event(dyn_time),
  name(name_),
  minimum_update_period(0),
  create_individual_group_file( true),
  always_triggered( false),
  t_next_update(0),
  t_last_update(0)
{
  log_hdf5_py_file = std::string(getenv_or_exit("CML_HOME"))+\
            "/models/utilities/summary_logging/src/log_hdf5.py";

  /* The group's CompoundEvent is not managed by any event manager.
     The event testing is handled by the routine update() method, which
     is called from the summary logging manager so this event should not
     be added to an event-manager.
   */
  event.add_self_to_manager_active_list = false;
}

/*****************************************************************************
activate
Purpose: Activates the group.
*****************************************************************************/
void
EnhancedLogging_BaseGroup::activate()
{
  if (!enabled || active) {return;}

  if (create_individual_group_file) {
    if (!file_out.is_open()) {
      file_out.open (csv_file);
      // if still not open, quit.
      if (!file_out.is_open()) {
        CMLMessage::error( __FILE__,__LINE__,
          "Could not open logging file with name: ",csv_file);
        return;
      }
      // If opened for the first time, initialize any content needed.
      else {
        initialize_file();
      }
    }
  }

  event.subscribe();
  SubscriptionBase::activate();
  /* Set flags to allow an update: set the active flag to true and set the
   * recorded times to an arbitrary value less than time.*/
  t_next_update = t_last_update = dyn_time - 1.0;
  /* Only update the group if activation is a consequence of a subscription,
   * don't if it a consquence of initialization.*/
  if (SubscriptionBase::sub_pending == 0) {update();}
}

/*****************************************************************************
initialize
Purpose:
  Executed at the end of the serial/summary-group specific initialization
  - Checks target directory name where group's output data will be placed
  - Executes the SubscriptionBase::initialize()
  - sets the always-triggered flag if the built-in event is empty.
Notes:
  The target-dir_ argument is the default target directory, passed in from
  the logging-manager. If target_dir has been assigned manually, that
  assignment takes precedence over the default.
*****************************************************************************/
void
EnhancedLogging_BaseGroup::initialize( const std::string & target_dir_)
{
  /* If target_dir has not been set manually, use the value coming in from
   * the logging manager as the default -- this will put all logging files
   * in the same location.*/
  if (target_dir.empty()) {target_dir = target_dir_;}

  /* Note -- order is important in initialization.
   * With a subscription already in place, SubscriptionBase baseline-
   * initialization will lead to activation and an update.
   * With the controlling trigger --either the event's trigger or the
   * always_triggered flag -- set to true, that would result in capturing
   * the **current** values of the variables, which is generally
   * undesirable.
   * So ensure that the SubscriptionBase::initialization -- and its
   * consequentially-possible activation, and first update executions --
   * is complete BEFORE the possibility of setting the
   * always_triggered flag.*/

  csv_file = target_dir+"/log_"+name+".csv";
  SubscriptionBase::initialize();

  /* The group may -- or may not -- be utilizing the CompoundEvent
   * capabilities to "arm the group", and evaluate a group-controlling
   * set of action-triggers.
   * If the group does not require arming, AND there are no
   * action-triggers, then the group is always active. The event-nature of
   * the group can be bypassed, which is easily accomplished by not
   * initializing it. In these conditions, set the always_triggered flag
   * Otherwise, initialize the CompoundEvent aspect of this class.*/
  if ( event.arming_triggers.get_num_triggers() == 0 &&
       event.action_triggers.get_num_triggers() == 0) {
    always_triggered = true;
  } else {
    /* The event is not added to an event-manager, so the list of
     * active-watches maintained by the manager is null.*/
    event.initialize(nullptr);
  }
}

/*****************************************************************************
update_base
Purpose:
  Executed from the sertial/summary-group update method to check on the
  scheduling that is common to both types of groups.
*****************************************************************************/
bool
EnhancedLogging_BaseGroup::update_base()
{
  if ( !active ||
       (dyn_time < t_next_update) ||
       (MathUtils::is_near_equal( dyn_time, t_last_update))) {
    return false;
  }
  // Update the reference times.
  t_next_update = dyn_time + minimum_update_period;
  t_last_update = dyn_time;
  return true;
}

/*****************************************************************************
log_group
Purpose:
  Logs the values for all variables in this group.
Notes:
  the log_group_<format> methods called from here will go to the
  serial/summary-group specific implemeentations.
*****************************************************************************/
void
EnhancedLogging_BaseGroup::log_group()
{
  switch (logging_format) {
  case CSV:
    log_group_csv(file_out);
    break;
  case CSV_FMT:
  case HDF5:
    log_group_csv_fmt(file_out);
    break;
  default:
    // no action.
    break;
  }
}
/****************************************************************************/
void
EnhancedLogging_BaseGroup::log_group(std::ofstream & fstream)
{
  switch (logging_format) {
  case CSV:
    log_group_csv(fstream);
    break;
  case CSV_FMT:
  case HDF5:
    log_group_csv_fmt(fstream);
    break;
  default:
    // no action.
    break;
  }
}

/*****************************************************************************
check_alias
Purpose:
  Ensures there are not two variables in the group being logged with the
  same alias.
Return:
  true if alias is unique
*****************************************************************************/
bool
EnhancedLogging_BaseGroup::check_alias( const std::string & name_,
                                        const std::string & alias_)
{
  /* Alias must be unique, search through all existing aliases to see whether
  *  any match.*/
  auto it = std::find_if( variables.begin(), variables.end(),
                          [alias_]( EnhancedLogging_BaseVariable* var) {
                            return (var->alias == alias_);}
                        );

  /* Then - if a matching alias found with different name, post error
   *      - if a matching alias found with matching name, it is a straight
   *        duplicate, just ignore it.
   *      - if no matching alias was found, return true to indicate that this
   *        item should be added to the list.
  */
  if (it != variables.end()) {
    if (name_ != (*it)->name) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot add a variable to a group with an alias matching an existing "
        "member.\n"
        "Failed to add variable ",name_," with alias ",alias_," to group ",
        name,"\nbecause alias ", alias_," is already "
        "associated with variable ", (*it)->name,"\n");
    }
    return false;
  }
  return true;
}

/*****************************************************************************
add_variable
Purpose:
  Add a loggable variable to the variables list
Notes:
- Usable instances of EnhancedLogging_SerialVariableT<T> and
  EnhancedLogging_SummaryVariableT<T> must be group-specific because
  serial-variables are only associated with a serial-group, and
  summary-variables only with summary-groups.
  Furthermore, summary-variables must have their data storage vectors sized
  according to the number of conditions in their specific groups.
  Consequently, all add_variable methods for summary-groups ultimately result
  in dynamic-allocation of one or more EnhancedLogging_SummaryVariableT<T>
  instance.
  Conversely, serial-logging-variables can be shared by multiple
  serial-groups, so add_variable methods targeting serial-groups can utilize
  (matching) serial-variables already allocated -- e.g. these may be found in
  the proto-logging-variable instance passed in to some of these methods
  For a proto-logging-variable (i.e. an instance of EnhancedLogging_Variable)
  that has not previously been added to a group, the add_variable method
  will result in the creation a set of either
  - EnhancedLogging_SerialVariableT<T> or
  - EnhancedLogging_SummaryVariableT<T>
  variables according to the group-type. Once created, each list can be used
  by subsequent groups of similar type (potentially, both lists may be created).
  - Each serial-group will simply access the set of
    EnhancedLogging_SerialVariableT<T> instances
  - Each summary-group will clone the set of
    EnhancedLogging_SummaryVariableT<T> instances for use by that group.
- Construction of a serial- or summary- variables from variable-name lookup
  requires using Trick's ref-attributes lookup to identify the type and
  location of that variable. This operation is managed by the
  EnhancedLogging_Variable (aka proto-logging-variable) class, which makes
  that lookup available to both serial and summary variable implementations.
  The methods to add serial and sumamry variables to a serial and summary
  group -- as baselined here -- use instances of EnhancedLogging_Variable.
  Such instances can be created on-the-fly, although that is not recommended.
*****************************************************************************/

/****************************************************************************
Method 1a: Add the logging variables associated with an
           EnhancedLogging_Variable instance;
           this is a specialized operation delegated to either the
           serial-group or summary-group specializations of this class.*/

/****************************************************************************
Method 1b: Add the logging variables associated with each of a list of
           EnhancedLogging_Variable instances;*/
void
EnhancedLogging_BaseGroup::add_variable(
  std::list< EnhancedLogging_Variable> & var_list)
{
  for (EnhancedLogging_Variable & var : var_list) {
    add_variable( var);
  }
}
/****************************************************************************
Method 1c: Add the logging variables associated with each of a set of
           EnhancedLogging_Variable instances;
           this is a specialized operation delegated to either the
           serial-group or summary-group specializations of this class.*/

/****************************************************************************
Method 1d: Add a subset of the logging variables associated with an
           EnhancedLogging_Variable instance;
           this is a specialized operation delegated to either the
           serial-group or summary-group specializations of this class.*/


/*****************************************************************************
remove_variable
Purpose: Remove a loggable variable from the variables list.
Notes:
- summary-variables are specific to each summary-group and dynamically
  allocated to that group. If the variable being removed is a sumamry-variable,
  it must be destroyed. Conversely, a serial-variable can be left intact,
  removing the pointer to it from the group is sufficient.
*****************************************************************************/
void
EnhancedLogging_BaseGroup::remove_variable( std::string var_alias)
{
  for (auto it = variables.begin(); it != variables.end(); ++it) {
    if ((*it)->alias == var_alias) {
      if ( (*it)->is_summary()) {
        delete *it;
      }
      variables.erase(it);
      return;
    }
  }
  CMLMessage::warn( __FILE__,__LINE__,
    "Variable with alias ",var_alias," not found in group ",name,".\n"
    "Cannot remove it from the group.\n");
}

/*****************************************************************************
Name: find_variable
Purpose:
  Returns a pointer to the variable with matching alias
*****************************************************************************/
EnhancedLogging_BaseVariable*
EnhancedLogging_BaseGroup::find_variable (const std::string var_alias_)
{
  auto it = std::find_if( variables.begin(), variables.end(),
                          [var_alias_]( EnhancedLogging_BaseVariable* var) {
                            return (var->alias == var_alias_);}
                        );
  if (it == variables.end()) {
    CMLMessage::error( __FILE__,__LINE__,
      "Did not find a variable in group ",name," with alias ",var_alias_,".\n");
    return nullptr;
  }
  return (*it);
}
