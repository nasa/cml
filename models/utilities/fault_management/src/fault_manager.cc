/*############################################################################
PURPOSE:
  (Fault injection)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/


#include "../include/fault_manager.hh"
#include <cstdlib> // strtod, strtol, etc.
#include <cstring> // strcmp
#include <cmath>   // abs
#include <libxml/parser.h> // xmlParseFile, xmlNodePtr
#include "cml/models/utilities/convert_string/include/convert_string.hh"

bool FaultManager::global_enabled = true;


/*****************************************************************************
Constructor
*****************************************************************************/
FaultManager::FaultManager() :
  enabled(false),
  parsed(false)
{}


/*****************************************************************************
Destructor
*****************************************************************************/
FaultManager::~FaultManager() {
  for (unsigned int ii = 0; ii < Location_count; ii++) {
    for (auto fault : faults[ii]) {
      delete fault;
    }
  }
  for (auto trigger : triggers) {
    delete trigger;
  }
  for (auto trigger_group : trigger_groups) {
    delete trigger_group;
  }
}


/*******************************************************************************
translate_location
Purpose:(Translates a string into a Location.)
*******************************************************************************/
FaultManager::Location FaultManager::translate_location( const char* str) {
  if (strcmp(str, "INIT") == 0) {
    return Location::Initialize;
  } else if (strcmp(str, "UPSTREAM") == 0) {
    return Location::Upstream;
  } else if (strcmp(str, "INTERMEDIATE_1") == 0) {
    return Location::Intermediate_1;
  } else if (strcmp(str, "INTERMEDIATE_2") == 0) {
    return Location::Intermediate_2;
  } else if (strcmp(str, "DOWNSTREAM") == 0) {
    return Location::Downstream;
  } else {
    return Location::INVALID;
  }
}


/*******************************************************************************
initialize
Purpose:(Parses the XML file and initializes the faults.)
*******************************************************************************/
void FaultManager::initialize() {
  if (!enabled || !global_enabled) {
    return;
  }
  parse();
  for (unsigned int ii = 0; ii < Location_count; ii++) {
    for (auto fault : faults[ii]) {
      fault->initialize(); // Initialize fault
    }
  }
  // Go back and put in all the requested param changes.
  for (auto& it : set_trigger_value_cache) {
    set_trigger_value(it.first, it.second);
  }
  for (auto& it : set_fault_param_cache) {
    set_fault_param(it.fault_name, it.param_name, it.value);
  }
}


/*******************************************************************************
update
Purpose:(Injects faults.)
*******************************************************************************/
void FaultManager::update( const Location& location) {
  if (enabled && global_enabled) {
    unsigned char location_index = static_cast<unsigned char>(location);
    if (location_index >= Location_count) {
      CMLMessage::fail(__FILE__, __LINE__,
        "Fault Management Error\n",
        "Invalid location passed to FaultManager::update.\n");
    } else {
      for (auto fault : faults[location_index]) {
        fault->update();
      }
    }
  }
}


/*******************************************************************************
get_fault
Purpose:(Looks up a fault by name. If no fault with that name is found, returns
         nullptr.)
*******************************************************************************/
Fault* FaultManager::get_fault( std::string name) {
  for (unsigned int ii = 0; ii < Location_count; ii++) {
    for (auto fault : faults[ii]) {
      if (name.compare(fault->name) == 0) {
        return fault;
      }
    }
  }

  return nullptr;
}


/*******************************************************************************
get_trigger
Purpose:(Looks up a trigger by name. If no trigger with that name is found,
         returns nullptr.)
*******************************************************************************/
TriggerBase* FaultManager::get_trigger( std::string name) {
  for (auto trigger : triggers) {
    if (name.compare(trigger->name) == 0) {
      return trigger;
    }
  }

  return nullptr;
}


/*******************************************************************************
set_fault_enabled
Purpose:(Enables or disables a fault.)
*******************************************************************************/
bool FaultManager::set_fault_enabled(
  std::string fault_name,
  bool        enable_flag)
{
  if (parsed) {
    Fault* fault = get_fault(fault_name);
    if (fault != nullptr) {
      if (enable_flag) {
        fault->enabled = true;
      } else {
        // disables the fault and resets the was_triggered_last_pass flag
        fault->disable();
      }
      return true;
    } else {
      CMLMessage::error(__FILE__, __LINE__,
        "Fault Management Error\n",
        "The fault <", fault_name, "> was not found and could not be enabled / disabled.\n");
      return false;
    }
  }
  // if not parsed, store it away for later.
  else {
    set_enable_for_fault_cache.push_back(
      std::pair<std::string, bool>(fault_name, enable_flag));
  }
  return true;
}


/*******************************************************************************
set_fault_trigger_enabled
Purpose:(Enables or disables a trigger for a specific fault. Triggers that are
         shared by multiple faults are disabled on a fault-by-fault basis.)
*******************************************************************************/
bool FaultManager::set_fault_trigger_enabled(
  std::string fault_name,
  std::string trigger_name,
  bool        enable_flag)
{
  if (parsed) {
    Fault* fault = get_fault(fault_name);
    if (fault == nullptr) {
      CMLMessage::error(__FILE__, __LINE__,
        "Fault Management Error\n",
        "The fault <", fault_name, "> was not found and its trigger could not be "
        "enabled / disabled.\n");
      return false;
    } else {
      bool trigger_found = false;
      for (auto tg : fault->trigger_groups) {
        trigger_found = tg->set_trigger_enable(trigger_name, enable_flag) ||
          trigger_found;
      }

      if (!trigger_found) {
        CMLMessage::error(__FILE__, __LINE__,
          "Fault Management Error\n",
          "The trigger: ", trigger_name, "\nwas not found for the fault: ", fault_name, "\n");
        return false;
      }
    }
  }
  // if not parsed, store it away for later
  else {
    set_trigger_enable_for_fault_cache.push_back(
      SetTriggerEnableEntry{fault_name, trigger_name, enable_flag});
  }
  return true;
}


/*******************************************************************************
set_fault_param
Purpose:(Sets the value of a fault parameter. What these parameters can be
         depends on the type of fault.)
*******************************************************************************/
bool FaultManager::set_fault_param(
  std::string fault_name,
  std::string param_name,
  double      value,
  bool        modify_nominal_with_rate)
{
  if (parsed) {
    Fault* fault = get_fault(fault_name);
    if (fault == nullptr) {
      CMLMessage::error(__FILE__, __LINE__,
        "Fault Management Error\n",
        "The fault <", fault_name, "> was not found and its parameter could not be set.\n");
      return false;
    }
    return fault->set_param(param_name, value, modify_nominal_with_rate);
  }
  // if not parsed, store it away for later, don't need the optional boolean
  // if the manager has not yet been parsed because this modification will be
  // applied at initialization, and the optional boolean is only to support
  // mid-sim changes.
  else {
    set_fault_param_cache.push_back(
      SetFaultParamEntry{fault_name, param_name, value});
  }
  return true;
}


/*******************************************************************************
set_trigger_value
Purpose:(Sets a trigger value.)
*******************************************************************************/
bool FaultManager::set_trigger_value(
  std::string trigger_name,
  double      value)
{
  if (parsed) {
    TriggerBase* trigger = get_trigger(trigger_name);

    if (trigger == nullptr) {
      CMLMessage::error(__FILE__, __LINE__,
        "Fault Management Error\n",
        "The trigger >", trigger_name, "> was not found and its value could not be set.\n");
      return false;
    }

    trigger->set_value(value);
  } else {
    set_trigger_value_cache.push_back(
      std::pair<std::string, double>(trigger_name, value));
  }
  return true;
}

