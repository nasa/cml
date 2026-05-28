/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides the grouping of variables for serial-logging; the values of
  variables in this group will be written out to an output file, with
  each line of the output data representing a snapshoit of data across all
  variables within this group.)

LIBRARY DEPENDENCY:
   ((../src/serial_logging_group.cc)
   )

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/
#ifndef CML_ENHANCED_LOGGING_SERIAL_GROUP_HH
#define CML_ENHANCED_LOGGING_SERIAL_GROUP_HH

#include <fstream> // ofstream

/*TODO Turner 2024/06
       Finish HDF5 formatting
#include "hdf5.h"
*/

#include "enhanced_logging_variable.hh"
#include "base_logging_group.hh"
#include "serial_logging_variable.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
EnhancedLogging_SerialGroup
Purpose:
  A EnhancedLogging_SerialGroup manages a set of variables, typically
  recording a series of snapshots of the values of the whole group at a set
  of timestamps.
  A group may be activated or deactivated in its entirety.
*****************************************************************************/
class EnhancedLogging_SerialGroup : public EnhancedLogging_BaseGroup
{
 protected:
  double t_last_log; /* (s)
    A record of the last time this group was logged.
    Used to prevent logs with duplicate time-stamps.*/

 public:
  enum LoggingSpecification {
   ALWAYS,  /* Log all variables every cycle the group is active and the
               time-interval constraint is satisfied.*/
   EVENT,   /* Log all variables if the group is active, the time-interval
               constraint is satisfied, and the event is satisfied.*/
   DISCRETE,/* Log all variables if the group is active, the time-interval
               constraint is satisfied, and:
               - event-conditions are newly satisfied (previously
                 not satisfied), OR
               - event-conditions remain satisfied but for a
                 different reason. */
   SINGLE   /* Produces a single log on the activation cycle, and then
                  self-deactivates.*/
  };


 protected:
  LoggingSpecification log_spec; /* (--)
    Specification of conditions under which the sim values are recorded for
    logging output.*/
  bool record_trigger_name; /* (--)
    An internal flag used to identify whether to append the name of the
    trigger that resulted in logging a particular data set. This only has
    meaning when the logging is conditional on the event, and when the event
    is satisfied by any triggger.
    Default: false
    Set by set_record_trigger_name(bool).*/
  std::string DISCRETE_condition; /* (--)
    Copy of the most recent value of event.trigger_name.
    Populated and used only with DISCRETE LoggingSpecification.*/
  bool DISCRETE_active; /* (--)
    Flag retained for next cycle to indicate whether the logging conditions
    were satsified in this cycle.
    Used only with DISCRETE LoggingSpecification.*/
  bool file_initialized; /* (--)
    Flag initially set to false, and set to true after the file has been opened.
    Determines whether to overwrite an existing file, or append to it.*/

 public:
  bool log_at_deactivate; /* (--)
    Boolean indicating whether the group should create a log of its values at
    deactivation.*/

  EnhancedLogging_SerialGroup( std::string name,
                               const double & dyn_time);
  virtual ~EnhancedLogging_SerialGroup();

  void initialize( const std::string &) override;
  void update() override;
  void shutdown() override;
  void set_record_trigger_name(bool);
  void set_log_spec( LoggingSpecification);

  void log_group_csv(std::ofstream &) override;
  void log_group_csv_fmt( std::ofstream &) override;

  void add_variable( EnhancedLogging_Variable &) override;
  void add_variable( EnhancedLogging_VariableSet &) override;
  void add_variable( EnhancedLogging_Variable &,
                     unsigned int ix_lo,
                     unsigned int ix_hi) override;
  void add_variable( const std::vector<EnhancedLogging_SerialVariable *> &);
  void add_variable( const std::vector<EnhancedLogging_SerialVariable *> &,
                     unsigned int ix_lo,
                     unsigned int ix_hi);
  void add_variable( EnhancedLogging_SerialVariable * var);

  template <typename T>
  void add_variable( const T & var,
                     std::string name,
                     std::string units="--")
  {
    EnhancedLogging_BaseVariable * new_var =
        new EnhancedLogging_SerialVariableT<T>( var,
                                                name,
                                                units);
    variables.push_back( new_var);
    allocated_variables.push_back( new_var);
  }
 protected:
  void add_variable_internal( EnhancedLogging_BaseVariable *);

protected:
  void deactivate();
  void initialize_file() override;

 private: // remove operator=, copy-constructor from SWIG accessibility
  EnhancedLogging_SerialGroup( const EnhancedLogging_SerialGroup&);
  EnhancedLogging_SerialGroup operator=( const EnhancedLogging_SerialGroup&);
};
#endif
