/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides the overall management of a set of logging-groups.
  The logging -groups are accessed via EnhancedLogging_BaseGroup pointers;
  EnhancedLogging_BaseGroup is an abstract class and these pointers address
  instances of either:
  - EnhancedLogging_SummaryGroup
  - EnhancedLogging_SerialGroup
  )

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES)
    (New implementation, with implied requirements drawn from
     DataCollect in GNC_PAR))
  )
***********************************************************************/

#include <algorithm> // find_if, any_of

#include "cml/models/utilities/env_utils/include/env_utils.h"

#include "../include/enhanced_logging_manager.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
EnhancedLogging_Manager::EnhancedLogging_Manager(
  const double & dyn_time_)
  :
  dyn_time( dyn_time_),
  all_groups(),
  active_groups(),
  allocated_groups(),
  allocated_variables(),
  external_variables(),
  hdf5_py_converter(),
  target_dir(),
  logging_format( EnhancedLogging_BaseGroup::CSV_FMT),
  consolidate_summary_group_data(false)
{
  hdf5_py_converter = std::string(getenv_or_exit("CML_HOME"))+\
            "/models/utilities/enhanced_logging/src/log_hdf5.py";
}

/*****************************************************************************
Destructor
*****************************************************************************/
EnhancedLogging_Manager::~EnhancedLogging_Manager()
{
  for (const EnhancedLogging_BaseGroup * group : allocated_groups) {
    delete group;
  }
}

/*****************************************************************************
Name: initialize
Purpose:
- Executed during simulation initialization, this will activate any
  subscribed groups, resulting in the preparation and opening of their
  associated data file.
- It provides a convenient single-stop shop to test initialization of
  the logging. If the initialization flag is true, then all groups have
  been initialized (or, at least, an attempt has been made to initialize
  all groups.)
*****************************************************************************/
void
EnhancedLogging_Manager::initialize()
{
  get_target_dir();
  for (EnhancedLogging_BaseGroup * group : all_groups) {
    group->create_individual_group_file = !(consolidate_summary_group_data);
    group->initialize(target_dir);
    if (group->is_active()) {
      add_to_active_list( *group);
    }
  }
  SubscriptionBase::initialize();
}

/*****************************************************************************
Name: activate
Purpose:
- Executed during model subscription, which will subscribe all groups pending
  a subscription, and run an update command.
*****************************************************************************/
void
EnhancedLogging_Manager::activate()
{
  // Active must be set prior to subscribe_group(); groups will only be
  // subscribed to if the manager is active.
  active = true;

  for (EnhancedLogging_BaseGroup * group : subscription_pending_groups) {
    subscribe_group( *group);
  }
}

/*****************************************************************************
Name: update
Purpose:
  Main executable; updates all active groups.
*****************************************************************************/
void
EnhancedLogging_Manager::update()
{
  if (!active) {return;}
  for (EnhancedLogging_BaseGroup * group : active_groups) {
    group->update();
  }
}

/*****************************************************************************
Name: shutdown
Purpose:
  Closes out the data logging files for summary-groups.
*****************************************************************************/
void
EnhancedLogging_Manager::shutdown()
{
  /* If consolidating the data into one file, ignore any group-specific
   * logging-format specifications and use the manager-level specification for
   * all summary-groups. Also use a common manager-level ofstream and ignore
   * the group-specific ofstream for summary-groups.*/
  if (consolidate_summary_group_data) {
    std::string filename = target_dir+"/log_manager_summary.csv";
    std::ofstream consolidated_file(filename);
    if (!consolidated_file.is_open()) {
      CMLMessage::error(__FILE__,__LINE__,
        "Could not open logging file: ", filename);
      return;
    }

    for (EnhancedLogging_BaseGroup* group : active_groups) {
      // send summary group data to the consolidated file
      if (group->is_summary()) {
        group->log_group( consolidated_file);
      }
      else { // else -- serial group, shutdown according to group rules.
        group->shutdown();
        if (group->logging_format == EnhancedLogging_BaseGroup::HDF5) {
          convert_to_hdf5( group->get_csv_file());
        }
      }
    }
    if (logging_format == EnhancedLogging_BaseGroup::HDF5) {
      convert_to_hdf5( target_dir+"/log_manager_summary.csv");
    }
  }

  // If not consolidating summary-group data into one file, just shut down the
  // groups individually.
  else {
    for (EnhancedLogging_BaseGroup* group : active_groups) {
      group->shutdown();
      if (group->logging_format == EnhancedLogging_BaseGroup::HDF5) {
        convert_to_hdf5( group->get_csv_file());
      }
    }
  }
}