/*******************************************************************************
unset_trigger_count
Purpose:(Removes the trigger-count-limit for the specified trigger.)
*******************************************************************************/
void FaultManager::unset_trigger_count( std::string trigger_name) {
  if (parsed) {
    TriggerBase* trigger = get_trigger(trigger_name);

    if (trigger != nullptr) {
      trigger->unset_trigger_count();
    }
  }
}
/*******************************************************************************
parse
Purpose:(Parses the XML file.)
*******************************************************************************/
void FaultManager::parse() {

  xmlDocPtr doc = xmlParseFile(fault_file.c_str());

  if (!doc) {
    CMLMessage::fail(__FILE__, __LINE__,
      "Fault Management Error\n",
      "\nThe following fault file could not be opened.\n", fault_file, "");
    // Terminated
  }
  xmlNodePtr root = doc->children;
  if (!root) {
    // Unreachable code, manually tested.
    CMLMessage::fail(__FILE__, __LINE__,
      "Fault Management Error\n",
      "The fault file >", fault_file, "> did not correctly parse for unknown reasons.\n");
    // Terminated
  }
  // Make sure the XML file we are working with is for FaultManager
  if (!XmlHelper::xml_name_match(root, "FaultManager")) {
    CMLMessage::fail(__FILE__, __LINE__,
      "Fault Management Error\n",
      "The following fault file did not specify a FaultManager hierarchy.\n", fault_file, "\n");
    // Terminated
  }

  // For each fault entry, parse it
  for (xmlNodePtr fault_node = root->children;
       fault_node != nullptr;
       fault_node = fault_node->next)
  {
    if (XmlHelper::xml_name_match(fault_node, "Fault")) {
      parse_fault(fault_node);
    }
  }

  parsed = true;

  // Go back and re-send the externally set variables that were cached off
  // prior to parsing.
  for (auto& it : set_enable_for_fault_cache) {
    set_fault_enabled(it.first, it.second);
  }
  for (auto& it : set_trigger_enable_for_fault_cache) {
    set_fault_trigger_enabled(it.fault_name, it.trigger_name, it.enable_flag);
  }
  // Note -- the fault_param_cache and trigger_value_cache pending assignments
  // are cleared in initialize() after this method has returned and after the
  // individual faults have been initialized.
}


/*******************************************************************************
parse_fault
Purpose:(Parses a fault node.)
*******************************************************************************/
void FaultManager::parse_fault( xmlNodePtr fault_node) {
  // Parse out the fault name
  const char* fault_name = XmlHelper::xml_find_value(fault_node, "ID");
  if (fault_name == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "A fault does not have an ID name. All faults must have a unique ID "
      "name.\n");
    return;
  }
  if (get_fault(fault_name) != nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "A fault named: <", fault_name, "> was already found in the fault map.\n"
      "Cannot have two faults with identical names.\n"
      "This Fault will be ignored.\n");
    return;
  }

  // Check for valid location string
  const char* loc_string = XmlHelper::xml_find_value(fault_node, "loc");
  if (loc_string == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault named <", fault_name, "> does not have an injection location.\n"
      "Injection location is required.\n"
      "This Fault will be ignored.\n");
    return;
  }
  Location location = translate_location(loc_string);
  if (location == Location::INVALID) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault named <", fault_name, "> does not have a valid injection location.\n"
      "Injection location is required.\n"
      "This Fault will be ignored.\n");
    return;
  }

  // Get Trick Reference Attributes of the variable to be faulted
  xmlNodePtr sim_var = XmlHelper::xml_find_child(fault_node, "SimVar");
  if (sim_var == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> is missing the parameter \"SimVar\".\n"
      "Target variable (to which the fault will be applied) is required.\n"
      "This Fault will be ignored.\n");
    return;
  }
  const char* sim_var_name = XmlHelper::xml_find_value(sim_var, "name");
  if (sim_var_name == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> is missing the \"name\" field in \"SimVar\".\n"
      "Target variable (to which the fault will be applied) is required.\n"
      "This Fault will be ignored.\n");
    return;
  }
  REF2* Symbol = get_trick_variable(sim_var_name);
  if (Symbol == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> is to be applied to variable >", sim_var_name, "\",\n"
      "but >", sim_var_name, "> cannot be found in the simulation.\n"
      "Target variable (to which the fault will be applied) is required.\n"
      "This Fault will be ignored.\n");
    return;
  }

  Fault* new_fault = nullptr;
  switch (Symbol->attr->type) {
    case TRICK_CHARACTER:
      new_fault = make_fault<char>(
        fault_node, *static_cast<char*>(Symbol->address), fault_name);
      break;
    case TRICK_UNSIGNED_CHARACTER:
      new_fault = make_fault<unsigned char>(
        fault_node, *static_cast<unsigned char*>(Symbol->address), fault_name);
      break;
    case TRICK_SHORT:
      new_fault = make_fault<short>(
        fault_node, *static_cast<short*>(Symbol->address), fault_name);
      break;
    case TRICK_UNSIGNED_SHORT:
      new_fault = make_fault<unsigned short>(
        fault_node, *static_cast<unsigned short*>(Symbol->address), fault_name);
      break;
    case TRICK_INTEGER:
      new_fault = make_fault<int>(
        fault_node, *static_cast<int*>(Symbol->address), fault_name);
      break;
    case TRICK_UNSIGNED_INTEGER:
      new_fault = make_fault<unsigned int>(
        fault_node, *static_cast<unsigned int*>(Symbol->address), fault_name);
      break;
    case TRICK_LONG:
      new_fault = make_fault<long>(
        fault_node, *static_cast<long*>(Symbol->address), fault_name);
      break;
    case TRICK_UNSIGNED_LONG:
      new_fault = make_fault<unsigned long>(
        fault_node, *static_cast<unsigned long*>(Symbol->address), fault_name);
      break;
    case TRICK_FLOAT:
      new_fault = make_fault<float>(
        fault_node, *static_cast<float*>(Symbol->address), fault_name);
      break;
    case TRICK_DOUBLE:
      new_fault = make_fault<double>(
        fault_node, *static_cast<double*>(Symbol->address), fault_name);
      break;
    case TRICK_LONG_LONG:
      new_fault = make_fault<long long>(
        fault_node, *static_cast<long long*>(Symbol->address), fault_name);
      break;
    case TRICK_UNSIGNED_LONG_LONG:
      new_fault = make_fault<unsigned long long>(
                       fault_node,
                       *static_cast<unsigned long long*>(Symbol->address),
                       fault_name);
      break;
    case TRICK_BOOLEAN: {
      // Checking on type is provided in make_fault, but make_fault is bypassed
      // for boolean types because they have additional restrictions.
      // So have to check type separately.
      const char* fault_type = XmlHelper::xml_find_value( fault_node,
                                                          "type",
                                                          true);
      if (fault_type == nullptr) {
        CMLMessage::error(__FILE__,__LINE__,
          "XML input error parsing fault configuration\n",
          "The fault <", fault_name, "> does not have a specified fault-type.\n"
          "Fault-type is required.\n"
          "This Fault will be ignored.\n");
        return;
      }
      bool& boolvar = *static_cast<bool*>(Symbol->address);
      if (strcmp(fault_type, "OVERWRITE") == 0) {
        new_fault = make_fault_overwrite<bool>( fault_node,
                                                boolvar,
                                                fault_name);
      } else if (strcmp(fault_type, "STALE") == 0) {
        new_fault = new FaultStale<bool>(boolvar);
      } else {
        CMLMessage::error(__FILE__,__LINE__,
          "XML input error parsing fault configuration\n",
          "The fault <", fault_name, "> specifies the Boolean variable >", sim_var_name, "\"\n"
          "and a fault-type >", fault_type, "\".\n"
          "This combination is not valid.\n"
          "This Fault will be ignored.\n");
      }
      break;
    }
    default:
      CMLMessage::error(__FILE__,__LINE__,
        "XML input error parsing fault configuration\n",
        "The variable >", sim_var_name, "> (the sim variable specified by fault <", fault_name, ">)\n"
        "is not of a type that can be faulted.\n"
        "This Fault will be ignored.\n");
  }
  if (new_fault == nullptr) {
    return;
  }

  // Optional parameters
  new_fault->name.assign(fault_name);
  const char* enabled_str = XmlHelper::xml_find_value(fault_node, "enabled");
  // If enabled option is not present, default to enabled.
  new_fault->enabled = enabled_str == nullptr ||
    ConvertString::convert<bool>(enabled_str);

  // Limit on number of frames the fault can be triggered
  const char* fire_limit_string =
    XmlHelper::xml_find_value(fault_node, "fire_limit");
  if (fire_limit_string != nullptr) {
    new_fault->is_fire_limited = true;
    new_fault->fire_limit = strtol(fire_limit_string, nullptr, 10);
  }

  for (xmlNodePtr trigger_group_node = fault_node->children;
       trigger_group_node != nullptr;
       trigger_group_node = trigger_group_node->next)
  {
    if (XmlHelper::xml_name_match(trigger_group_node, "TriggerGroup")) {
      TriggerGroup* new_trigger_group =
        parse_trigger_group(trigger_group_node, fault_name);
      if (new_trigger_group != nullptr) {
        new_fault->add_trigger_group(*new_trigger_group);
      }
    }
  }

  faults[static_cast<unsigned char>(location)].push_back(new_fault);
}


