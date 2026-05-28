/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides a staging-ground for later creation of seria-variables and
  summary-variables. Provides the interface to the Trick reference-attributes
  mechanism for identifying variables and theiorr attributes just by name.
  Note -- This is a staging ground for logging-variables,
          IT IS NOT a base-class of logging-variables.
          It is referred to in the model documentation as a
          proto-logging-variable.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/

#include "../include/enhanced_logging_variable.hh"
#include "trick/attributes.h"

/*****************************************************************************
Constructor
*****************************************************************************/
EnhancedLogging_Variable::EnhancedLogging_Variable(
  std::string name_,
  std::string alias_,
  std::string units_)
  :
  name(  std::move(name_)),
  alias( std::move(alias_)),
  units( std::move(units_)),
  type(),
  var_ptr(nullptr),
  array_size(),
  serial_variable_list(),
  summary_variable_list(),
  trick_processed(false),
  serial_list_processed(false),
  summary_list_processed(false)
{
  if (alias.empty()) { alias = name;}
}

/****************************************************************************/
EnhancedLogging_VariableSet::EnhancedLogging_VariableSet()
{}
/****************************************************************************/
EnhancedLogging_VariableSet::EnhancedLogging_VariableSet(
  const std::list<EnhancedLogging_Variable> & var_list_)
  :
  var_list( var_list_),
  serial_variable_list(),
  summary_variable_list(),
  summary_list_processed(false),
  serial_list_processed(false)
{}

/*****************************************************************************
Destructor
*****************************************************************************/
EnhancedLogging_Variable::~EnhancedLogging_Variable()
{
  for (EnhancedLogging_SerialVariable* var: serial_variable_list) {
    delete var;
  }
  for (EnhancedLogging_SummaryVariable* var: summary_variable_list) {
    delete var;
  }
}

/*****************************************************************************
Name: get_serial_variable_list
Purpose:
  Returns the list of serial-logging-variables generated from this instance
  If that list has not yet been generated, calling this method will generate it.
*****************************************************************************/
std::vector<EnhancedLogging_SerialVariable*> &
EnhancedLogging_Variable::get_serial_variable_list()
{
  if (!trick_processed) {
    process_ref_attributes();
  }

  // trick_processed flag is set in process_ref_attributes, if that method
  // completes successfully, so check it again.
  if (trick_processed && !serial_list_processed) {
    generate_serial_variables();
  }

  return serial_variable_list;
}

/****************************************************************************/
std::vector<EnhancedLogging_SerialVariable*> &
EnhancedLogging_VariableSet::get_serial_variable_list()
{
  if ( !serial_list_processed) {
    if ( var_list.empty()) {
      CMLMessage::warn( __FILE__,__LINE__,
        "The var_list set has no entries, returning an empty set of variables."
        "\n");
    } else {
      for( EnhancedLogging_Variable & var : var_list) {
        std::vector<EnhancedLogging_SerialVariable*> & var_list_ =
                                               var.get_serial_variable_list();
        serial_variable_list.insert( serial_variable_list.end(),
                                     var_list_.begin(),
                                     var_list_.end());
      }
      serial_list_processed = true;
    }
  }
  return serial_variable_list;
}


/*****************************************************************************
Name: get_summary_variable_list
Purpose:
  Returns the list of summary-logging-variables generated from the specified
  name-alias combinations.
  If that list has not yet been generated, calling this method will generate it.
*****************************************************************************/
std::vector<EnhancedLogging_SummaryVariable*> &
EnhancedLogging_Variable::get_summary_variable_list()
{
  if (!trick_processed) {
    process_ref_attributes();
  }

  // trick_processed flag is set in process_ref_attributes, if that method
  // completes successfully, so check it again.
  if (trick_processed && !summary_list_processed) {
    generate_summary_variables();
  }

  return summary_variable_list;
}

/****************************************************************************/
std::vector<EnhancedLogging_SummaryVariable*> &
EnhancedLogging_VariableSet::get_summary_variable_list()
{
  if ( !summary_list_processed) {
    if ( var_list.empty()) {
      CMLMessage::warn( __FILE__,__LINE__,
        "The var_list set has no entries, returning an empty set of variables."
        "\n");
    } else {
      for( EnhancedLogging_Variable & var : var_list) {
        std::vector<EnhancedLogging_SummaryVariable*> & var_list_ =
                                               var.get_summary_variable_list();
        summary_variable_list.insert( summary_variable_list.end(),
                                      var_list_.begin(),
                                      var_list_.end());
      }
      summary_list_processed = true;
    }
  }
  return summary_variable_list;
}


