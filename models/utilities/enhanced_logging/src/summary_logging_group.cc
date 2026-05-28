/*******************************TRICK HEADER******************************
PURPOSE: ()


PROGRAMMERS:
  (((Gary Turner) (OSR) (date) (project) (comment)))
***********************************************************************/

#include <iomanip> // setw
#include <algorithm> // find_if

#include "../include/summary_logging_group.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh"
/*****************************************************************************
Cosntructor
*****************************************************************************/
EnhancedLogging_SummaryGroup::EnhancedLogging_SummaryGroup(
  std::string name_,
  const double & dyn_time_)
  :
  EnhancedLogging_BaseGroup(name_, dyn_time_)
{
  subscribe_name = "EnhancedLogging_SummaryGroup:"+name;
}

/*****************************************************************************
Destructor
*****************************************************************************/
EnhancedLogging_SummaryGroup::~EnhancedLogging_SummaryGroup()
{
  // All variables for a Summary Group are clones of the originals, so
  // delete them all (originals remain intact).
  for (auto * var : variables) {
    delete var;
  }
  for (auto * cond : allocated_conditions) {
    delete cond;
  }
}

/*****************************************************************************
Name: initialize
Purpose:
  Initializes the evaluation-conditions and summary-variables, and passes
  up to the base-class initialization.
*****************************************************************************/
void
EnhancedLogging_SummaryGroup::initialize( const std::string & target_dir_)
{
  for (auto & cond : conditions) {
    cond.initialize();
  }

  size_t cond_size = conditions.size();
  /* Safety valve. If there are no conditions, there will be no data
   * recorded. Default operation is to record the data at the time the
   * group is fully activated and its triggers are satisfied.*/
  if (cond_size == 0) {
    CMLMessage::warn( __FILE__, __LINE__,
      "No conditions were specified for group ", name,".\n"
      "Adding a single default condition to record the first set of data\n"
      "after the group meets its action-triggers.\n");
    new_base_condition();
    cond_size = 1;
  }

  for (auto & var : variables) {
    var->initialize(cond_size);
  }

  EnhancedLogging_BaseGroup::initialize( target_dir_);
}

/*****************************************************************************
Name: update
Purpose:
  Main executable resulting in assignments of values to the var variables
  in the group's vars vector.
Notes:
- Called from manager's list of enabled groups.
*****************************************************************************/
void
EnhancedLogging_SummaryGroup::update()
{
  if (!EnhancedLogging_BaseGroup::update_base()) {return;}

  /* Continue on to check the group's conditions if either:
     - the group has no configured event so is marked as always-triggered, or
     - the event is configured and the update_no_manager() method returns
       true, indicating either:
       - the event has satisfied its action-triggers, evaluated within the
         test-crossing() method, or:
       - the event has no action-triggers and has satisfied its
         arming-triggers.
       Note: update_no_manager is an interface method to avoid requiring
             an event manager for just this one event.*/
  if (always_triggered ||
      event.update_no_manager()) {
    check_conditions();
  }
}

/*****************************************************************************
Name: shutdown
Purpose:
  Shuts the group down, closing out any (all) logging of variables.
*****************************************************************************/
void
EnhancedLogging_SummaryGroup::shutdown()
{
  if (file_out.is_open()) {
    log_group();
    file_out.close();
  }
}

/*****************************************************************************
Name: check_conditions
Purpose:
  Checks each of the conditions to evaluate whether to record the current
  values.
*****************************************************************************/
void
EnhancedLogging_SummaryGroup::check_conditions()
{
  // Check each of the conditions.
  for (auto & cond : conditions) {
    if (cond.update(dyn_time)) {
      for (auto & var : variables) {
        var->populate_condition( cond.get_ID(),
                                 cond.get_record_type());
      }
    }
  }
}

/*****************************************************************************
Name: find_condition
Purpose:
  Returns the index of a condition of specified name.
*****************************************************************************/
unsigned int
EnhancedLogging_SummaryGroup::find_condition( const std::string & cond_name)
{
  auto it = std::find_if( conditions.begin(), conditions.end(),
                [cond_name](EnhancedLogging_SummaryCondition_GroupData & cond_){
                return (cond_.get_name() == cond_name);}
               );
  if (it != conditions.end()) {return (*it).get_ID();}

  CMLMessage::error( __FILE__,__LINE__,
    "Did not find a condition in group ",name," with name ",cond_name,
    ".\n");
  return -1;
}