/*******************************************************************************
get_trick_variable
Purpose:(Gets a Trick Reference from a variable name and checks that the
         variable is not a pointer.)
*******************************************************************************/
REF2* FaultManager::get_trick_variable( const char* variable_name) {
  REF2* Symbol = trick_MM->ref_attributes(variable_name);
  if (Symbol == nullptr) {
    CMLMessage::error(
      __FILE__, __LINE__,"Trick lookup error parsing variable name\n",
      "No Trick variable named >", variable_name, "> found.\n");
    return nullptr;
  }

  // num_index is the variable's number of indirections - e.g. a double** would
  // have a num_index of 2. This model does not support the faulting of
  // pointers or the use of pointers as independent or trigger variables.
  if (Symbol->attr->num_index != 0) {
    CMLMessage::error(__FILE__, __LINE__,
      "Trick lookup error parsing variable name\n",
      "The variable >", variable_name, "> is a pointer.\n"
      "The FaultManager does not support the faulting of pointers or the\n"
      "use of pointers as trigger variables.\n");
    return nullptr;
  }
  return Symbol;
}


/*******************************************************************************
make_fault
Purpose:(Determines the type of a fault.)
*******************************************************************************/
template<typename T> Fault* FaultManager::make_fault(
  xmlNodePtr  fault_node,
  T&          variable,
  const char* fault_name)
{
  const char* fault_type = XmlHelper::xml_find_value( fault_node,
                                                      "type",
                                                      true);
  if (fault_type == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> does not have a specified fault-type.\n"
      "Fault-type is required.\n"
      "This Fault will be ignored.\n");
    return nullptr;
  }

  Fault* new_fault = nullptr;
  if (strcmp(fault_type, "BIAS") == 0) {
    new_fault = make_fault_bias<T>(fault_node, variable, fault_name);
  } else if (strcmp(fault_type, "SCALE") == 0) {
    new_fault = make_fault_scale<T>(fault_node, variable, fault_name);
  } else if (strcmp(fault_type, "OVERWRITE") == 0) {
    new_fault = make_fault_overwrite<T>(fault_node, variable, fault_name);
  } else if (strcmp(fault_type, "STALE") == 0) {
    new_fault = new FaultStale<T>(variable);
  } else if (strcmp(fault_type, "FUNCTION") == 0) {
    FaultFunctionBase* function_fault = new FaultFunction<T>(variable);
    if (populate_fault_function(function_fault, fault_node, fault_name)) {
      new_fault = function_fault;
    } else {
      delete function_fault;
      new_fault = nullptr;
    }
  } else if (strcmp(fault_type, "WHITENOISE") == 0) {
    new_fault = make_fault_white_noise<T>(fault_node, variable, fault_name);
  } else if (strcmp(fault_type, "RANDOMWALK") == 0) {
    new_fault = make_fault_random_walk<T>(fault_node, variable, fault_name);
  } else {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> has an invalid fault type of <", fault_type, "\".\n"
      "A valid fault-type is required.\n"
      "This Fault will be ignored.\n");
  }
  return new_fault;
}


/*******************************************************************************
make_fault_bias
Purpose:(Constructs a bias fault.)
*******************************************************************************/
template<typename T> Fault* FaultManager::make_fault_bias(
  xmlNodePtr  fault_node,
  T&          variable,
  const char* fault_name)
{
  // Get the Bias value.
  const char* bias_string = nullptr;
  xmlNodePtr bias_node = XmlHelper::xml_find_child(fault_node, "Bias");
  if (bias_node == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> has no Bias child node specified.\n"
      "A valid Bias-node \"<Bias>...</Bias>\" is required for BIAS faults.\n"
      "This Fault will be ignored.\n");
    return nullptr;
  }

  bias_string = XmlHelper::xml_find_value(bias_node, "value");
  if (bias_string == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> has no value specified in its Bias child node.\n"
      "The Bias value is required for BIAS faults.\n"
      "This Fault will be ignored.\n");
    return nullptr;
  }

  FaultBias<T>* new_fault = new FaultBias<T>(variable);
  new_fault->bias = ConvertString::convert<T>(bias_string);

  return new_fault;
}


/*******************************************************************************
make_fault_scale
Purpose:(Constructs a scale fault.)
*******************************************************************************/
template<typename T> Fault* FaultManager::make_fault_scale(
  xmlNodePtr  fault_node,
  T&          variable,
  const char* fault_name)
{
  // Get the Scale value.
  xmlNodePtr scale_node = XmlHelper::xml_find_child(fault_node, "Scale");
  if (scale_node == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> has no Scale child node specified.\n"
      "A valid Scale-node \"<Scale>...</Scale>\" is required for SCALE faults.\n"
      "This Fault will be ignored.\n");
    return nullptr;
  }

  const char* scale_string = nullptr;
  scale_string = XmlHelper::xml_find_value(scale_node, "value");
  if (scale_string == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> has no value specified in its Scale child node.\n"
      "The Scale value is required for SCALE faults.\n"
      "This Fault will be ignored.\n");
    return nullptr;
  }

  FaultScale<T>* new_fault = new FaultScale<T>(variable);
  new_fault->scale_factor = ConvertString::convert<T>(scale_string);

  return new_fault;
}