/*****************************************************************************
Name: new_serial_group
Purpose:
  Create a new serial-recording group, returns a reference to that group.
Note:
  There is a lot of similarity between this method and
  new_summary_group(...), except for two pivotal lines:
    - the iterative call to new_serial_group
    - the use of "new" in allocating new memory for the desired
      group-type. This could be templatized but it is not worth the effort
      given the previous line as well.
*****************************************************************************/
EnhancedLogging_SerialGroup &
EnhancedLogging_Manager::new_serial_group( std::string name,
                                           bool auto_subscribe)
{
  /* Check for uniqueness of group-name. If the name already exists,
     iteratively try again by adding "_" to the name.*/
  if (std::any_of( all_groups.begin(), all_groups.end(),
                   [name] (EnhancedLogging_BaseGroup* group_) {
                     return (group_->name == name);}
                 )) {
    CMLMessage::error(__FILE__,__LINE__,
      "Cannot create a new group with name ",name," because a group\n"
      "already exists with that name.\n"
      "Modifying name to ",name+"_","\n");

    /* Iteratively call this method until enough underscores have been added
    * to make it unique. At that point, that final call will create and
    * configure a new group and return it back down the iteration chain.*/
    return new_serial_group( name+"_",
                             auto_subscribe);
  }

  /* else:
     no matching name found, continue to create a group with the specified name.
  */
  EnhancedLogging_SerialGroup * new_group =
                            new EnhancedLogging_SerialGroup( name,
                                                             dyn_time);
  all_groups.push_back( new_group);
  allocated_groups.push_back( new_group);
  new_group->set_target_dir( target_dir);

  if (auto_subscribe) { new_group->subscribe(); }
  if (initialized)    { new_group->initialize(target_dir);}
  return *new_group;
}
/*****************************************************************************
Name: new_summary_group
Purpose:
  Create a new summary-recording group, returns a reference to that group.
Note:
  There is a lot of similarity between this method and
  new_serial_group(...), except for two pivotal lines:
    - the iterative call to new_summary_group
    - the use of "new" in allocating new memory for the desired
      group-type. This could be templatized but it is not worth the effort
      given the previous line as well.
*****************************************************************************/
EnhancedLogging_SummaryGroup &
EnhancedLogging_Manager::new_summary_group( std::string name,
                                            bool auto_subscribe)
{
  /* Check for uniqueness of group-name. If the name already exists,
     iteratively try again by adding "_" to the name.*/
  if (std::any_of( all_groups.begin(), all_groups.end(),
                   [name] (EnhancedLogging_BaseGroup* group_) {
                     return (group_->name == name);}
                 )) {
    CMLMessage::error(__FILE__,__LINE__,
      "Cannot create a new group with name ",name," because a group\n"
      "already exists with that name.\n"
      "Modifying name to ",name+"_","\n");

    /* Iteratively call this method until enough underscores have been added
    * to make it unique. At that point, that final call will create and
    * configure a new group and return it back down the iteration chain.*/
    return new_summary_group( name+"_",
                              auto_subscribe);
  }

  /* else:
     no matching name found, continue to create a group with the specified name.
  */
  EnhancedLogging_SummaryGroup * new_group =
                           new EnhancedLogging_SummaryGroup( name,
                                                             dyn_time);
  all_groups.push_back( new_group);
  allocated_groups.push_back( new_group);
  new_group->set_target_dir( target_dir);

  if (auto_subscribe) { new_group->subscribe(); }
  if (initialized)    { new_group->initialize( target_dir);}
  return *new_group;
}

/*****************************************************************************
Name: subscribe_group
Purpose:
  Enables a group
*****************************************************************************/
void
EnhancedLogging_Manager::subscribe_group( std::string name)
{
  EnhancedLogging_BaseGroup * group_ptr = get_group(name);
  if (group_ptr == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "Manager has no registered data-record group with name ",name,".\n"
      "Cannot subscribe to this group.\n");
    return;
  }
  subscribe_group(*group_ptr);
}
/****************************************************************************/
void
EnhancedLogging_Manager::subscribe_group()
{
  EnhancedLogging_BaseGroup * group_ptr = get_last_group();
  if (group_ptr == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "Manager has no data-record groups registered.\n"
      "Cannot subscribe to a group.\n");
    return;
  }
  subscribe_group(*group_ptr);
}
/****************************************************************************/
void
EnhancedLogging_Manager::subscribe_group( EnhancedLogging_BaseGroup & group)
{
  // Check to see if the manager is active, rather than initialized, to avoid group.activate()
  // calls from group.subscribe() that would be pointless if the manager is never activated.
  // subscription_pending_groups can hold the group reference until the manager is activated.
  if (active) {
    // Going to subscribe to the group, which will activate it if it was not already active.
    // Query whether the group is already active before losing that information.
    bool is_active = group.is_active();
    group.subscribe();
    // If the group was not active, and now is, add it to the list of active
    // groups. If it was already active, it is already in the list. If it still
    // isn't active, it doesn't go in the list.
    if (!is_active && group.is_active()) {
      add_to_active_list( group);
    }
  }
  else {subscription_pending_groups.push_back(&group);}

}