/*****************************************************************************
Name: reset_summary_value
Purpose:
  For the variable with specified alias, resets the summary value
  associated with the specified condition.
*****************************************************************************/
void
EnhancedLogging_SummaryGroup::reset_summary_value(
  std::string var_alias,
  std::string cond_name)
{
  // Match with the variable alias; exit if failed
  EnhancedLogging_BaseVariable* var = find_variable( var_alias);
  if (!var) {return;}
  // Match with the condition name; exit if failed
  int cond_ix = find_condition( cond_name);
  if (cond_ix < 0) {return;}
  // Reset the specified variable's summary value at that index.
  var->reset_summary_val(cond_ix);
}



/*****************************************************************************
log_group_csv
Purpose:
  Simply directs the output ofstream content to a csv file.
*****************************************************************************/
void
EnhancedLogging_SummaryGroup::log_group_csv(std::ofstream & fstream)
{
  fstream << "condition_name {--}, condition_type {--}," <<
              "condition_count {--}, condition_first_time {s}," <<
              "condition_last_time {s}";
  for (auto & var : variables) {
    fstream << ", " << var->alias << "{" << var->units << "}";
  }
  fstream << std::endl;

  for (const EnhancedLogging_SummaryCondition_GroupData & cond : conditions) {
    fstream << cond.get_name() << "," <<
            cond.get_record_type() << "," <<
            cond.get_count();
    if (cond.get_count() > 0) {
      fstream  << "," << cond.get_first_time() << "," << cond.get_last_time();
      for (auto & var : variables) {
        var->log_value( cond.get_ID(),
                        fstream);
      }
    }
    fstream << std::endl;
  }
}

/*****************************************************************************
log_group_csv_fmt
Purpose:
  Directs the output ofstream content to a consistently-formatted csv file.
*****************************************************************************/
void
EnhancedLogging_SummaryGroup::log_group_csv_fmt( std::ofstream & fstream)
{
  fstream << "\nGroup: " << name << "\n" <<
              "condition_name {--}, condition_type {--}," <<
              "condition_count {--}, condition_first_time {s}," <<
              "condition_last_time {s}";
  for (auto & var : variables) {
    fstream << ", " << var->alias << "{" << var->units << "}";
  }
  fstream << std::endl;

  /* For uniform formatting, get the length of the longest condition-name
   * so that we can reserve that length for all condition-names.*/
  size_t cond_name_size=0;
  for (const EnhancedLogging_SummaryCondition_GroupData & cond : conditions) {
    size_t cond_name_size_ = cond.get_name().size();
    if (cond_name_size_ > cond_name_size) {
      cond_name_size = cond_name_size_;
    }
  }

  for (const EnhancedLogging_SummaryCondition_GroupData & cond : conditions) {
    fstream << std::setw(cond_name_size) << std::left <<
                cond.get_name() << std::right <<
                "," << std::setw(2) << cond.get_record_type() <<
                "," << std::setw(7) << cond.get_count();
    if (cond.get_count() > 0) {
      fstream  << "," << std::setprecision(3) << std::fixed <<
                   std::setw(11) << cond.get_first_time() << "," <<
                   std::setw(11) << cond.get_last_time();
      for (auto & var : variables) {
        var->log_value_fmt( cond.get_ID(),
                            fstream);
      }
    }
    fstream << std::endl;
  }
}