/*****************************************************************************
Name: process_ref_attributes
Purpose:
  Uses Trick's ref-attributes to identify the nature of the variable
  identified by "name"
*****************************************************************************/
void
EnhancedLogging_Variable::process_ref_attributes()
{
  REF2 * ref = ref_attributes( name.c_str());
  if ((ref          == nullptr) ||
      (ref->attr    == nullptr) ||
      (ref->address == nullptr)) {
    CMLMessage::error(__FILE__,__LINE__,
      "Unable to find simulation variable ",name,
      ".\n Cannot create a new EnhancedLogging_SummaryVariableT or\n"
      "EnhancedLogging_SerialVariableT instance based on this name.\n");
    return;
  }

  /* Units are complicated than alias because they can come in from 2 sources:
   * the class member at construction (typically an override of "--") or the
   * ref-attributes.
   * - If the 2 units are identical, no action
   * - If the units provided to this class instance are empty, use the
   *   ref-attributes
   * - If there are units provided to this class instance, and ref-attributes
   *   units are generic "--", use the provided units.
   * - Otherwise, there is a conflict. Flag and use ref-attributes.*/
  std::string ref_units;
  if (ref->attr->mods & TRICK_MODS_UNITSDASHDASH ) {
    ref_units = "--";
  }
  else { 
    ref_units = ref->attr->units;
  }
  if (units != ref_units) {
    if (units.empty()) { units = ref_units; }
    else if (ref_units != "--") {
      CMLMessage::error( __FILE__,__LINE__,
        "In creating logging-variable ",name,":\n"
        "\nthe provided units  (",units,") and those extracted"
        "\nfrom ref-attributes (",ref_units,") are inconsistent."
        "\nProceeding with ref-attributes units (",ref_units,").\n");
      units = ref_units;
    }
    // else keep units provided at class construction
  }
  // else keep units provided at class construction

  /* ref->address is a void* pointer returned by the Trick simulation engine.
   * ref->attr->type is a string that identifies the data type of
   * ref->address. We can now cast the ref->address pointer to the data type
   * identified by ref->attr->type, and build a type-specific
   * implementation of the serial- or summary-variable template class:
   * - EnhancedLogging_SerialVariableT<T> or
   * - EnhancedLogging_SummaryVariableT<T>

   * We do not want to setup conditions for every enumerated type, so lump
   * these together and treat as an int.*/
  type = ref->attr->type_name;
  if (ref->attr->type == TRICK_ENUMERATED) {
    type = "int";
  }

  var_ptr = ref->address;

  /* If the variable specified is an array, we will typically want to log
     the entire thing. However, if a subset of the array is specified by
     index, we need to limit it to that subset.
     E.g.
       Suppose we have a variable foo[3][3]
       If name = "foo", this instance will represent all 9 elements of foo,
       allowing for the creation of all 9 serial-variables or summary-variables.
       If name = "foo[1]", wthis instance will represent only foo[1][0],
       foo[1][1], and foo[1][2], allowing for the creation of only 3
       serial-variables or summary-variables.
     The first step is to count how many indices (i.e. dimensions) are
     specified, and only expand the array beyond the point at which it has been
     specified. Strip off instances of [*] until the last character is not "]".
     Note that we retain "name" unaltered; this still forms the basis of the
     variable name and the array is expanded beyond the limit of the "name"
     specification.
  */
  int index_count = 0;
  if (name.back() == ']') {
    std::string name_base = name;
    while (name_base.back() == ']') {
      name_base.resize( name_base.find_last_of('['));
      index_count++;
    }
  }

  /* If the last specified index within "name" does not expand any further,
  *  don't add any values to array_size. An empty array_size is interpreted as
  *  indicating that "name" represents an atomic variable.
  *  The ref-attributes index[index_count].size indicates how many variables
  *  there are in the array identified at the level to which "name" is
  *  specified; if it is 0, it is atomic.
  *  If not, keep going until we either:
  *    - run out of indices, or
  *    - hit 8 levels.
  */
  for (unsigned int ii = index_count; ii < 8; ++ii) {
    if (ref->attr->index[ii].size == 0) { break;}
    array_size.push_back( ref->attr->index[ii].size);
  }
  // Flag the variable as trick-processed so we only need to do this once.
  trick_processed = true;
}


