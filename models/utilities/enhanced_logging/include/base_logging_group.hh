/*******************************TRICK HEADER******************************
PURPOSE: (Provides the abstract base-class for the serial-logging group
          and summary-=logging-group.)

LIBRARY DEPENDENCY:
   ((../src/base_logging_group.cc)
   )

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/
#ifndef CML_ENHANCED_LOGGING_BASE_GROUP_HH
#define CML_ENHANCED_LOGGING_BASE_GROUP_HH

#include <string>
#include <list>
#include <fstream> // ofstream

#include "base_logging_variable.hh"
#include "enhanced_logging_variable.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/vehicle_management/compound_events/include/compound_event.hh"

/*****************************************************************************
EnhancedLogging_BaseGroup
Purpose:
  A EnhancedLogging_BaseGroup is the abstract base class of
  EnhancedLogging_SerialGroup and EnhancedLogging_SummaryGroup.
*****************************************************************************/
class EnhancedLogging_BaseGroup : public SubscriptionBase
{
 protected:
  const double & dyn_time; /* (s)
    Clock used in event and for recording timestamps and elapsed times*/

 public:
  enum LoggingFormat{
    CSV,
    CSV_FMT,
    HDF5
  } logging_format; /* (--)
    Specification of format to use for recording data.*/

  CompoundEvent event; /* (--)
    A controlling event that controls the group at the top level.
    For a Serial-logging group, this event determines whether the group
    appends a new data set to its output.
    Fpr a Summary-logging group, this event determines whether the
    group will start assessing its logging-conditions.*/
  std::string name; /* (--)
    The group name, used for identification purposes. */
  double minimum_update_period; /* (s)
    Minimum duration between logging cycles for this group.*/
  bool create_individual_group_file; /* (--)
    Flag used to tell whether to create a file for this group (intended
    to be set to false when using consolidate_summary_group_data from the 
    logging_manager).*/

 protected:
  std::string target_dir; /* (--)
    Path to the location where data files will be written.*/
  std::ofstream file_out; /* (--)
    The output stream to which data will be sent.*/
  std::string csv_file; /* (--)
    The generated filename for the csv logged data.*/
  bool always_triggered; /* (--)
    Flag used when bypassing the compoundEvent inheritance of this class.
    If the group is simply activated/deactivated with subscriptions, then
    this flag should get set to true automatically.*/
  double t_next_update; /* (s)
    Bypass updates to this group until dyn-time is at least this value.*/
  double t_last_update; /* (s)
    Used to bypass updates to this group with timestamps that have already
    processed. Note -- not redundant with t_next_update because
    minimum_update_period could be legitimately 0.0.*/
  std::string log_hdf5_py_file; /* (--)
    Name of python file that parses CSV formatted log file and outputs to
    HDF5 data_collect file */

  std::list< EnhancedLogging_BaseVariable* > variables; /* (--)
    List of pointers to the ivariables being logged by this group.
    These are base-class pointers to instances of type
    EnhancedLogging_SerialVariableT<T> and
    EnhancedLogging_SummaryVariableT<T> .*/
  std::list< EnhancedLogging_BaseVariable* > allocated_variables; /* (--)
    List of pointers to the group-specific instances of loggable-variables.*/
 public:
  EnhancedLogging_BaseGroup( std::string name,
                             const double & dyn_time);
  virtual ~EnhancedLogging_BaseGroup(){};

  virtual void initialize( const std::string &);
  bool update_base();
  virtual void update() = 0;
  virtual void shutdown() = 0;

  virtual void add_variable( EnhancedLogging_Variable &) = 0;
  void add_variable( std::list< EnhancedLogging_Variable> &);
  virtual void add_variable( EnhancedLogging_VariableSet &) = 0;
  virtual void add_variable( EnhancedLogging_Variable &,
                             unsigned int ix_lo,
                             unsigned int ix_hi) = 0;

  /* Note -- there is no option to add a variable based on a
   * template-argument referencing the actual variable because we still need
   * the variable name or alias to it as a string for logging purposes. This
   * capability is pushed down to the serial-variable and summary-variable.*/

  void remove_variable( std::string name);
  EnhancedLogging_BaseVariable* find_variable (const std::string var_alias);

  void set_target_dir (const std::string& dir) {target_dir = dir;}
  virtual void log_group();
  virtual void log_group(std::ofstream &);

  std::string & get_csv_file() {return csv_file;}
  virtual bool is_summary() {return false;}

 protected:
  virtual void log_group_csv(std::ofstream &) = 0;
  virtual void log_group_csv_fmt(std::ofstream &) = 0;
  /* TODO Turner 2024/06
  *    Provide a direct-to-HD5 logging capability. Current (temporary)
  *    implementation requires writing to a CSV, then converting that to HD5.
  */

  virtual void initialize_file(){};
  void activate();
  bool check_alias( const std::string & name,
                    const std::string & alias);

 private: // remove operator=, copy-constructor from SWIG accessibility
  EnhancedLogging_BaseGroup( const EnhancedLogging_BaseGroup&);
  EnhancedLogging_BaseGroup & operator=( const EnhancedLogging_BaseGroup&);
};
#endif