/*******************************************************************************
make_fault_overwrite
Purpose:(Constructs an overwrite fault.)
*******************************************************************************/
template<typename T> Fault* FaultManager::make_fault_overwrite(
  xmlNodePtr  fault_node,
  T&          variable,
  const char* fault_name)
{
  bool random_value = false;
  bool overwrite_value = false;

  // Need either a correctly configured Overwrite child node (specifying the
  // overwrite value) or a RandValue child node (specifying the random number
  // distribution parameters for a random overwrite). Either is valid.
  // Look for the Overwrite node first:
  xmlNodePtr overwrite_node = XmlHelper::xml_find_child(fault_node, "Overwrite");

  // If found the Overwrite-node, extract the Overwrite-value from it.
  const char* overwrite_string = nullptr;
  if (overwrite_node != nullptr) {
    overwrite_string = XmlHelper::xml_find_value(overwrite_node, "value");
  }
  // Identify whether we got a configured Overwrite-value
  overwrite_value = (overwrite_string!=nullptr);

  // Now check for the RandValue node.
  xmlNodePtr rand_node = XmlHelper::xml_find_child(fault_node, "RandValue");
  // If we found one, try parsing the random values. If successful, we have a
  // successfully configured random value.
  if (rand_node) {
    random_value = parse_rand_number(rand, rand_node, fault_name);
  }

  // If neither was succesful, the Fault fails configuration
  if (!overwrite_value && !random_value) {
    std::string msg;
    if (overwrite_node) {
      msg = "The Overwrite node exists but is incomplete.\n";
    } else {
      msg = "The Overwrite node does not exist.\n";
    }
    if (rand_node) {
      msg += "The RandValue node exists but is incomplete.";
    } else {
      msg += "The RandValue node does not exist.";
    }
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The Fault <", fault_name, "> requires an overwrite value specified in either an\n"
      "Overwrite node or a RandValue node\n",
       msg, "\n"
      "This Fault will be ignored.\n");
    return nullptr;
  }
  if (overwrite_value && random_value) {
    CMLMessage::warn(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The Fault <", fault_name, "> requires an overwrite value specified in either an\n"
      "Overwrite node or a RandValue node\n"
      "This Fault has both fully specified.\n"
      "The specified value in the Overwrite node takes precedence.\n");
    random_value = false;
  }

  FaultOverwrite<T>* new_fault = new FaultOverwrite<T>(variable);

  new_fault->faulted_value = (random_value) ?
                             generate_random_value<T>() :
                             ConvertString::convert<T>(overwrite_string);

  return new_fault;
}


/*****************************************************************************
Name: generate_random_value
Purpose:
  A template method to generate a random value of type T. Purpose is to
  support specialization where rand.get_random_number() will not work
  as a standalone form.
*****************************************************************************/
template<>
bool FaultManager::generate_random_value<bool>()
{
  CMLMessage::error( __FILE__, __LINE__,
    "Random number generation returns a value of type double.\n"
    "Casting that to a boolean is problematic without knowing the "
    "distribution parameters.\n"
    "Random boolean represents whether the random number > 0.\n");
  return (rand.get_random_number() > 0);
}


/*******************************************************************************
populate_fault_function
Purpose:(Populates a function fault. Called from make_fault, from parse_fault.)
*******************************************************************************/
bool FaultManager::populate_fault_function(
  FaultFunctionBase* new_fault,
  xmlNodePtr         fault_node,
  const char*        fault_name)
{
  // Check for existence of the Function node.
  xmlNodePtr function_node = XmlHelper::xml_find_child(fault_node, "Function");
  if (function_node == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> is declared as type FUNCTION but the configuration\n"
      "has no Function node.\n"
      "This is a required configuration.\n"
      "This fault will be ignored.\n");
    return false;
  }

  // Check for population of the Function node
  // Test 1: is type specified and valid
  const char* function_type_string = XmlHelper::xml_find_value( function_node,
                                                                "type",
                                                                true);
  if (function_type_string == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> is declared as type FUNCTION but the configuration\n"
      "of the accompanying Function node does not include a \"type\" "
      "specification (e.g. type=\"LINEAR\").\n"
      "This is a required configuration.\n"
      "This fault will be ignored.\n");
    return false;
  }
  // Use the is_periodic_function flag to separate LINEAR from
  // SINE/SQUARE/TRIANGLE in later processing.
  bool is_periodic_function = true;
  if (strcmp(function_type_string, "LINEAR") == 0) {
    is_periodic_function = false;
    new_fault->type = FaultFunctionBase::Linear;
  }
  else if (strcmp(function_type_string, "SINEWAVE") == 0) {
    new_fault->type = FaultFunctionBase::Sinewave;
  }
  else if (strcmp(function_type_string, "SQUAREWAVE") == 0) {
    new_fault->type = FaultFunctionBase::Squarewave;
  }
  else if (strcmp(function_type_string, "TRIANGLEWAVE") == 0) {
    new_fault->type = FaultFunctionBase::Trianglewave;
  }
  else {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The Fault <", fault_name, "> is declared as type FUNCTION but the configuration\n"
      "of the accompanying Function node has an invalid function type <", function_type_string, ">."
      "\nThis is a required configuration.\n"
      "This fault will be ignored.\n");
    return false;
  }


  // Test 2: is independent variable specified and valid
  xmlNodePtr variable_node =
    XmlHelper::xml_find_child(function_node, "IndVariable");
  if (variable_node == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The fault <", fault_name, "> is declared as type FUNCTION but the configuration\n"
      "of the accompanying Function node does not include a \"IndVariable\" "
      "child-node (i.e. the independent variable location).\n"
      "This is a required configuration.\n"
      "This fault will be ignored.\n");
    return false;
  }
  // Check for population of the IndVariable node
  if (!parse_ind_variable(new_fault->ind_variable, variable_node, fault_name)) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing fault configuration\n",
      "In fault <", fault_name, ">, the IndVariable node could not be fully processed\n"
      "(see error message immediately preceding)\n"
      "The Fault, being a function-fault, requires an independent variable.\n"
      "This Fault will be ignored.\n");
    return false;
  }

  if (is_periodic_function) {
  // Process the SINEWAVE/SQUAREWAVE/TRIANGLEWAVE specifications of periodicity
  // Amplitude and Frequency are required; PhaseOffset is optional.
    if (!parse_periodic_param( new_fault->amplitude,
                               function_node,
                               "Amplitude",
                               variable_node,
                               fault_name,
                               true)) {
      return false;
    }
    if (!parse_periodic_param( new_fault->frequency,
                               function_node,
                               "Frequency",
                               variable_node,
                               fault_name,
                               true)) {
      return false;
    }
    // parse "Phase Offset" but pass "false" as the 6th argument to
    // indicate that this is not a required parameter. Regardless of the
    // outcome, return true upon return to indicate that the function
    // parameters are completely parsed.
    parse_periodic_param( new_fault->phase_offset,
                          function_node,
                          "PhaseOffset",
                          variable_node,
                          fault_name,
                          false);
    return true;
  }
  // else, working with non-periodic functions (i.e. LINEAR)
  return parse_non_periodic_param( *new_fault,
                                   function_node,
                                   fault_name);
}