/*****************************************************************************
Name: add_variable
Purpose:
  Add one or more summary-variables to the variables list
Note:
- Usable instances of EnhancedLogging_SummaryVariableT<T> must be group-specific
  because they have the data storage vector that must be sized according to
  the number of conditions in the group. Consequently, all
  add_variable methods ultimately result in dynamic-allocation of new
  EnhancedLogging_SummaryVariableT<T> instance(s) specifically for this group.
- We have several methods for adding a variable to this group:
  1: Using one or more proto-logging-variables (EnhancedLogging_Variable),
     the associated EnhancedLogging_SummaryVariableT<T> instances are passed
     (by base-class pointer of type EnhancedLogging_SummaryVariable) to
     stage 2. The associated EnhancedLogging_SummaryVariableT<T> instances
     will be created if they do not already exist for the specified
     EnhancedLogging_Variable.
     An option exists to select a contiguous subset of the summary-variables
     associated with the provided variable.
  2: A set of instances of EnhancedLogging_SummaryVariableT<T>, accessed by
     generic base-class pointer EnhancedLogging_SummaryVariable
     is broken out and the individual instances passed to step 3.
       (e.g. the set of instances found within each instance of
             EnhancedLogging_Variable from step 1)
     An option exists to select a contiguous subset of the summary-variables
     from within the provided list.
  3a: A pre-existing instance of EnhancedLogging_SummaryVariableT<T> is cloned
     into this group using its base-class EnhancedLogging_SummaryVariable
     pointer.
     Because each instance of EnhancedLogging_SummaryVariableT<T>
     has to store as many values as there are logging conditions in the group,
     each group must have its own independent instance of
     EnhancedLogging_SummaryVariableT<T> even when multiple groups are logging
     the same simulation-variable.
  3b: Alternative process -- a pre-existing EnhancedLogging_SummaryVariableT<T>
     instance can be cloned directly into this group without going through the
     base-class pointer. Because the instance is based on a class-template,
     the method needed for this operation must be based on a method-template;
     that method-template is defined in the header file.
  4: The newly-cloned instance is added to the group's list of variables and
     initialized if the group has already been initialized.
     This step is provided by method add_variable_internal, which is a
     protected method and only called from steps 3a or 3b
*****************************************************************************/

/****************************************************************************
Method 1a: Clone and add the EnhancedLogging_SummaryVariable produced by
           an external EnhancedLogging_Variable.*/
void
EnhancedLogging_SummaryGroup::add_variable(
  EnhancedLogging_Variable & var)
{
  add_variable( var.get_summary_variable_list());
}
/****************************************************************************
Method 1b: Add the serial-variables associated with each of a list of
           EnhancedLogging_Variable instances. This capability is provided in
           EnhancedLogging_BaseGroup.*/

/****************************************************************************
Method 1c: Add the logging variables associated with each of a set of
           EnhancedLogging_Variable instances.*/
void
EnhancedLogging_SummaryGroup::add_variable(
  EnhancedLogging_VariableSet & set)
{
  add_variable(set.get_summary_variable_list());
}
/****************************************************************************
Method 1d: Add a subset of the summary-variables associated with an
           EnhancedLogging_Variable instance.*/
void
EnhancedLogging_SummaryGroup::add_variable(
  EnhancedLogging_Variable & var,
  unsigned int ix_lo,
  unsigned int ix_hi)
{
  add_variable( var.get_summary_variable_list(),
                ix_lo,
                ix_hi);
}

/****************************************************************************
Method 2a: Clone and add all summary-variables from a list.*/
void
EnhancedLogging_SummaryGroup::add_variable(
  const std::vector<EnhancedLogging_SummaryVariable *> & var_set)
{
  for (EnhancedLogging_SummaryVariable * var : var_set) {
    add_variable( var);
  }
}
/****************************************************************************
Method 2b: Clone and add a subset of summary-variables from a list.*/
void
EnhancedLogging_SummaryGroup::add_variable(
  const std::vector<EnhancedLogging_SummaryVariable *> & var_list,
  unsigned int ix_lo,
  unsigned int ix_hi)
{
  // Make sure thaat the indices are in the correct order
  if (ix_lo < ix_hi) {
    unsigned int scratch = ix_lo;
    ix_lo = ix_hi;
    ix_hi = scratch;
  }

  unsigned int max_ix = var_list.size()-1;
  if ( ix_hi > max_ix) {
    CMLMessage::error( __FILE__,__LINE__,
      "Provided limits on the list, [",ix_lo,", ",ix_hi,"]\nare beyond the"
      "limits of the specified list of variables, of size ",var_list.size(),
      "Adding the variables that are within limits.\n");
    ix_hi = max_ix;
  }

  for (unsigned int ix = ix_lo; ix <= ix_hi; ix++) {
    add_variable( var_list[ix]);
  }
}

/****************************************************************************
Method 3a: Clone and add a specific summary-variable. */
void
EnhancedLogging_SummaryGroup::add_variable(
  EnhancedLogging_SummaryVariable * var)
{
  if (var &&
      check_alias( var->name, var->alias)) {
    add_variable_internal( var->clone_variable());
  }
  // else ignore.
}
/****************************************************************************
Method 3b: Clone and add a specific instance from reference to templated
           instance -- see header file.*/