/*****************************************************************************
Name: unsubscribe_group
Purpose:
  Unsubscribes from a group, potentially deactivating it.
*****************************************************************************/
void
EnhancedLogging_Manager::unsubscribe_group( std::string name)
{
  for (auto it = active_groups.begin(); it!= active_groups.end(); ++it) {
    if ( (*it)->name == name) {
      (*it)->unsubscribe();
      /* If the unsubsribe made the group inactive, remove it from the
       * active-groups. Then quit, regardless of whether the group is still
       * active -- the unsubscribe has been applied.*/
      if (!(*it)->is_active()) { active_groups.erase(it); }
      return;
    }
  }
  /* No return; encountered means that no group was found with the specified
   * name.*/
  CMLMessage::inform(__FILE__,__LINE__,
    "Manager has no active data-record group with name ",name,".\n"
    "Cannot unsubscribe from this group.\n");
}

/*****************************************************************************
Name: add_to_active_list
Purpose:
  Checks for presence on the list, and adds a new entry if it doesn't
  already exist.
*****************************************************************************/
void
EnhancedLogging_Manager::add_to_active_list(
  EnhancedLogging_BaseGroup& new_group)
{
  auto it = std::find( active_groups.begin(),
                       active_groups.end(),
                       &new_group);
  // If specified group is not already in the list, add it
  if (it == active_groups.end()) {
    active_groups.push_back(&new_group);
  }
  // else, it is already in the list, can silently skip adding it again.
}

/*****************************************************************************
Name: get_group
Purpose:
  Identifies a group by name and returns a pointer to that group.
Note:
  Returning a pointer rather than a reference because of the potential for a
  failed search -- i.e. no group is found with that name.
*****************************************************************************/
EnhancedLogging_BaseGroup *
EnhancedLogging_Manager::get_group( std::string name)
{
  auto it = std::find_if( all_groups.begin(), all_groups.end(),
                          [name](EnhancedLogging_BaseGroup * group_) {
                            return (group_->name == name); }
                        );
  if (it == all_groups.end()) {
    CMLMessage::error(__FILE__,__LINE__,
      "Manager has no registered data-record group with name ",name,".\n"
      "Returning NULL.\n");
    return nullptr;
  }
  return (*it);
}

/*****************************************************************************
Name: get_sumary_group
Purpose:
  Identifies a summary group by name and returns a pointer to that group.
Note:
  Returning a pointer rather than a reference because of the potential for a
  failed search -- i.e. no summary group is found with that name.
*****************************************************************************/
EnhancedLogging_SummaryGroup *
EnhancedLogging_Manager::get_summary_group( std::string name)
{
  EnhancedLogging_SummaryGroup * group = dynamic_cast<EnhancedLogging_SummaryGroup *>(EnhancedLogging_Manager::get_group(name));
  if (group == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "Group ",name, "is not an EnhancedLogging_SummaryGroup.\n"
      "Returning NULL.\n");
  }
  return group;
}

/*****************************************************************************
Name: get_group
Purpose:
  Identifies a group by name and returns a pointer to that group.
Note:
  Returning a pointer rather than a reference because of the potential for a
  failed search -- i.e. no group is found with that name.
*****************************************************************************/
EnhancedLogging_SerialGroup *
EnhancedLogging_Manager::get_serial_group( std::string name)
{
  EnhancedLogging_SerialGroup * group = dynamic_cast<EnhancedLogging_SerialGroup *>(EnhancedLogging_Manager::get_group(name));
  if (group == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "Group ",name, "is not an EnhancedLogging_SerialGroup.\n"
      "Returning NULL.\n");
  }
  return group;
}