/*******************************************************************************
parse_ind_variable
Purpose:(Parses an independent-variable node.)
*******************************************************************************/
bool FaultManager::parse_ind_variable(
  FaultFunctionIndependentVariable& ind_variable,
  xmlNodePtr                        variable_node,
  const char*                       fault_name)
{
  const char* variable_name = XmlHelper::xml_find_value(variable_node, "name");
  if (variable_name == nullptr) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing fault configuration\n",
      "In fault <", fault_name, ">, the IndVariable node is missing the specification\n"
      "of the variable name (\'<IndVariable name=\"...\">\').\n"
      "This is a required configuration.\n"
      "This Independent variable cannot be used as configured.\n");
    return false;
  }
  REF2* Symbol = get_trick_variable(variable_name);
  if (Symbol == nullptr) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing fault configuration\n",
      "In fault <", fault_name, ">, the IndVariable node includes a specified variable\n"
      "name but that name <", variable_name, "> cannot be found in the simulation.\n"
      "This is a required configuration.\n"
      "This Independent variable cannot be used as configured.\n");
    return false;
  }

  switch (Symbol->attr->type) {
    case TRICK_CHARACTER:
      ind_variable.initialize<char>(*static_cast<char*>(Symbol->address));
      break;
    case TRICK_UNSIGNED_CHARACTER:
      ind_variable.initialize<unsigned char>(
        *static_cast<unsigned char*>(Symbol->address));
      break;
    case TRICK_SHORT:
      ind_variable.initialize<short>(*static_cast<short*>(Symbol->address));
      break;
    case TRICK_UNSIGNED_SHORT:
      ind_variable.initialize<unsigned short>(
        *static_cast<unsigned short*>(Symbol->address));
      break;
    case TRICK_INTEGER:
      ind_variable.initialize<int>(*static_cast<int*>(Symbol->address));
      break;
    case TRICK_UNSIGNED_INTEGER:
      ind_variable.initialize<unsigned int>(
        *static_cast<unsigned int*>(Symbol->address));
      break;
    case TRICK_LONG:
      ind_variable.initialize<long>(*static_cast<long*>(Symbol->address));
      break;
    case TRICK_UNSIGNED_LONG:
      ind_variable.initialize<unsigned long>(
        *static_cast<unsigned long*>(Symbol->address));
      break;
    case TRICK_FLOAT:
      ind_variable.initialize<float>(*static_cast<float*>(Symbol->address));
      break;
    case TRICK_DOUBLE:
      ind_variable.initialize<double>(*static_cast<double*>(Symbol->address));
      break;
    case TRICK_LONG_LONG:
      ind_variable.initialize<long long>(*static_cast<long long*>(Symbol->address));
      break;
    case TRICK_UNSIGNED_LONG_LONG:
      ind_variable.initialize<unsigned long long>(
        *static_cast<unsigned long long*>(Symbol->address));
      break;
    default:
      CMLMessage::error(__FILE__, __LINE__,
        "XML input error parsing fault configuration\n",
        "In fault <", fault_name, ">, the IndVariable node includes a specified variable\n"
        "but that variable <", variable_name, "> is not of a valid type for an independent "
        "variable.\n"
        "This is a required configuration.\n"
        "This Independent variable cannot be used as configured.\n");
      return false;
  }

  // Optional: specify whether independent variable is relative to its initial
  // value or absolute.
  const char* relative_string =
    XmlHelper::xml_find_value(variable_node, "relative");
  if (relative_string != nullptr) {
    ind_variable.relative_value = ConvertString::convert<bool>(relative_string);
  }

  return true;
}


/*******************************************************************************
parse_periodic_param
Purpose:(Parses a Frequency, Amplitude, or PhaseOffset node.)
*******************************************************************************/
bool FaultManager::parse_periodic_param(
  FaultFunctionParameter & param,
  xmlNodePtr               function_node,
  const char *             param_name,
  xmlNodePtr               ind_var_node,
  const char *             fault_name,
  bool                     nom_required)
{
  xmlNodePtr param_node = XmlHelper::xml_find_child( function_node,
                                          param_name);
  if (param_node == nullptr) {
    if (nom_required) {
      CMLMessage::error(__FILE__,__LINE__,
        "XML input error parsing fault configuration\n",
        "The Fault: <", fault_name, "> was defined as a periodic function type\n"
        "but it has no specified ", param_name, " node.\n"
        "This is a required configuration for periodic functions.\n"
        "This Fault will be ignored.\n");
    }
    return false;
  }

  const char* value_string = XmlHelper::xml_find_value(param_node, "nominal");
  if (value_string == nullptr) {
    if (nom_required) { // did not find it, and need it,
      CMLMessage::error(__FILE__,__LINE__,
        "XML input error parsing fault configuration\n",
        "The Fault <", fault_name, "> was defined as a periodic function type but\n"
        "its ", param_name, " node does not have a specified nominal value.\n"
        "  <", param_name, " nominal=\"...\" ...>\n"
        "This is a required configuration for periodic functions.\n"
        "This Fault will be ignored.\n");
    }
    return false;
  }
  // else translate value_string to param.nominal
  param.nominal = strtod(value_string, nullptr);

  // Optional: make the parameter a linear function of some independent
  // variable.
  value_string = XmlHelper::xml_find_value(param_node, "rate");
  xmlNodePtr variable_node =
    XmlHelper::xml_find_child(param_node, "IndVariable");
  if (value_string == nullptr) {
    // No rate specified.
    if (variable_node != nullptr) {
      // But there is an Independent Variable specified, which is indicative of
      // a misconfiguration..
      CMLMessage::error(__FILE__,__LINE__,
        "XML input error parsing fault configuration\n",
        "The Fault <", fault_name, "> is configured as a periodic function, with the\n",
         param_name, " parameter assigned an independent variable node but not a rate.\n"
        "The ", param_name, " parameter will be treated as constant, and held at the\n"
        "value specified with the \"nominal\" keyword (", param.nominal, ").\n"
        "This may not be intended behavior, proceed with caution.\n");
    }
    // else, (i.e. neither specified), take no action. This parameter is
    // intended to be constant.
  }
  else { // rate is specified
    param.rate = strtod(value_string, nullptr);
    // Default to using the same independent variable as the fault's main
    // function if no IndVariable specified.
    if (variable_node == nullptr) {
      variable_node = ind_var_node;
    }
    // Parse the independent variable specifications.
    if (!parse_ind_variable( param.ind_variable,
                             variable_node,
                             fault_name)) {
      CMLMessage::error(__FILE__,__LINE__,
        "XML input error parsing fault configuration\n",
        "The Fault <", fault_name, "> is configured as a periodic function, with the\n",
         param_name, " parameter assigned a rate, indicating that it is to be treated "
        "as a function.\n"
        "However, the configuration of the Independent Variable is\n"
        "incomplete (see error message preceding).\n"
        "Without an independent variable, the ", param_name, " parameter will be treated\n"
        "as constant, and held at the value specified with the \"nominal\" "
        "keyword (", param.nominal, ").\n"
        "This may not be intended behavior, proceed with caution.\n");
    }
  }
  return true;
}

/*******************************************************************************
parse_nom_periodic_param
Purpose:(Extract the rate and nominal values from the Parameters node)
*******************************************************************************/
bool FaultManager::parse_non_periodic_param(
  FaultFunctionParameter & params,
  xmlNodePtr               function_node,
  const char *             fault_name)
{
  xmlNodePtr params_node =
      XmlHelper::xml_find_child(function_node, "Parameters");
  if (params_node == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The Fault <", fault_name, "> was defined as a Linear FUNCTION type\n"
      "but is missing the Parameters subnode of the Function node.\n"
      "This is a required configuration.\n"
      "This fault will be ignored.\n");
    return false;
  }
  const char* temp_string = XmlHelper::xml_find_value(params_node, "rate");
  if (temp_string == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The Fault <", fault_name, "> was defined as a Linear FUNCTION type but the\n"
      "Parameters subnode of the Function node is missing specification of\n"
      "the rate (\'<Parameters rate = \"...\"...>\').\n"
      "This is a required configuration.\n"
      "This fault will be ignored.\n");
    return false;
  }
  params.rate = strtod(temp_string, nullptr);
  // "initial" and "nominal" are synonymous
  // If both are specified, "nominal" takes precedence.
  // If neither is specified, it defaults to 0 without comment.
  temp_string = XmlHelper::xml_find_value(params_node, "nominal");
  if (temp_string == nullptr) {
    temp_string = XmlHelper::xml_find_value(params_node, "initial");
  }
  if (temp_string == nullptr) {
    params.nominal = 0;
  } else {
    params.nominal = strtod(temp_string, nullptr);
  }
  return true;
}

