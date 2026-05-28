/*******************************TRICK HEADER******************************
PURPOSE: (Provides a grouping of variables to be used for serial-logging)


PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/

#include "../include/serial_logging_group.hh"
#include <iomanip> // setw
#include "cml/models/utilities/math_utils/include/math_utils.hh"
/*****************************************************************************
Constructor
*****************************************************************************/
EnhancedLogging_SerialGroup::EnhancedLogging_SerialGroup(
  std::string name_,
  const double & dyn_time_)
  :
  EnhancedLogging_BaseGroup(name_, dyn_time_),
  t_last_log(0.0),
  log_spec(ALWAYS),
  record_trigger_name(false),
  DISCRETE_condition(""),
  DISCRETE_active(false),
  file_initialized(false),
  log_at_deactivate(false)
{
  subscribe_name = "EnhancedLogging_SerialGroup:"+name;

  /* Set the event to record trigger-names. Even if we are not recording
     trigger-names, they are essential for correct operation with
     logging-spec=DISCRETE and I don't have a good way to switch this on
     for that logging-spec except by switching this on at construction.
     TODO Turner 2024/06
          Investigate whether this necessary or whether there is a better
          way to switch this flag on only for logging-spec=DISCRETE
   */
  event.action_triggers.record_trigger_name = true;
}

/*****************************************************************************
Destructor
*****************************************************************************/
EnhancedLogging_SerialGroup::~EnhancedLogging_SerialGroup()
{
  for ( auto var : allocated_variables) {
    delete var;
  }
}


