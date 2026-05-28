/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides the overall management of a set of logging-groups.
  The logging -groups are accessed via EnhancedLogging_BaseGroup pointers;
  EnhancedLogging_BaseGroup is an abstract class and these pointers address
  instances of either:
  - EnhancedLogging_SummaryGroup
  - EnhancedLogging_SerialGroup
  )

LIBRARY DEPENDENCY:
   ((../src/enhanced_logging_manager.cc)
   )

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES)
    (Implementation loosely based heavily on DataCollect in GNC_PAR,
     with extensive upgrades)))
***********************************************************************/
#ifndef CML_ENHANCED_LOGGING_BASE_MANAGER_HH
#define CML_ENHANCED_LOGGING_BASE_MANAGER_HH

#include "base_logging_group.hh"
#include "serial_logging_group.hh"
#include "summary_logging_group.hh"

#include "trick/CommandLineArguments.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include <list>

class EnhancedLogging_Manager : public SubscriptionBase {
 protected:
  const double & dyn_time; /* (s)
    Reference to simulation dynamic-time.*/
  std::list<EnhancedLogging_BaseGroup*> all_groups; /* (--)
    Set of pointers to all groups known by this logging manager.*/
  std::list<EnhancedLogging_BaseGroup*> active_groups; /* (--)
    Set of pointers to all groups currently actively creating logs.*/
  std::list<EnhancedLogging_BaseGroup*> subscription_pending_groups; /* (--)
    Set of pointers to all groups waiting to be added to the active list.
    This was created to ensure that if a manager has not been activated and
    it subscribes a group, this list will ensure that it subscribed at 
    manager activation.*/
  std::list<const EnhancedLogging_BaseGroup*> allocated_groups; /* (--)
    Set of pointers to groups dynamically allocated by this manager.
    Used only in the destructor for appropriate memory management.*/
  std::list<EnhancedLogging_Variable> allocated_variables; /* (--)
    Set of proto-logging-variable instances created internally by the manager.
    Logging variables will be created from these instances.
    Note: the logging manager owns these and will destroy them and consequently
    all the logging-variables they create at the manager destruction; if
    using multiple managers, do not cross-link these variables.*/
  std::list<EnhancedLogging_Variable*> external_variables; /* (--)
    A register of externally-provided proto-logging-variable instances.
    When adding a variable to a group, this list and the allocated_variables
    list will be searched; if no match is found a new proto-logging-variable
    will be created. Registering externally instantiated proto-logging-variable
    instances with this manager avoids unnecessary duplication.
    Note: there is no mechanism to supportt searching the entire sim database
    for these instances, they have to be registered with the manager for the
    manager to find them.*/

  std::string hdf5_py_converter; /* (--)
    Path to the Python HDF5 converter used to convert from CSV to HDF5.
    TODO Turner 2024/06
         Replace with a C++ version.*/
  std::string target_dir; /* (--)
    Path to the location where data files will be written.*/


 public:
  EnhancedLogging_BaseGroup::LoggingFormat logging_format; /* (--)
    Identification of the logging format to be used for a consolidated data
    file. This is used only if "consolidate_group_data" is set to true.
    Otherwise, each group has its own logging-format specification.*/
  bool consolidate_summary_group_data; /* (--)
    Flag indicating whether summary-groups should produce their own data
    files, or consolidate all summaries into one file.
    Default: false (individual files).*/

 public:
  explicit EnhancedLogging_Manager( const double & dyn_time);
  virtual ~EnhancedLogging_Manager();

  void initialize();
  void activate();
  void update();
  void subscribe_group();
  void subscribe_group( std::string name);
  void subscribe_group( EnhancedLogging_BaseGroup&);
  void unsubscribe_group( std::string name);
  void shutdown();

  EnhancedLogging_BaseGroup * get_last_group();
  EnhancedLogging_BaseGroup * get_group( std::string name);
  EnhancedLogging_SerialGroup * get_serial_group( std::string name);
  EnhancedLogging_SummaryGroup * get_summary_group( std::string name);
  void add_group(EnhancedLogging_BaseGroup & group);

  EnhancedLogging_SerialGroup & new_serial_group( std::string name,
                                                  bool auto_subscribe=false);
  EnhancedLogging_SummaryGroup & new_summary_group( std::string name,
                                                    bool auto_subscribe=false);

  EnhancedLogging_Variable* get_variable( const std::string &,
                                          const std::string &);
  void add_variable( std::string name,
                     std::string alias = "",
                     std::string units = "");
  void add_variable( EnhancedLogging_Variable &);
  void add_variable( std::list< EnhancedLogging_Variable> &);
  void add_variable( EnhancedLogging_VariableSet &);


  void add_variable( EnhancedLogging_BaseGroup & group,
                     std::string name,
                     std::string alias = "",
                     std::string units = "");
  void add_variable( EnhancedLogging_BaseGroup &,
                     EnhancedLogging_Variable &);
  void add_variable( EnhancedLogging_BaseGroup &,
                     std::list<EnhancedLogging_Variable> &);
  void add_variable( EnhancedLogging_BaseGroup &,
                     EnhancedLogging_VariableSet &);
  void add_variable( EnhancedLogging_BaseGroup &,
                     EnhancedLogging_Variable & var,
                     unsigned int ix_lo,
                     unsigned int ix_hi);

  void log_all();

 private:
  void add_to_active_list( EnhancedLogging_BaseGroup& new_group);
  void convert_to_hdf5( const std::string & filename);
  void get_target_dir();

 private: // remove operator=, copy-constructor from SWIG accessibility
  EnhancedLogging_Manager( const EnhancedLogging_Manager&);
  EnhancedLogging_Manager & operator=( const EnhancedLogging_Manager&);
};
#endif