/*******************************************************************************
make_fault_white_noise
Purpose:(Constructs a white-noise fault.)
*******************************************************************************/
template<typename T> Fault* FaultManager::make_fault_white_noise(
  xmlNodePtr  fault_node,
  T&          variable,
  const char* fault_name)
{
  xmlNodePtr rand_node = XmlHelper::xml_find_child(fault_node, "RandValue");
  if (rand_node == nullptr) {
    // to support legacy FaultArch pattern, random parameters may have been
    // specified in the Params node. This pattern has been deprecated but is
    // supported for legacy compatibility.
    // TODO Turner 2022/09: Remove deprecated content, leave the error
    //      message unguarded by the additional if which can be removed.
    rand_node = check_rand_in_params( fault_node);
    if (rand_node == nullptr) {
      CMLMessage::error(
         __FILE__,__LINE__,"XML input error parsing fault configuration\n",
        "The Fault <", fault_name, "> is configured as a WHITENOISE Fault\n"
        "but is missing a RandValue node to specify the random distribution "
        "characteristics\n"
        "This is required for a WHITENOISE Fault.\n"
        "This Fault will be ignored.\n");
      return nullptr;
    }
  }

  FaultWhiteNoise<T>* new_fault = new FaultWhiteNoise<T>(variable);
  // Parse the RandValue node and populate rand. If this fails, delete the
  // fault and return nullptr.
  if (!parse_rand_number(new_fault->noise, rand_node, fault_name)) {
    delete new_fault;
    CMLMessage::error(
       __FILE__,__LINE__,"XML input error parsing fault configuration\n",
      "The Fault <", fault_name, "> is configured as a WHITENOISE Fault\n"
      "but the RandValue node is incomplete\n"
      "(see error mesage immediately preceding)\n"
      "A configured random variable is required for a WHITENOISE Fault.\n"
      "This Fault will be ignored.\n");
    return nullptr;
  }
  return new_fault;
}


/*******************************************************************************
make_fault_random_walk
Purpose:(Constructs a random-walk fault.)
*******************************************************************************/
template<typename T> Fault* FaultManager::make_fault_random_walk(
  xmlNodePtr  fault_node,
  T&          variable,
  const char* fault_name)
{
  xmlNodePtr rand_node = XmlHelper::xml_find_child(fault_node, "RandValue");
  if (rand_node == nullptr) {
    // to support legacy FaultArch pattern, random parameters may have been
    // specified in the Params node. This pattern has been deprecated but is
    // supported for legacy compatibility.
    // TODO Turner 2022/09: Remove deprecated content, leave the error
    //      message unguarded by the additional if which can be removed.
    rand_node = check_rand_in_params( fault_node);
    if (rand_node == nullptr) {
      CMLMessage::error(__FILE__,__LINE__,
        "XML input error parsing fault configuration\n",
        "The Fault <", fault_name, "> is configured as a RANDOMWALK Fault\n"
        "but is missing a RandValue node to specify the random distribution "
        "characteristics\n"
        "This is required for a RANDOMWALK Fault.\n"
        "This Fault will be ignored.\n");
      return nullptr;
    }
  }

  FaultRandomWalk<T>* new_fault = new FaultRandomWalk<T>(variable);
  // Parse the RandValue node and populate rand. If this fails, delete the
  // fault and return nullptr.
  if (!parse_rand_number(new_fault->rand, rand_node, fault_name)) {
    delete new_fault;
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The Fault <", fault_name, "> is configured as a RANDOMWALK Fault\n"
      "but the RandValue node is incomplete\n"
      "(see error mesage immediately preceding)\n"
      "A configured random variable is required for a RANDOMWALK Fault.\n"
      "This Fault will be ignored.\n");
    return nullptr;
  }

  return new_fault;
}


/*******************************************************************************
parse_trigger_group
Purpose:(Parses a trigger-group node.)
*******************************************************************************/
TriggerGroup* FaultManager::parse_trigger_group(
  xmlNodePtr  trigger_group_node,
  const char* fault_name)
{
  TriggerGroup* new_trigger_group = new TriggerGroup;

  bool empty_group = true;
  for (xmlNodePtr trigger_node = trigger_group_node->children;
       trigger_node != nullptr;
       trigger_node = trigger_node->next)
  {
    if (XmlHelper::xml_name_match(trigger_node, "Trigger")) {
      // If the Trigger has a reuse_name pull it from the trigger library
      const char* reuse_name =
        XmlHelper::xml_find_value(trigger_node, "reuse_name");
      if (reuse_name != nullptr) {
        TriggerBase* temp_ptr = get_trigger(reuse_name);
        if (temp_ptr == nullptr) {
          CMLMessage::error(__FILE__, __LINE__,
            "XML input error parsing trigger group contents.\n",
            "The trigger <", reuse_name, "> listed for fault <", fault_name, ">\n"
            "is listed as re-used but no existing trigger with that name can "
            "be found.\n");
        } else {
          empty_group = false;
          new_trigger_group->add_trigger(*temp_ptr);
        }
      } else {
        TriggerBase* new_trigger = parse_trigger(trigger_node, fault_name);
        if (new_trigger != nullptr) {
          empty_group = false;
          new_trigger_group->add_trigger(*new_trigger);
        }
      }
    }
  }

  if (empty_group) {
    delete new_trigger_group;
    return nullptr;
  } else {
    trigger_groups.push_back(new_trigger_group);
    return new_trigger_group;
  }
}


