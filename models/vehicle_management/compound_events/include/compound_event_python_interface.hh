/*******************************TRICK HEADER******************************
PURPOSE: Provides the Python / SWIG-friendly interface for adding actions
         from a Python input file.()

LIBRARY DEPENDENCY:
   ((../src/compound_event_python_interface.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (2023/10) (ANTARES)
    (Comprehensive overhaul of event management)))
**********************************************************************/
#ifndef GROK_EVENTS_PYTHON_INTERFACE_HH
#define GROK_EVENTS_PYTHON_INTERFACE_HH
#include <string>
#include <vector>

/*****************************************************************************
CompoundEventActionConfig
Purpose:
  Provides a structure for staging the definition of an action via Python;
  contents are parsed into CompoundEventBase
*****************************************************************************/
struct CompoundEventActionConfig
{
  std::string name;
  std::string datatype;
  void * target_ptr;
  double target_value_assignment;
  std::string target_string_assignment;
  CompoundEventActionConfig()
    :
    name(""),
    datatype(""),
    target_ptr(nullptr),
    target_value_assignment(0.0),
    target_string_assignment("")
  {};
};

/*****************************************************************************
CompoundEventTriggerConfig
Purpose:
  Provides a structure for staging the definition of a trigger via Python;
  contents are parsed into CompoundEventBase
*****************************************************************************/
struct CompoundEventTriggerConfig
{
  std::string trigger_type; /* (--) "action", "arm", "disarm"*/
  std::string var_name;
  std::string comparator_operation;
  std::string target_value;
};

/*****************************************************************************
CompoundEventPythonInterface
Purpose:
  Provides a temporary storage of configuration data for processing by
  CompoundEventManager.
*****************************************************************************/
struct CompoundEventPythonInterface
{
 public:
  std::string name;
  double      call_rate;
  bool        flag_report;
  std::vector< CompoundEventTriggerConfig > trigger_configs;
  std::vector< CompoundEventActionConfig >  actions;
  void add_action( std::string, double, std::string);
  void add_action( std::string, std::string);
  void add_action( std::string, double);
  void add_action( std::string, void*, double);
  void add_action( std::string, void*);
  void add_action( std::string);
};

#endif