/****************************************************************************
Name: add_variable_internal (add_variable Method 4)
Purpose:
  Common wrap-up to all add_variable options.
  Check on initialization, and add new variable to the variables set.
****************************************************************************/
void
EnhancedLogging_SummaryGroup::add_variable_internal(
  EnhancedLogging_SummaryVariable * var)
{
  // If already initialized, be sure to initialize the new variable.
  if (initialized) {
    var->initialize( conditions.size());
  }
  variables.push_back(var);
}

/*****************************************************************************
Name: add_condition
Purpose:
  Adds access to a EnhancedLogging_SummaryCondition to the group.
  If the group is already initialized, re-initialize the group's
  summary-variables to add slot(s) to account for th enew condition(s).
*****************************************************************************/
void
EnhancedLogging_SummaryGroup::add_condition(
  EnhancedLogging_SummaryConditionBase & cond)
{
  /* Conditions are normally initialized with the group, and it is strongly
     recommended that a group be fully configured before being initialized
     because the logging-variables are sized according to how many logging-
     conditions are in the group at initialization. If we add a condition
     after initialization, we need to resize the logging-variables arrays
     and initialize the condition.*/
  conditions.emplace_back( cond,
                           conditions.size());
  if (initialized) {
    cond.initialize(); /* initialize() is polymorphic so works for all types
                          of conditions.*/
    size_t num_cond = conditions.size();
    for (EnhancedLogging_BaseVariable* var : variables) {
      var->initialize( num_cond);
    }
  }
}


/*****************************************************************************
Name: new_*_condition
Purpose:
  needed by the
  There are 4 options:
    1: Create a basic evaluation-condition that is always satisfied.
    2: Create an evaluation-condition that contains its own dedicated
       compound-event
    3: Create an evaluation-condition that contains a reference to an
       externally-instantiated compound-event.
    4: Create a Boolean evaluation-condition that contains a reference to an
       external Boolean flag, such as the event_triggered flag of an
       externally-instantiated compound-event.
Notes:
- Because events require non-trivial configuration, trying to create and
  configure a new event through method arguments is prohibiti
  It is recommended that events be constructed and configured independently
  and accessed from the group, even if they are unique to the group. This
  can be done as part of the group configuration ... it is the configuration
  of a new CompoundEvent created on-the-fly using "new" that is going to be
  difficult.
- Option #2: Useful if a group has a specific event unique to that group.
             Not well suited to dynamic-allocation of a group, the group
             really needs to be independently defined and compiled to
             easily configure the event.
- Option #3: Useful if a group is being  configured on-the-fly because it
             provides access to an independently configured event without
             needing to configure the event on-the-fly along with the group.
             Note that if multiple groups are using the same event, each group
             will cause the event to re-test every cycle unless the
             *_check_period values are set in the event.
- Option #4 Same advantages as #2, but the group WILL NOT have the event
            evaluate its conditions. Most useful when an event is being used
            multiple times within a group, or by multiple groups and it is
            known that the event will have been evaluated somewhere prior
            to this condition within the current cycle.
*****************************************************************************/
EnhancedLogging_SummaryConditionBase &
EnhancedLogging_SummaryGroup::new_base_condition()
{
  EnhancedLogging_SummaryConditionBase * cond =
                                new EnhancedLogging_SummaryConditionBase();
  add_condition(*cond);
  allocated_conditions.push_back( cond);
  return *cond;
}
/****************************************************************************/
/****************************************************************************/
EnhancedLogging_SummaryConditionEvent &
EnhancedLogging_SummaryGroup::new_event_condition()
{
  EnhancedLogging_SummaryConditionEvent * cond =
                          new EnhancedLogging_SummaryConditionEvent( dyn_time);
  add_condition(*cond);
  allocated_conditions.push_back( cond);
  return *cond;
}
/****************************************************************************/
EnhancedLogging_SummaryConditionEventRef &
EnhancedLogging_SummaryGroup::new_event_condition( CompoundEvent & event_)
{
  EnhancedLogging_SummaryConditionEventRef * cond =
                          new EnhancedLogging_SummaryConditionEventRef(event_);
  add_condition(*cond);
  allocated_conditions.push_back( cond);
  return *cond;
}
/****************************************************************************/
EnhancedLogging_SummaryConditionBool &
EnhancedLogging_SummaryGroup::new_bool_condition( bool & triggered)
{
  EnhancedLogging_SummaryConditionBool * cond =
                          new EnhancedLogging_SummaryConditionBool(triggered);
  add_condition(*cond);
  allocated_conditions.push_back( cond);
  return *cond;
}