/*****************************************************************************
Name: add_group
Purpose:
  Adds an externally-instantiated group to the set of groups
*****************************************************************************/
void
EnhancedLogging_Manager::add_group(EnhancedLogging_BaseGroup & new_group)
{
  // Check for duplication:
  for ( EnhancedLogging_BaseGroup * group: all_groups) {
    if (group == &new_group) {
      // already registered, skip.
      return;
    }
    if (group->name == new_group.name) {
      CMLMessage::error(__FILE__,__LINE__,
        "Cannot add a group with name ",group->name," because a group\n"
        "already exists with that name.\n"
        "Manager uses \"name\" to identify groupd and cannot manage "
        "duplicated names.\n"
        "Group has not been added.\n");
      return;
    }
  }
  all_groups.push_back(&new_group);
  new_group.set_target_dir( target_dir);
  if (new_group.is_active()) { active_groups.push_back(&new_group);}
  if (initialized) {new_group.initialize( target_dir);}
}

/*****************************************************************************
Name: get_last_group
Purpose:
  Returns a pointer to the last group in the all_groups list.
*****************************************************************************/
EnhancedLogging_BaseGroup *
EnhancedLogging_Manager::get_last_group()
{
  if (all_groups.empty()) {return nullptr;}
  return all_groups.back();
}

/*****************************************************************************
Name: log_all
Purpose:
  Forces an update on the all active serial groups and logs their content.
  Has no effect on summary groups.
Notes:
- The update method is still subject to the "next time" constraint
- Some group settings result in logging from the update method, but the
  logging method checks the time of last log, so we still don't get
  duplicate logs.
- For summary-groups this method will cause a dump of the current
  summary values (e.g. average values). The processing (e.g. averaging)
  will then continue without resetting.
*****************************************************************************/
void
EnhancedLogging_Manager::log_all()
{
  for (EnhancedLogging_BaseGroup * group : active_groups) {
    if (!group->is_summary()) {
      group->update();
      group->log_group();
    }
  }
}

/*****************************************************************************
Name: get_variable
Purpose:
  Search through the allocated_variables and external_variables lists for the
  occurrence with matching name.
*****************************************************************************/
EnhancedLogging_Variable *
EnhancedLogging_Manager::get_variable(
  const std::string & var_name,
  const std::string & var_alias)

{
  auto it =
     std::find_if( allocated_variables.begin(), allocated_variables.end(),
                   [var_name, var_alias] (EnhancedLogging_Variable& var_) {
                     return (var_.name == var_name &&
                             var_.alias == var_alias); }
                 );
  if (it != allocated_variables.end()) {
    return &(*it);
  }

  // If not found in allocated-variabels, search in external-variables.
  auto it1 =
      std::find_if( external_variables.begin(), external_variables.end(),
                    [var_name, var_alias] (EnhancedLogging_Variable* var_) {
                      return ((var_->name  == var_name) &&
                              (var_->alias == var_alias)); }
                  );
  if (it1 != external_variables.end()) {
    return (*it1);
  }
  return nullptr;
}


/*****************************************************************************
Name: add_variable
Purpose:
  Add a proto-logging-variable to the manager for use by one or more of its
  groups.
Notes:
- If not group is specified, this method does not add a logging-variable
  to any groups, it does not even create the logging-variable for adding
  to a group, it only deals with the proto-logging-variables that may be used
  later to create associated logging-variables and adding them to a group.
- A proto-logging-variable can represent a single value or an array of values.
- Several methods available:
  1: Create a new EnhancedLogging_Variable proto-logging-variable using Trick
     ref-attributes; this requires the variable name and (optionally) its
     alias and intended units.
  2: Make available to the manager one or more pre-existing
     proto-logging-variable instances.
     The purpose of having this list is to provide a searchable database of
     pre-existing instances to avoid duplication when using option #1.
     a. Add a single proto-logging-variable.
     b. Add all of the contents of a list of proto-logging-variables.
     c. Add all of the contents of a proto-logging-variable-set.
  3: Similar to #1, and also create all associated logging-variables and add
     these to the specified group.
  4: Similar to #2, and also create all associated logging-variables and add
     these to the specified group.
     - Adding the proto-logging-variable to the manager's database may be
       unnecessary as part of this action, but building a comprehensive
       database ultimately saves time by avoiding the unnecessary creation
       of a new proto-logging-variable when an existing one can be easily
       identified and used.
  5: Add a subset of the logging-variables identified by an
     EnhancedLogging_Variable representing an array of variables.
*****************************************************************************
Method 1: using variable name and Trick ref-attributes (single value).*/
void
EnhancedLogging_Manager::add_variable( std::string name,
                                       std::string alias,
                                       std::string units)
{
  /* First, check that such an instance is not already available to the manager.
   * If it is, do nothing.*/
  if (!get_variable(name, alias)) {
    allocated_variables.emplace_back( EnhancedLogging_Variable( name,
                                                                alias,
                                                                units));
  }
}
/****************************************************************************
Method 2a: Just add a pointer to the provided instance*/
void
EnhancedLogging_Manager::add_variable( EnhancedLogging_Variable & var)
{
  if (!get_variable(var.name, var.alias)) {
    external_variables.push_back( &var);
  }
}
/****************************************************************************
Method 2b: Add pointers to each member of the the provided list*/
void
EnhancedLogging_Manager::add_variable(
  std::list< EnhancedLogging_Variable> & list_)
{
  for (EnhancedLogging_Variable & var : list_) {
    if (!get_variable(var.name, var.alias)) {
      external_variables.push_back( &var);
    }
  }
}
/****************************************************************************
Method 2c: Add pointers to each member of the the provided set*/
void
EnhancedLogging_Manager::add_variable(
  EnhancedLogging_VariableSet & set_)
{
  for (EnhancedLogging_Variable & var : set_.var_list) {
    if (!get_variable(var.name, var.alias)) {
      external_variables.push_back( &var);
    }
  }
}
/****************************************************************************
Method 3: Create the new proto-logging-variable and add contents to the
          specified group.*/
