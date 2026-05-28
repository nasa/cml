/*******************************TRICK HEADER************************************
PURPOSE: (Interface to the newer FaultManagement model to support backward
compatibility with models using the legacy fault_arch sensor-faults.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Mar 2022) (Antares) (Initial)))
*******************************************************************************/
#include "../include/sSensorFaults.hh"


/*******************************************************************************
Injection
Purpose:
  This is a named method in the old Fault-Arch, formerly the top-level call
  to inject a fault.
  This implementation translates the original call into the appropriate
  call to FaultManager::update(...)
*******************************************************************************/
void
sSensorFaults::Injection( sSFault loc)
{
  switch (loc) {
  case INIT:
    update(FaultManager::Location::Initialize);
    break;
  case UPSTREAM:
    update(FaultManager::Location::Upstream);
    break;
  case INTERMEDIATE_1:
    update(FaultManager::Location::Intermediate_1);
    break;
  case INTERMEDIATE_2:
    update(FaultManager::Location::Intermediate_2);
    break;
  case DOWNSTREAM:
    update(FaultManager::Location::Downstream);
    break;
  default:
    update(FaultManager::Location::INVALID);
    break;
  }
}


/*******************************************************************************
parse_non_periodic_param
Purpose:
 The parameters "rate" and "nominal" in the XML files for a FaultManager
 configuration are called "a" and "b" in the XML files for the legacy
 FaultArch.
 Here, we rewrite the FaultManager:: parse_non_periodic_param(...) method
 to account for this change.
*******************************************************************************/
bool
sSensorFaults::parse_non_periodic_param( FaultFunctionParameter& params,
                                         xmlNodePtr              function_node,
                                         const char*             fault_name)
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
  const char* temp_string = XmlHelper::xml_find_value(params_node, "a");
  if (temp_string == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The Fault <", fault_name, "> was defined as a Linear FUNCTION type but the\n"
      "Parameters subnode of the Function node is missing specification of\n"
      "the rate, a (\'<Parameters a = \"...\"...>\').\n"
      "This is a required configuration.\n"
      "This fault will be ignored.\n");
    return false;
  }
  params.rate = strtod(temp_string, nullptr);
  // If "b" is not specified, it defaults to 0 without comment.
  temp_string = XmlHelper::xml_find_value(params_node, "b");
  if (temp_string == nullptr) {
    params.nominal = 0;
  } else {
    params.nominal = strtod(temp_string, nullptr);
  }
  return true;
}


/*******************************************************************************
parse_periodic_param
Purpose:(Parses a Frequency, Amplitude, or PhaseOffset node.)
*******************************************************************************/
bool
sSensorFaults::parse_periodic_param( FaultFunctionParameter&  param,
                                     xmlNodePtr               function_node,
                                     const char*              param_name,
                                     xmlNodePtr               ind_var_node,
                                     const char*              fault_name,
                                     bool                     nom_required)
{
  // First check for a syntax compatible with FaultManager
  // Note that no parameter is "required" at this point, so pass "false" as
  // the last argument rather than nom_required. If FaultManager is not able
  // to configure the parameter, then sSensorFaults will try, at which point
  // the concept of nom_required returns to its passed meaning.
  bool new_syntax_valid = FaultManager::parse_periodic_param( param,
                                                              function_node,
                                                              param_name,
                                                              ind_var_node,
                                                              fault_name,
                                                              false);
  if (new_syntax_valid) {
    // All done, the XML file is consistent with FaultManager.
    return true;
  }

  // Otherwise, check for legacy syntax:

  // There is a clarification in one of the variable names, the old "phase"
  // has been changed to "PhaseOffset". So look for "phase" instead of
  // "PhaseOffset" in the old syntax.
  char phase[] = "Phase"; // old style
  const char * param_name_ = param_name;
  if (strcmp (param_name, "PhaseOffset") == 0) {
    param_name_ = phase;
  }

  // Check for a param_name field in the Parameters node;
  // if it is there (and does not have a dedicated node), treat this
  // parameter as a constant.
  xmlNodePtr param_node = XmlHelper::xml_find_child( function_node,
                                                     "Parameters");
  if (param_node == nullptr) {
    if (nom_required) {
      CMLMessage::error(__FILE__,__LINE__,
        "XML input error parsing fault configuration\n",
        "The Fault: <", fault_name, "> was defined as a periodic function type\n"
        "but it has no specified Parameters node for specifying the value\n"
        "of the ", param_name_, ".\n"
        "This is a required configuration for periodic functions.\n"
        "This Fault will be ignored.\n");
    }
    return false;
  }


  // Allow lower-case and upper-case on first character to be equivalent
  const char* value_string = XmlHelper::xml_find_value( param_node,
                                                        param_name_,
                                                        true);
  if (value_string == nullptr) {
    if (nom_required) {
      CMLMessage::error(__FILE__,__LINE__,
        "XML input error parsing fault configuration\n",
        "The Fault: <", fault_name, "> was defined as a periodic function type\n"
        "but it has no specified ", param_name_, " field in the Parameters node.\n"
        "This is a required configuration for periodic functions.\n"
        "This Fault will be ignored.\n");
    }
    return false;
  }

  // else
  param.nominal = strtod(value_string, nullptr);

  // Optional: make the parameter a linear function of some independent
  // variable.
  // Look for a node named Var<param_name>, e.g. VarAmplitude
  std::string var_param(param_name_);
  var_param.insert(0,"Var");
  xmlNodePtr var_node = XmlHelper::xml_find_child( function_node,
                                                   var_param.c_str());
  if (var_node == nullptr) {
    // No Var<param_name> node, this was optional anyway.
    return true;
  }

  value_string = XmlHelper::xml_find_value(var_node, "scalar");

  if (value_string == nullptr) {
    // No rate specified, no point in going on to look for an independent
    // variable.
    CMLMessage::error(__FILE__,__LINE__,
      "XML input error parsing fault configuration\n",
      "The Fault <", fault_name, "> is configured as a periodic function, with the\n",
      param_name_, " parameter assigned a Var<param_name> node but no \"scalar\" field.\n"
      "This would specify the linear rate of change. Without it the ", param_name_, "\n"
      "parameter will be constant, which means the node is redundant.\n"
      "Continuing with the assumption that ", param_name_, " is a constant ", param.nominal, ".\n"
      "This may not be intended behavior, proceed with caution.\n");
    return true;
  }
  param.rate = strtod(value_string, nullptr);

  // Now look for the independent variable.
  xmlNodePtr variable_node = XmlHelper::xml_find_child( var_node,
                                                        "IndVariable");
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
      param_name_, "s parameter assigned a Var node, indicating that it is to be treated\n"
      "as a linear function.\n"
      "However, the configuration of the Independent Variable is\n"
      "incomplete (see error message preceding).\n"
      "Without an independent variable, the ", param_name_, " parameter will be treated\n"
      "as constant, and held at the value specified in the Parameter node "
      "(", param.nominal, ").\n"
      "This may not be intended behavior, proceed with caution.\n");
  }
  return true;
}