/*****************************************************************************
Name: initialize
Purpose:
  For the most part, initialization simply accesses the base-class
  EnhancedLogging_BaseGroup::initialize(), with one edge case.
  If the data type is SINGLE and there are subscriptions pending,
  SubscriptionBase::intiialize() will lead to activation, which leads to an
  immediate update / processing of the available data, and immediate
  deactivation. producing an error because SubscriptionBase believes that the
  derived activation sequence that it tried failed to complete.
*****************************************************************************/
void
EnhancedLogging_SerialGroup::initialize( const std::string & target_dir_)
{
  if (log_spec == SINGLE && sub_pending > 0) {
    CMLMessage::warn( __FILE__,__LINE__,
      "Activating a data-record-group (",name,") at its initialization.\n"
      "Type is set to SINGLE, so group will record the data at\n"
      "initialization and immediately deactivate.\n"
      "This can lead to unexpected results because target data may not "
      "be fully populated at sim-start.\n"
      "A SubscriptionBase error message will follow and can be ignored.\n");
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
EnhancedLogging_SerialGroup::update()
{
  if (!EnhancedLogging_BaseGroup::update_base()) {return;}

  switch(log_spec) {
  case ALWAYS:
    log_group();
    break;
  case EVENT:
    event.update_no_manager();
    if (event.event_triggered) {log_group();}
    break;
  case DISCRETE:
    event.update_no_manager();
    /* Log the group if the event is triggered and either:
       - it was previously untriggered (i.e. DISCRETE_active is false), OR
       - it was previously triggered and has now been triggered by a
         different trigger
    */
    if ( event.event_triggered &&
         ( !DISCRETE_active ||
           (DISCRETE_condition != event.action_triggers.get_trigger_name()))){
      log_group();
    }
    DISCRETE_active = event.event_triggered;
    DISCRETE_condition = event.action_triggers.get_trigger_name();
    break;
  case SINGLE:
    log_group();
    active = false;
    break;
  default:
    CMLMessage::error( __FILE__,__LINE__,
      "Invalid logging specification:",log_spec);
    break;
  }
}


/*****************************************************************************
Name: shutdown
Purpose:
  Shuts down the serial-group
*****************************************************************************/
void
EnhancedLogging_SerialGroup::shutdown()
{
  deactivate(); // adds a log if flag log_at_deactivate is set
  if (file_out.is_open()) {
    file_out.close();
  }
}

/*****************************************************************************
Name: log_group_csv
Purpose:
  Simply directs the output ofstream content to a csv file.
*****************************************************************************/
void
EnhancedLogging_SerialGroup::log_group_csv(std::ofstream & fstream)
{
  if (MathUtils::is_near_equal( t_last_log, dyn_time)) {return;}
  fstream << dyn_time;
  for (EnhancedLogging_BaseVariable * var : variables) {
    var->log_value( fstream);
  }
  /* At the end of the line, record which trigger caused the data dump if
     instructed to do so with record_trigger_name.*/
  if ( record_trigger_name) {
    fstream << ", "<< event.action_triggers.get_trigger_name();
  }
  // Add a newline character and flush the buffer with std::endl.
  fstream << std::endl;
}

/*****************************************************************************
Name: log_group_csv_fmt
Purpose:
  Directs the output ofstream content to a cosnsistently-formatted csv file.
*****************************************************************************/
void
EnhancedLogging_SerialGroup::log_group_csv_fmt(std::ofstream & fstream)
{
  if (MathUtils::is_near_equal( t_last_log, dyn_time)) {return;}
  fstream << std::setw(8) << dyn_time;
  for (EnhancedLogging_BaseVariable * var : variables) {
    var->log_value_fmt( fstream);
  }
  // At the end of the line, record which trigger caused the data dump if
  // instructed to do so with record_trigger_name.
  if ( record_trigger_name) {
    fstream << ", "<< event.action_triggers.get_trigger_name();
  }
  // Add a newline character and flush the buffer with std::endl.
  fstream << std::endl;
}

/*****************************************************************************
Name: deactivate
Purpose:
  Switches the group off; forces output of a log set if the log_at_deactivate
  flag is set
*****************************************************************************/
void
EnhancedLogging_SerialGroup::deactivate()
{
  if (!active) {return;}
  if (log_at_deactivate) {
    log_group();
  }
  active = false;
}


/*****************************************************************************
Name: initialize_file
Purpose:
- Writes the header line
Notes: File is opened with group activation, and closed with shutdown
*****************************************************************************/
void
EnhancedLogging_SerialGroup::initialize_file()
{
  file_out << "dyn_time{s}";
  for (EnhancedLogging_BaseVariable * var : variables) {
    file_out << ", " << var->alias << "{" << var->units << "}";
  }
  file_out << std::endl;
}

/*****************************************************************************
Name: set_record_trigger_name
Purpose:
  Sets the record_trigger_name protected variable subject to checks
*****************************************************************************/
void
EnhancedLogging_SerialGroup::set_record_trigger_name(bool record)
{
  if (!record) {
    record_trigger_name = false;
  }
  else if (   (!event.action_triggers.require_all)
           && (( log_spec == DISCRETE) || (log_spec == EVENT))) {
    record_trigger_name = true;
  }
  else if ( event.action_triggers.require_all) {
    CMLMessage::warn( __FILE__,__LINE__,
      "The triggering-name is not recorded when the triggering-event\n"
      "requires all triggers; it has no meaning.\n");
    record_trigger_name = false;
  }
  else {
    CMLMessage::warn( __FILE__,__LINE__,
      "The triggering-name is not recorded when the logging is not\n"
      "conditional on the triggering-event; it has no meaning.\n");
    record_trigger_name = false;
  }
}

/*****************************************************************************
Name: set_log_spec
Purpose: Sets the LoggingSpecification type and makes internal checks/settings
*****************************************************************************/
void
EnhancedLogging_SerialGroup::set_log_spec( LoggingSpecification log_spec_)
{
  log_spec = log_spec_;
  if ( (log_spec == ALWAYS) || (log_spec == SINGLE)) {
    record_trigger_name = false;
  }
}

/*****************************************************************************
Name: add_variable
Purpose:
  Add one or more serial-variables to the variables list
Notes:
- We have several methods for adding a variable to this group:
  1: Using one or more proto-logging-variables (EnhancedLogging_Variable),
     the associated EnhancedLogging_SerialVariableT<T> instances are passed
     (by base-class pointer of type EnhancedLogging_SerialVariable) to
     stage 2. The associated EnhancedLogging_SerialVariableT<T> instances
     will be created if they do not already exist for the specified
     EnhancedLogging_Variable.
     An option exists to select a contiguous subset of the serial-variables
     associated with the provided variable.
  2: A set of instances of EnhancedLogging_SerialVariableT<T>, accessed by
     generic base-class pointer EnhancedLogging_SerialVariable
     is broken out and the individual instances passed to step 3.
       (e.g. the set of instances found within each instance of
             EnhancedLogging_Variable from step 1)
     An option exists to select a contiguous subset of the serial-variables
     from within the provided list.
  3: A pre-existing instance of EnhancedLogging_SerialVariableT<T> is added
     to this group using its base-class EnhancedLogging_SerialVariable
     pointer.
  4: A common wrap-up method accessed from either step 3a or 3b.
     A check is made here on the state of the output file; when the file is
     initialized, the aliases of the variables are output to the file on the
     first line. If this has already been done, it is not safe to be adding
     new variables to this group and the add_variable process exits
     Otherwise, the specified instance is added to the group's list of
     variables and initialized if the group has already been initialized.
     This step is provided by method add_variable_internal, which is a
     protected method and only called from steps 3a or 3b
*****************************************************************************/
/****************************************************************************
Method 1a: Add the serial-variables associated with an
           EnhancedLogging_Variable instance.*/
void
EnhancedLogging_SerialGroup::add_variable(
  EnhancedLogging_Variable & var)
{
  add_variable( var.get_serial_variable_list());
}
/****************************************************************************
Method 1b: Add the serial-variables associated with each of a list of
           EnhancedLogging_Variable instances. This capability is provided in
           EnhancedLogging_BaseGroup.*/

/****************************************************************************
Method 1c: Add the logging variables associated with each of a set of
           EnhancedLogging_Variable instances.*/
void
EnhancedLogging_SerialGroup::add_variable(
  EnhancedLogging_VariableSet & set)
{
  add_variable(set.get_serial_variable_list());
}

/****************************************************************************
Method 1d: Add a subset of the serial-variables associated with an
           EnhancedLogging_Variable instance.*/
void
EnhancedLogging_SerialGroup::add_variable(
  EnhancedLogging_Variable & var,
  unsigned int ix_lo,
  unsigned int ix_hi)
{
  add_variable( var.get_serial_variable_list(),
                ix_lo,
                ix_hi);
}

/****************************************************************************
Method 2a: Add all serial-variables from a list.*/
void
EnhancedLogging_SerialGroup::add_variable(
  const std::vector<EnhancedLogging_SerialVariable *> & var_set)
{
  for (EnhancedLogging_SerialVariable * var : var_set) {
    add_variable( var);
  }
}
/****************************************************************************
Method 2b: Add a subset of serial-variables from a list.*/
void
EnhancedLogging_SerialGroup::add_variable(
  const std::vector<EnhancedLogging_SerialVariable *> & var_list,
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
Method 3: Add a specific serial-variable*/
void
EnhancedLogging_SerialGroup::add_variable(
  EnhancedLogging_SerialVariable * var)
{
  if (var &&
      check_alias( var->name, var->alias)) {
    add_variable_internal( var);
  }
  // else ignore.
}

/****************************************************************************
Name: add_variable_internal (add_variable Method 4)
Purpose:
  Common wrap-up to all add_variable options.
  Check on initialization, and add new variable to the variables set.
****************************************************************************/
void
EnhancedLogging_SerialGroup::add_variable_internal(
  EnhancedLogging_SerialVariable * var)
{
  if (file_initialized) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot add a variable to a group after the data-recording file "
      "has been opened.\n"
      "Group ",name," remains unaltered.\n");
    return;
  }

  // If already initialized, be sure to initialize the new variable.
  if (initialized) {
    var->initialize();
  }
  variables.push_back(var);
}
