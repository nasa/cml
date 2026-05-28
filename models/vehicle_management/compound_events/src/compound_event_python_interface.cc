/*******************************TRICK HEADER******************************
PURPOSE: Provides the Python / SWIG-friendly interface for adding actions
         from a Python input file.()

PROGRAMMERS:
  (((Gary Turner) (OSR) (2023/10) (ANTARES)
    (Comprehensive overhaul of event management)))
**********************************************************************/


#include <string>
#include "../include/compound_event_python_interface.hh"

/* Note: these are the ways in which an event can have an action added to it via
   the input file. But most of the action content can be redirected anyway,
   Available actions are:
   trick.stop -- use WatchValuesSimStop
   trick.message_publish -- use the message field in WatchValuesBaseCore
   DataRecordGroup.record  -- use WatchValuesLogNow
                               or WatchValuesAdjustLogging with "log_now = true"
                                  if also adjusting logging rate.
   DataRecordGroup.cycle   -- use WatchValuesAdjustLogging with "log_cycle = ***"
     // TODO -- do we need a DR-group specific adjuster? If so, TrickLogging has
     //         methods to hit these with specific group names rather than
     //         all-groups; current events do all-groups simultaneously but that
     //         can be easily changed/extended by adding a name field.
   DataCollectionFileWrite -- calls data_collection->output_HDF5_multiGROUP(...)
                                TODO need to provide this one.
  assignment  -- abandon this, it cannot be done safely through SWIG.
*/

void
CompoundEventPythonInterface::add_action( std::string name,
                                          double      action_value,
                                          std::string action_string)
{
  CompoundEventActionConfig config;
  config.name = name;
  config.target_value_assignment = action_value;
  config.target_string_assignment = action_string;
  actions.push_back(config);
}


void
CompoundEventPythonInterface::add_action( std::string name,
                                          std::string action_string)
{
  CompoundEventActionConfig config;
  config.name = name;
  config.target_string_assignment = action_string;
  actions.push_back(config);
}

void
CompoundEventPythonInterface::add_action( std::string name,
                                          double      action_value)
{
  CompoundEventActionConfig config;
  config.name = name;
  config.target_value_assignment = action_value;
  actions.push_back(config);
}

void
CompoundEventPythonInterface::add_action( std::string name,
                                          void *      target_ptr,
                                          double      action_value)
{
  CompoundEventActionConfig config;
  config.name = name;
  config.target_ptr = target_ptr;
  config.target_value_assignment = action_value;
  actions.push_back(config);
}

void
CompoundEventPythonInterface::add_action( std::string name,
                                          void *      target_ptr)
{
  CompoundEventActionConfig config;
  config.name = name;
  config.target_ptr = target_ptr;
  actions.push_back(config);
}

void
CompoundEventPythonInterface::add_action( std::string name)
{
  CompoundEventActionConfig config;
  config.name = name;
  actions.push_back(config);
}