/*******************************************************************************
check_rand_in_params
Purpose:
  Legacy FaultArch supported specification of random distribution
  parameters in a Params node; modern FaultManager requires these be in a
  RandValue node. This method provides the escape to read the Params values
  if necessary. This is only called if the RandValue node was not found.
*******************************************************************************/
xmlNodePtr
sSensorFaults::check_rand_in_params( xmlNodePtr fault_node)
{
  xmlNodePtr param_node = XmlHelper::xml_find_child( fault_node,
                                                     "Params");
  if (param_node != nullptr) {
    CMLMessage::warn(__FILE__,__LINE__,
      "XML input checking for deprecated pattern\n",
      "Fault Manager did not find a RandValue node, which is required to\n"
      "configure the random distribution somewhere in the fault system.\n"
      "Legacy (now deprecated) design allowed for these parameters to be\n"
      "placed in a Params node.\n"
      "Reassigning the identified Params node as though it were the desired\n"
      "RandValue node.\n"
      "If further messages from FaultManager indicate that parameters were\n"
      "not found in a RandValue node, they refer to this Params node.\n");
  }
  // If didn't find Params either, just keep going as though no RandValue
  // node was found and no further check was made.
  return param_node;
}


/*******************************************************************************
parse_rand_number
Purpose:
  This is a copy of the FaultManager function, except that if the
  distribution is not specified, it defaults to GAUSSIAN as it did in
  FaultArch.
*******************************************************************************/
bool sSensorFaults::parse_rand_number( FaultRandNumber&  rng,
                                       xmlNodePtr        rand_node,
                                       const char*       fault_name)
{
  const char* temp_str = XmlHelper::xml_find_value(rand_node, "distribution");
  // Assume GAUSSIAN
  // Note -- the FaultArch mechanism supported specification of random
  // parameters through a Params node for a random-walk Fault.
  // This option did not require (nor support) a "distribution" field
  // within the "Params" node, the distribution was required to be GAUSSIAN.
  // The FaultManagement implementation does not allow this syntax
  // and requires specification of a "RandValue" node, with a required
  // "distribution" field.
  // Thus we cannot use FaultManager::parse_random_number(...).
  rng.distribution_type = FaultRandNumber::GAUSSIAN;
  if (temp_str != nullptr &&
      strcmp(temp_str, "FLAT") == 0) {
    rng.distribution_type = FaultRandNumber::FLAT;
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
        "A Gaussian / Normal random variable in fault ", fault_name, " was specified\n"
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
        "A Gaussian / Normal random variable in Fault ", fault_name, " S was specified\n"
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

    // else, look for a the (rel_min, mean, rel_max) option:
    else {
      min_str  = XmlHelper::xml_find_value(rand_node, "rel_min");
      max_str  = XmlHelper::xml_find_value(rand_node, "rel_max");
      temp_str = XmlHelper::xml_find_value(rand_node, "mean");
      if (min_str  != nullptr &&
          max_str  != nullptr &&
          temp_str != nullptr ) {
        double mean_val = strtod(temp_str, nullptr);
        rng.lower_limit = mean_val - std::abs(strtod(min_str, nullptr));
        rng.upper_limit = mean_val + std::strtod(max_str, nullptr);
      }
      else {
        CMLMessage::error(__FILE__, __LINE__,
          "XML input error parsing randomized "
          "configuration.\n",
          "A flat distribution random variable in fault ", fault_name, "\n"
          " was specified without a complete range.\n"
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
  //   has been set. Currently, if the seed is set to 0, that is equivalent
  //   to not being set at all and leads to a system-generated seed. If
  //   necessary, we could change this behavior to make 0 a legitimate assigned
  //   value and use a flag to indicate whether the seed has been set, rather
  //   than testing whether (seed==0).
  temp_str = XmlHelper::xml_find_value(rand_node, "seed");
  if (temp_str != nullptr) {
    rng.seed = strtol(temp_str, nullptr, 10);
  }

  return true;
}