/*******************************************************************************
parse_trigger
Purpose:(Parses a trigger node.)
*******************************************************************************/
TriggerBase* FaultManager::parse_trigger(
  xmlNodePtr  trigger_node,
  const char* fault_name)
{
  const char* name_string = XmlHelper::xml_find_value(trigger_node, "name");
  if (name_string == nullptr) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing trigger name\n",
      "A trigger for fault <", fault_name, "> was defined without a name.\n"
      "This is not allowed.\n"
      "Trigger has not been added to the set of triggers.\n");
    return nullptr;
  }

  // Check for a trigger of the same name already existing
  TriggerBase * duplicate_trigger = get_trigger(name_string);
  if (duplicate_trigger != nullptr) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing trigger name\n",
      "A trigger with the name <", name_string, "> has already been defined.\n"
      "Duplicate instance found under fault <", fault_name, ">\n"
      "Duplicate names are permitted, but avoidance is strongly recommended.\n"
      "Setting parameters for a trigger with this name may not function as\n"
      "intended because the name-lookup may select the wrong trigger.\n"
      "Adding this trigger with a duplicated name to the set of triggers.\n"
      "Proceed with caution.\n");
  }

  // Pull the variable name from the trigger node
  const char* var_name = XmlHelper::xml_find_value(trigger_node, "variable");
  if (var_name == nullptr) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing trigger variable name\n",
      "A trigger with the name <", name_string, "> (under fault <", fault_name, ">)\n"
      "was defined without naming a trigger variable.\n"
      "This is not allowed.\n"
      "Trigger has not been added to the set of triggers.\n");
    return nullptr;
  }
  REF2* Symbol = get_trick_variable(var_name);
  if (Symbol == nullptr) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing trigger variable\n",
      "A trigger with the name <", name_string, "> (under fault <", fault_name, ">)\n"
      "was defined with an invalid trigger variable\n"
      "(see error message immediately preceding for details).\n"
      "This is not allowed.\n"
      "Trigger has not been added to the set of triggers.\n");
    return nullptr;
  }

  // Pull the comparison type
  const char* comp_str = XmlHelper::xml_find_value(trigger_node, "compare");
  if (comp_str == nullptr) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing trigger comparator\n",
      "A trigger with the name <", name_string, "> (under fault <", fault_name, ">)\n"
      "was defined without a comparison type.\n"
      "This is not allowed.\n"
      "Trigger has not been added to the set of triggers.\n");
    return nullptr;
  }
  TriggerBase::Operator_enm comp = TriggerBase::translate_operator(comp_str);
  if (comp == TriggerBase::Invalid) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing trigger comparator\n",
      "A trigger with the name <", name_string, "> (under fault <", fault_name, ">)\n"
      "was defined with an invalid comparison type of <", comp_str, "\".\n"
      "Valid values are:\n"
      "  * LE (less than or equal to)\n"
      "  * LT (less than)\n"
      "  * EQ (equal to)\n"
      "  * GE (greater than or equal to)\n"
      "  * GT (greater than)\n"
      "  * NE (not equal to)\n"
      "This is not allowed.\n"
      "Trigger has not been added to the set of triggers.\n");
    return nullptr;
  }

  // Pull the comparison value. We may not have one.
  // If we don't, check for a random value set.
  const char* value = XmlHelper::xml_find_value(trigger_node, "value");
  xmlNodePtr rand_node = nullptr;
  if (value == nullptr) {
    rand_node = XmlHelper::xml_find_child(trigger_node, "RandValue");
    // If there is also no RandValue XML child-node, the trigger is incomplete.
    if (rand_node == nullptr) {
      CMLMessage::error(__FILE__, __LINE__,
        "XML input error parsing trigger comparison.\n",
        "A trigger with the name <", name_string, "> (under fault <", fault_name, ">)\n"
        "was defined without a valid comparison value or random value setup.\n"
        "This is not allowed.\n"
        "Trigger has not been added to the set of triggers.\n");
      return nullptr;
    }
    // If there is a RandValue XML child-node, parse it; if it is incomplete,
    // then the trigger is also incomplete.
    if (!parse_rand_number(rand, rand_node, fault_name)) {
      CMLMessage::error(__FILE__, __LINE__,
        "XML input error parsing trigger random config.\n",
        "A trigger with the name <", name_string, "> (under fault <", fault_name, ">)\n"
        "was defined to use a random trigger value but has an incomplete\n"
        "random value setup (see error message immediately preceding).\n"
        "This is not allowed.\n"
        "Trigger has not been added to the set of triggers.\n");
      return nullptr;
    }
    // else, we have a trigger successfully configured with a random trigger
    // value.
  }

  TriggerBase* new_trigger = nullptr;
  switch (Symbol->attr->type) {
    case TRICK_CHARACTER:
      new_trigger = make_trigger<char>(
        *static_cast<char*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_UNSIGNED_CHARACTER:
      new_trigger = make_trigger<unsigned char>(
        *static_cast<unsigned char*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_STRING: {
      // Note -- not using make_trigger because we do not want to be trying to
      // convert a string via the ConvertString algorithm which is used in
      // make_trigger.
      Trigger<std::string>* string_trigger =
        new Trigger<std::string>(*static_cast<std::string*>(Symbol->address));
      string_trigger->set_value(value);
      new_trigger = string_trigger;
      break;
    }
    case TRICK_SHORT:
      new_trigger = make_trigger<short>(
        *static_cast<short*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_UNSIGNED_SHORT:
      new_trigger = make_trigger<unsigned short>(
        *static_cast<unsigned short*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_INTEGER:
      new_trigger = make_trigger<int>(
        *static_cast<int*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_UNSIGNED_INTEGER:
      new_trigger = make_trigger<unsigned int>(
        *static_cast<unsigned int*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_LONG:
      new_trigger = make_trigger<long>(
        *static_cast<long*>(Symbol->address), trigger_node, value);
       break;
    case TRICK_UNSIGNED_LONG:
      new_trigger = make_trigger<unsigned long>(
        *static_cast<unsigned long*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_FLOAT:
      new_trigger = make_trigger<float>(
        *static_cast<float*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_DOUBLE:
      new_trigger = make_trigger<double>(
        *static_cast<double*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_LONG_LONG:
      new_trigger = make_trigger<long long>(
        *static_cast<long long*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_UNSIGNED_LONG_LONG:
      new_trigger = make_trigger<unsigned long long>(
        *static_cast<unsigned long long*>(Symbol->address), trigger_node, value);
      break;
    case TRICK_BOOLEAN:
    {
      // Note -- not using make_trigger because we do not want to be trying to
      // convert a set boolean periods etc. as used in make_trigger.
      Trigger<bool>* bool_trigger =
        new Trigger<bool>(*static_cast<bool*>(Symbol->address));
      bool_trigger->set_value(ConvertString::convert<bool>(value));
      new_trigger = bool_trigger;
      break;
    }
    default:
      CMLMessage::error(__FILE__, __LINE__,
        "XML input error parsing trigger variable (data type).\n",
        "A trigger with the name <", name_string, "> (under fault <", fault_name, ">)\n"
        "was defined with an invalid comparison variable <", var_name, ">.\n"
        "The data type of the comparison variable <", var_name, "> is not recognized\n"
        "or not supported.\n"
        "This is not allowed.\n"
        "Trigger has not been added to the set of triggers.\n");
  }
  // if creation failed, return empty
  // Note -- intended as a catch-all, with the only truly anticipated path being
  // the one through the default: option of the switch above.
  if (new_trigger == nullptr) {
    return nullptr;
  }

  // assign values to  new trigger.
  new_trigger->name.assign(name_string);
  new_trigger->Operator = comp;

  // if this trigger has a random-node, use the defined distribution to generate
  // the random value. Note that this will override any value previously
  // assigned, although in most cases a fault would not be given both a value
  // and a random node because they are mutually exclusive.
  // Also note that in the case of a STL-string template, the
  // new_trigger->set_value(double) method is empty.
  if (rand_node != nullptr) {
    new_trigger->set_value(rand.get_random_number());
  }

  // Check for the presence of "fire_limit" as a field in the definition of
  // the Trigger; this limits the number of times a trigger can be triggered.
  const char* fire_limit_str = XmlHelper::xml_find_value( trigger_node,
                                                          "fire_limit");
  if (fire_limit_str != nullptr) {
    new_trigger->set_trigger_count( strtoul( fire_limit_str, nullptr, 10));
  }


  triggers.push_back(new_trigger);
  return new_trigger;
}


/*******************************************************************************
make_trigger
Purpose:(Constructs a trigger and check it for assigned periodicity.)

Note -- setting periodicity is a method of Trigger<T>, not of TriggerBase;
if we send this back to FaultManager::parse_trigger(...), we lose information
on T. Although this is sort of a stitched-together combination of two processes,
it is easier to do it this way than to recover T from the TriggerBase
base-class pointer that is returned to parse_trigger(...)
*******************************************************************************/
template<typename T> TriggerBase* FaultManager::make_trigger(
  T&          variable,
  xmlNodePtr  trigger_node,
  const char* value)
{

  //*****************************************************
  //Make the new trigger                                *
  //*****************************************************
  Trigger<T>* new_trigger = new Trigger<T>(variable);
  if (value != nullptr) {
    new_trigger->value = ConvertString::convert<T>(value);
  }

  //*****************************************************
  // Asssign its periodicity (if required to do so)     *
  //*****************************************************

  // Parameters for periodic triggers
  const char* trigger_period_string =
    XmlHelper::xml_find_value(trigger_node, "trigger_period");
  const char* trigger_length_string =
    XmlHelper::xml_find_value(trigger_node, "trigger_length");
  // If both are specified, let's try assigning them
  if (trigger_period_string != nullptr && trigger_length_string != nullptr) {
    T trigger_length = ConvertString::convert<T>(trigger_length_string);
    T trigger_period = ConvertString::convert<T>(trigger_period_string);
    new_trigger->set_periodic(trigger_length, trigger_period);
    // If periodic settings failed, drop a message.
    if (!new_trigger->get_periodic()) {
      CMLMessage::error(__FILE__, __LINE__,
        "XML input error parsing trigger periodicity.\n",
        "A trigger with the name <", XmlHelper::xml_find_value(trigger_node, "name"), ">\n"
        "was assigned values for periodic parameters:\n"
        " * trigger_period and\n"
        " * trigger_length.\n"
        "One or both of these values were not configured with valid values\n"
        "(see error message(s) preceding) and the\n"
        "trigger has consequently NOT been assigned periodic features.\n"
        "This trigger will be added to the set of available triggers, but\n"
        "as a non-periodic trigger.\n"
        "This may result in unexpected behavior.\nProceed with caution.\n");
    }
  }
  // If only 1 setting was made, the configuration is incomplete.
  else if (trigger_period_string != nullptr ||
           trigger_length_string != nullptr ) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing trigger periodicity.\n",
      "A trigger with the name <", XmlHelper::xml_find_value(trigger_node, "name"), ">\n"
      "was assigned values for only 1 of:\n"
      " * trigger_period and\n"
      " * trigger_length.\n"
      "These values are both required to make the trigger periodic.\n"
      "This trigger will be added to the set of available triggers, but\n"
      "as a non-periodic trigger.\n"
      "This may result in unexpected behavior.\nProceed with caution.\n");
  }
  return new_trigger;
}


/*******************************************************************************
parse_rand_number
Purpose:(Parses a random-variable node.)
*******************************************************************************/
bool FaultManager::parse_rand_number(
  FaultRandNumber & rng,
  xmlNodePtr        rand_node,
  const char*       fault_name)
{
  const char* temp_str = XmlHelper::xml_find_value(rand_node, "distribution");
  if (temp_str == nullptr) {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing randomized configuration.\n",
      "A random variable in Fault <", fault_name, "> was specified\n"
      "without a distribution specification.\n"
      "This is not supported.\n"
      "Configuration of random value failed.\n");
    return false;
  }
  if (strcmp(temp_str, "GAUSSIAN") == 0) {
    rng.distribution_type = FaultRandNumber::GAUSSIAN;
  } else if (strcmp(temp_str, "FLAT") == 0) {
    rng.distribution_type = FaultRandNumber::FLAT;
  } else {
    CMLMessage::error(__FILE__, __LINE__,
      "XML input error parsing randomized configuration.\n",
      "A random variable in Fault <", fault_name, "> was specified\n"
      "with an invalid distribution specification <", temp_str, "\".\n"
      "This is not supported.\n"
      "Configuration of random value failed.\n");
    return false;
  }


  // if distribution_type is GAUSSIAN, we must have standard-deviation
  // specified; mean is optional, it defaults to 0.0 if not specified.
  if ( rng.distribution_type == FaultRandNumber::GAUSSIAN) {
    temp_str = XmlHelper::xml_find_value(rand_node, "std_dev");
    if (temp_str != nullptr) {
      rng.std_dev = strtod(temp_str, nullptr);
    } else {
      CMLMessage::error( __FILE__, __LINE__,
        "XML input error parsing randomized configuration.\n",
        "A Gaussian / Normal random variable in fault <", fault_name, "> was specified\n"
        "without a standard deviation value.\n"
        "Standard deviation is required for a Gaussian distribution.\n"
        "Configuration of random value failed.\n");
      return false;
    }

    temp_str = XmlHelper::xml_find_value(rand_node, "mean");
    if ( temp_str != nullptr) {
      rng.mean = strtod(temp_str, nullptr);
    } else {
      CMLMessage::warn(__FILE__, __LINE__,
        "XML input missing while parsing randomized "
        "configuration.\n",
        "A Gaussian / Normal random variable in Fault <", fault_name, "> was specified\n"
        "without specifying the mean.\n"
        "Mean defaults to 0.0 in this situation.\n");
      rng.mean = 0.0;
    }
  }

  // For FLAT distribution, can have either (min,max), or
  // (rel_min, mean, rel_max).
  else if ( rng.distribution_type == FaultRandNumber::FLAT) {
    const char* min_str = XmlHelper::xml_find_value(rand_node, "min");
    const char* max_str = XmlHelper::xml_find_value(rand_node, "max");
    // if min and max found, use them:
    if (min_str != nullptr &&
        max_str != nullptr) {
      rng.lower_limit = strtod(min_str, nullptr);
      rng.upper_limit = strtod(max_str, nullptr);
    }

    // else, look for the (rel_min, mean, rel_max) option:
    else {
      min_str = XmlHelper::xml_find_value(rand_node, "rel_min");
      max_str = XmlHelper::xml_find_value(rand_node, "rel_max");
      temp_str = XmlHelper::xml_find_value(rand_node, "mean");
      if (min_str != nullptr &&
          max_str != nullptr &&
          temp_str != nullptr ) {
        double mean_val = strtod(temp_str, nullptr);
        rng.lower_limit = mean_val - std::abs(strtod(min_str, nullptr));
        rng.upper_limit = mean_val + strtod(max_str, nullptr);
      }
      else {
        CMLMessage::error(__FILE__, __LINE__,
          "XML input error parsing randomized "
          "configuration.\n",
          "A flat distribution random variable in fault <", fault_name, ">\n"
          "was specified without a complete range.\n"
          "Flat distribution requires a range specified as either:\n"
          " - a (min,max) pair, or\n"
          " - a (rel_min, mean, rel_max) triplet\n"
          "Neither set was provided\n"
          "Configuration of random value failed.\n");
        return false;
      }
    }
  }

  // Seed is optional. If one is specified pull it out and set.
  // Optional future work: Consider setting a flag to indicate that the seed
  //    has been set. Currently, if the seed is set to 0, that is equivalent
  //    to not being set at all and leads to a system-generated seed. If
  //    necessary, we could change this behavior to make 0 a legitimate assigned
  //    value and use a flag to indicate whether the seed has been set, rather
  //    than testing whether (seed==0).
  temp_str = XmlHelper::xml_find_value(rand_node, "seed");
  if (temp_str != nullptr) {
    rng.seed = strtol(temp_str, nullptr, 10);
  }

  return true;
}