/*****************************************************************************
Name: generate_serial_variables
Purpose:
  Create a set of serial-logging-variables from the already processed
  ref-attributes values.
Notes:
- The var_ptr returned from the Trick ref-attributes lookup is a void*.
  We use the data-type (also returned from the ref-attributes lookup) to
  interpret the real data-type of the variable at the address identified by
  var_ptr. Then we reinterpret the type of var_ptr from void* to the type so
  identified. This involves the use of reinterpret_cast, which is not
  permitted per coding standards except where suitable checks are made to
  ensure type safety. We believe these checks have been implemented in this
  case, which legitimizes this particular use of reinterpret_cast.
*****************************************************************************/
void
EnhancedLogging_Variable::generate_serial_variables()
{
  serial_list_processed = true;
  if (type == "double") {
    double * typed_ptr = reinterpret_cast<double*>(var_ptr);
    EnhancedLogging_SerialVariableT<double>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    serial_variable_list);
  }
  else if ( type == "int") {
    int * typed_ptr = reinterpret_cast<int*>(var_ptr);
    EnhancedLogging_SerialVariableT<int>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    serial_variable_list);
  }
  else if ( type == "bool") {
    bool * typed_ptr = reinterpret_cast<bool*>(var_ptr);
    EnhancedLogging_SerialVariableT<bool>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    serial_variable_list);
  }
  else if ( type == "std::string") {
    std::string * typed_ptr = reinterpret_cast<std::string*>(var_ptr);
    EnhancedLogging_SerialVariableT<std::string>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    serial_variable_list);
  }
  else if ( type == "unsigned char") {
    unsigned char * typed_ptr = reinterpret_cast<unsigned char*>(var_ptr);
    EnhancedLogging_SerialVariableT<unsigned char>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    serial_variable_list);
  }
  else if ( type == "signed char") {
    signed char * typed_ptr = reinterpret_cast<signed char*>(var_ptr);
    EnhancedLogging_SerialVariableT<signed char>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    serial_variable_list);
  }
  else if ( type == "unsigned int") {
    unsigned int * typed_ptr = reinterpret_cast<unsigned int*>(var_ptr);
    EnhancedLogging_SerialVariableT<unsigned int>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    serial_variable_list);
  }
  else if ( type == "float") {
    float * typed_ptr = reinterpret_cast<float*>(var_ptr);
    EnhancedLogging_SerialVariableT<float>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    serial_variable_list);
  }
  else {
    CMLMessage::error(__FILE__,__LINE__,
      "Unrecognized data type: ",type, " for variable ",name,
      "\nwhile allocating a new EnhancedLogging_SummaryVariableT instance.\n"
      "Omitting this variable.\n"
      "See summary_logging_variable.cc to add support for this data type.\n");
    serial_list_processed = false;
  }
}


/*****************************************************************************
Name: generate_summary_variables
Purpose:
  Create a set of summary-logging-variables from the already processed
  ref-attributes values.
*****************************************************************************/
void
EnhancedLogging_Variable::generate_summary_variables()
{
  summary_list_processed = true;
  if (type == "double") {
    double * typed_ptr = reinterpret_cast<double*>(var_ptr);
    EnhancedLogging_SummaryVariableT<double>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else if ( type == "int") {
    int * typed_ptr = reinterpret_cast<int*>(var_ptr);
    EnhancedLogging_SummaryVariableT<int>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else if ( type == "bool") {
    bool * typed_ptr = reinterpret_cast<bool*>(var_ptr);
    EnhancedLogging_SummaryVariableT<bool>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else if ( type == "std::string") {
    std::string * typed_ptr = reinterpret_cast<std::string*>(var_ptr);
    EnhancedLogging_SummaryVariableT<std::string>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else if ( type == "unsigned char") {
    unsigned char * typed_ptr = reinterpret_cast<unsigned char*>(var_ptr);
    EnhancedLogging_SummaryVariableT<unsigned char>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else if ( type == "signed char") {
    signed char * typed_ptr = reinterpret_cast<signed char*>(var_ptr);
    EnhancedLogging_SummaryVariableT<signed char>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else if ( type == "unsigned int") {
    unsigned int * typed_ptr = reinterpret_cast<unsigned int*>(var_ptr);
    EnhancedLogging_SummaryVariableT<unsigned int>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else if ( type == "float") {
    float * typed_ptr = reinterpret_cast<float*>(var_ptr);
    EnhancedLogging_SummaryVariableT<float>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else if ( type == "char") {
    char * typed_ptr = reinterpret_cast<char*>(var_ptr);
    EnhancedLogging_SummaryVariableT<char>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else if ( type == "short") {
    short * typed_ptr = reinterpret_cast<short*>(var_ptr);
    EnhancedLogging_SummaryVariableT<short>::new_var_array(
                                                    typed_ptr,
                                                    alias,
                                                    units,
                                                    name,
                                                    array_size,
                                                    summary_variable_list);
  }
  else {
    CMLMessage::error(__FILE__,__LINE__,
      "Unrecognized data type: ",type, " for variable ",name,
      "\nwhile allocating a new EnhancedLogging_SummaryVariableT instance.\n"
      "Omitting this variable.\n"
      "See summary_logging_variable.cc to add support for this data type.\n");
    summary_list_processed = false;
  }
}

/*****************************************************************************
Name: add_variable
Purpose:
  Adds a pointer to an externally-created serial-variable or summary-variable
  to the appropriate list of variables included in the set.
Notes:
  This capability allows for the introduction of logging-variables into the
  set without needing to add another EnhancedLogging_Variable instance, and
  go back through the ref-attributes process.
*****************************************************************************/
void
EnhancedLogging_VariableSet::add_variable(
  EnhancedLogging_SerialVariable & var)
{
  serial_variable_list.push_back(&var);
}
/****************************************************************************/
void
EnhancedLogging_VariableSet::add_variable(
  EnhancedLogging_SummaryVariable & var)
{
  summary_variable_list.push_back(&var);
}