void
EnhancedLogging_Manager::add_variable(
  EnhancedLogging_BaseGroup & group,
  std::string name,
  std::string alias,
  std::string units)
{
  EnhancedLogging_Variable * var_ptr = get_variable(name, alias);
  if (var_ptr) {
    group.add_variable( *var_ptr);
  }
  else {
    allocated_variables.emplace_back( EnhancedLogging_Variable( name,
                                                                alias,
                                                                units));
    group.add_variable( allocated_variables.back());
  }
}
/****************************************************************************
Method 4a: Add the logging-variables associated with the specified
           proto-logging-variable to the specified group.*/
void
EnhancedLogging_Manager::add_variable(
  EnhancedLogging_BaseGroup & group,
  EnhancedLogging_Variable & var)
{
  add_variable(var);
  group.add_variable(var);
}
/****************************************************************************
Method 4b: Add the logging-variables associated with each of the
           proto-logging-variables in the specified lis to the specified
           group.*/
void
EnhancedLogging_Manager::add_variable(
  EnhancedLogging_BaseGroup & group,
  std::list<EnhancedLogging_Variable> & var_list)
{
  add_variable(var_list);
  group.add_variable(var_list);
}
/****************************************************************************
Method 4c: Add the logging-variables associated with each of the
           proto-logging-variables in the specified set to the specified
           group.*/
void
EnhancedLogging_Manager::add_variable(
  EnhancedLogging_BaseGroup & group,
  EnhancedLogging_VariableSet & var_set)
{
  add_variable(var_set);
  group.add_variable(var_set);
}
/****************************************************************************
Method 5: Add a subset of the logging-variables associated with the specified
          proto-logging-variable to the specified group.*/
void
EnhancedLogging_Manager::add_variable(
  EnhancedLogging_BaseGroup & group,
  EnhancedLogging_Variable & var,
  unsigned int ix_lo,
  unsigned int ix_hi)
{
  add_variable(var);
  group.add_variable(var, ix_lo, ix_hi);
}

// Access the Trick Global CommandLineArguments
extern Trick::CommandLineArguments *the_cmd_args;
/*****************************************************************************
Name: get_target_dir
Purpose:
  Populates the target_dir string.
*****************************************************************************/
void
EnhancedLogging_Manager::get_target_dir()
{
  target_dir = the_cmd_args->get_output_dir();
  if (target_dir.empty()) {
    CMLMessage::error( __FILE__,__LINE__,
      "Error in getting intended directory for writing data files.\n"
      "Placing data files where SIM was executed from.\n");
  }
}


/*****************************************************************************
Name: convert_to_hdf5
Purpose:
  Converts a csv file to HDF5 format
FIXME Turner 2024/06
      For now, this uses a Python script because that was easier to implement.
      This should be converted to C++ at some point, at which point it should
      be feasible to log directly to HDF5, and this method to convert from
      CSV to HDF5 would be moot.
*****************************************************************************/
void
EnhancedLogging_Manager::convert_to_hdf5( const std::string & filename)
{
  std::string command = "python " + hdf5_py_converter + " " + filename;
  system(command.c_str());
}
