/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides a staging-ground for later creation of seria-variables and
  summary-variables. Provides the interface to the Trick reference-attributes
  mechanism for identifying variables and theiorr attributes just by name.
  Note -- This is a staging ground for logging-variables,
          IT IS NOT a base-class of logging-variables.
          It is referred to in the model documentation as a
          proto-logging-variable.)

LIBRARY DEPENDENCY:
   ((../src/enhanced_logging_variable.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/
#ifndef CML_ENHANCED_LOGGING_VARIABLE_HH
#define CML_ENHANCED_LOGGING_VARIABLE_HH

#include <string>
#include <list>
#include <vector>

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include "summary_logging_variable.hh"
#include "serial_logging_variable.hh"

#include "trick/memorymanager_c_intf.h"

/*****************************************************************************
EnhancedLogging_Variable
Purpose:
  Provides a configuration / staging point for creation of either or both:
  - EnhancedLogging_SerialVariable instance(s)
  - EnhancedLogging_SummaryVariable instance(s).
  Contains the Trick interface logistics to facilitate identification of
  variable charactersitics from basic string variable-name.
Notes:
  This class does not sit in the inheritance path of either of the two target
  variable types; its purpose is to provide an easy-to-use capability to
  instantiate a staging-point for a logging-variable, and the mechanism by
  which that easy-to-use configuration-tool can be converted into an actual
  logging variable.
  When a EnhancedLogging_Variable instance is used to identify a C-style
  array, it can be responsible for spawning multiple
  EnhancedLogging_S*Variable instances. An EnhancedLogging_Variable instance
  may represent an array, but each EnhancedLogging_S*Variable instance
  represents a single data value.
*****************************************************************************/
class EnhancedLogging_Variable
{
 public:
  std::string name; /* (--)
    Name of the variable being logged;
    this is the name as it appears in the simulation.*/
  std::string alias; /* (--)
    Alias of the variable being logged, this is the name that will be
    associated with the data in the output file.*/
  std::string units; /* (--)
    Units associated with the variable being logged.*/
 protected:
  std::string type; /* (--)
    String representation of the data type associated with the variable
    identified  by "name".*/
  void * var_ptr; /* (--)
    Trick ref-attributes returns a void pointer to the address of the
    sim-variable identified by "name". This will later be converted into a typed
    pointer using the string "type", also provided by ref_attributes.*/
  std::vector<unsigned int> array_size; /* (--)
    When the variable "name" refers to an array, this list of integers
    identifies the size of the array. The number of entries identifies the
    dimensionality of the array, and the values within it identify the number
    of entries in each dimension.
    Note -- this recognizes the expansion of the array *beyond* that point to
    which it is specified by "name". It does not represent the entire array
    size if "name" already restricts to a sub-array. */
  std::vector<EnhancedLogging_SerialVariable*> serial_variable_list; /* (--)
    List of pointers to the variables generated from the sim-variable-name.
    The sim-variable identified by "name" can be processed into either
    (or both) summary-variables and/or serial-variables; a name may be the
    name of an array, so potentially expands into multiple single variables.
    This is the set of the serial-variables associated with "name".
    Note: the actual instances will be of the templated class
    EnhancedLogging_SerialVariableT; we store pointers to these instances
    rather than the instances themselves for convenience because the pointers
    to the common base-class can be held within one container while storing the
    instances would require a container for each template-specialization.*/
  std::vector<EnhancedLogging_SummaryVariable*> summary_variable_list; /* (--)
    List of pointers to the variables generated from the sim-variable-name.
    The sim-variable identified by "name" can be processed into either
    (or both) summary-variables and/or serial-variables; "name" may identify
    an array, so it potentially expands into multiple single variables.
    This is the set of the summary-variables associated with "name".
    Note: the actual instances will be of the templated class
    EnhancedLogging_SummaryVariableT; we store pointers to these instances
    rather than the instances themselves for convenience because the pointers
    to the common base-class can be held within one container while storing the
    instances would require a container for each template-specialization.*/
    /* TODO Turner 2024/03
       Both lists should contain unique-pointers, but Trick cannot handle
       containers of unique-pointers, so they have to be raw pointers,
       protected, and deleted at destruction of the class. If Trick advances
       to the point of supporting unique-pointers, this can be changed.*/
  bool trick_processed; /* (--)
    Simple flag indicating whether the variable specifications have been
    processed through Trick's ref-attributes.*/
  bool serial_list_processed; /* (--)
    Simple flag indicating whether the variable specifications have been
    processed into the serial-variable list. */
  bool summary_list_processed; /* (--)
    Simple flag indicating whether the variable specifications have been
    processed into the summary-variable list. */

 public:
  EnhancedLogging_Variable( std::string name,
                            std::string alias = "",
                            std::string units = "");
  virtual ~EnhancedLogging_Variable();

  std::vector<EnhancedLogging_SerialVariable*> & get_serial_variable_list();
  std::vector<EnhancedLogging_SummaryVariable*> & get_summary_variable_list();
 private:
  void generate_serial_variables();
  void generate_summary_variables();
  void process_ref_attributes();
};

/*****************************************************************************
EnhancedLogging_BaseVariable_Set
Purpose:
  Provides the infrastructore for grouping together a set of
  EnhancedLogging_Variable instances and batch-processing for creating
  logging-variables from the entire set of EnhancedLogging_Variable instances.
*****************************************************************************/
class EnhancedLogging_VariableSet
{
 public:
  std::list<EnhancedLogging_Variable> var_list; /* (--)
    List of the name-alias combinations, unprocessed raw strings.*/
 protected:
  std::vector<EnhancedLogging_SerialVariable*> serial_variable_list; /* (--)
    List of pointers to the variables generated from the members of
    the var_list, and to external instances added to the list.
    The config-list can be processed into either (or both) summary-variables
    and/or serial-variables. This is the set of serial-variables associated with
    all of the variables found in var_list.
    Note: the actual instances will be of the templated class
    EnhancedLogging_SerialVariableT; we store pointers to these instances
    rather than the instances themselves for convenience because the pointers
    to the common base-class can be held within one container while storing the
    instances would require a container for each template-specialization.*/
  std::vector<EnhancedLogging_SummaryVariable*> summary_variable_list; /* (--)
    List of pointers to the variables generated from the members of
    the config_list, and to external instances added to the list.
    The config-list can be processed into either (or both) summary
    variables and serial-variables. This is the set of summary-variables
    associated with all of the variables found in var_list.
    Note: the actual instances will be of the templated class
    EnhancedLogging_SummaryVariableT; we store pointers to these instances
    rather than the instances themselves for convenience because the pointers
    to the common base-class can be held within one container while storing the
    instances would require a container for each template-specialization.*/
  bool summary_list_processed; /* (--)
    Simple flag indicating whether the config list has been processed into
    the summary-variable list. */
  bool serial_list_processed; /* (--)
    Simple flag indicating whether the config list has been processed into
    the serial-variable list. */

 public:
  EnhancedLogging_VariableSet();
  explicit EnhancedLogging_VariableSet(
                                   const std::list<EnhancedLogging_Variable> &);
  /* Note -- the serial_variable_list and summary_variable_list vectors are
     amalgamations of the vectors from the individual EnhancedLogging_Variable
     instances; the EnhancedLogging_Variable are responsible for creating
     and destroying these instances.*/
  virtual ~EnhancedLogging_VariableSet(){};

  void add_variable(EnhancedLogging_SerialVariable & var);
  void add_variable(EnhancedLogging_SummaryVariable & var);

  std::vector<EnhancedLogging_BaseVariable*> & get_serial_variable_list();
  std::vector<EnhancedLogging_SummaryVariable*> & get_summary_variable_list();
};
#endif
