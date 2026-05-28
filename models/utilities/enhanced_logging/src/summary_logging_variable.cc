/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides the class template for the data-type-specific implementation
  of a summary-logging-variable.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/
#include "../include/summary_logging_variable.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils


/*****************************************************************************
Constructor
*****************************************************************************/
EnhancedLogging_SummaryVariable::EnhancedLogging_SummaryVariable(
  std::string alias_,
  std::string units_,
  std::string name_)
  :
  EnhancedLogging_BaseVariable( std::move(alias_),
                                std::move(units_),
                                std::move(name_)),
  condition_index(0),
  num_vals(),
  first_pass()
{}


/*****************************************************************************
populate_condition
Purpose:
  Uses the template specializations to populate the ii'th member of the
  recorded_vals vector according to the provided mathematical specification
*****************************************************************************/
void
EnhancedLogging_SummaryVariable::populate_condition(
   size_t ii,
   EnhancedLogging_SummaryConditionBase::LoggingSpecification type)
{
  condition_index = ii;
  switch(type) {
  case EnhancedLogging_SummaryConditionBase::LATEST:
    set_var(); //just always set_var, overwriting any previous values
    break;
  case EnhancedLogging_SummaryConditionBase::SINGLE:
    if (first_pass[ii]) {
      set_var();
    }
    // else, don't do anything! It has already been recorded for this
    // condition.
    break;
  case EnhancedLogging_SummaryConditionBase::MAX:
    if (first_pass[ii]) {
      set_var();
    } else {
      set_var_max();
    }
    break;
  case EnhancedLogging_SummaryConditionBase::MIN:
    if (first_pass[ii]) {
      set_var();
    } else {
      set_var_min();
    }
    break;
  case EnhancedLogging_SummaryConditionBase::ABS_MAX:
    if (first_pass[ii]) {
      set_var();
    } else {
      set_var_abs_max();
    }
    break;
  case EnhancedLogging_SummaryConditionBase::ABS_MIN:
    if (first_pass[ii]) {
      set_var();
    } else {
      set_var_abs_min();
    }
    break;
  case EnhancedLogging_SummaryConditionBase::ABS_MAX_RECORD_ABS:
    if (first_pass[ii]) {
      set_var_abs();
    } else {
      set_var_abs_max_abs();
    }
    break;
  case EnhancedLogging_SummaryConditionBase::ABS_MIN_RECORD_ABS:
    if (first_pass[ii]) {
      set_var_abs();
    } else {
      set_var_abs_min_abs();
    }
    break;
  case EnhancedLogging_SummaryConditionBase::AVG:
    set_var_avg();
    break;
  case EnhancedLogging_SummaryConditionBase::ABS_AVG:
    set_var_abs_avg();
    break;
  case EnhancedLogging_SummaryConditionBase::NZ_AVG:
    set_var_avg_excl_zeros();
    break;
  default: // includes all conditional logging; these requirethe extended class
    CMLMessage::error( __FILE__,__LINE__,
      "Logging Specification type ",type," is not supported\n"
      "by EnhancedLogging_SummaryVariable.\n");
  }
  first_pass[ii] = false;
}


/*****************************************************************************
Name: reset_summary_val
Purpose:
  Resets the protected first_pass values for the specified index.
  On the next pass for which the condition is satsified, the summary-value will reset to the current value of the variable.
*****************************************************************************/
void
EnhancedLogging_SummaryVariable::reset_summary_val( size_t ix)
{
  if (ix >= first_pass.size()) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot reset condition at index ",ix,". There are only ",
      first_pass.size()," conditions.\n");
  }
  else {
    first_pass[ix] = true;
    num_vals[ix] = 0;
  }
}

/*****************************************************************************
Template specializations
*****************************************************************************/
template<>
void EnhancedLogging_SummaryVariableT<bool>::set_var_abs()
{
  set_var();
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_max()
{
  set_var_max();
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_min()
{
  set_var_min();
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_max_abs()
{
  set_var_max();
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_min_abs()
{
  set_var_min();
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<bool>::set_var_avg()
{
  unsigned int num_vals_ = ++num_vals[condition_index];
  double var_dbl =
         (recorded_vals_dbl[condition_index] * ((num_vals_-1.0) / num_vals_));
  if (var_ref) { var_dbl += 1.0 / num_vals_;}
  recorded_vals_dbl[condition_index] = var_dbl;
  recorded_vals[condition_index] = (var_dbl >= 0.5);
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_avg()
{
  set_var_avg();
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<bool>::set_var_avg_excl_zeros()
{
  // It makes no sense to exclude zeros (false) from boolean averaging.
  set_var_avg();
}
/**************************************************************************/
template<>
void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs()
{
  set_var();
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs_max()
{
  if (var_ref > recorded_vals[condition_index]) {
      recorded_vals[condition_index] = var_ref;
  }
}
/**************************************************************************/
template<>
void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs_min()
{
  if (var_ref < recorded_vals[condition_index]) {
      recorded_vals[condition_index] = var_ref;
  }
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs_max_abs()
{
  set_var_abs_max();
}
/**************************************************************************/
template<>
void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs_min_abs()
{
  set_var_abs_min();
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<float>::set_var_avg_excl_zeros()
{
  if (!MathUtils::is_near_equal( var_ref, (float)0.0)) {
    set_var_avg();
  }
}
//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<double>::set_var_avg_excl_zeros()
{
  if (!MathUtils::is_near_equal( var_ref, 0.0)) {
    set_var_avg();
  }
}

//**************************************************************************
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_field_width()
{
  field_width = 12;
}

/*****************************************************************************
Name: log_value
Purpose:
  Appends the recorded value to the collected data; these are the template
  specializations for data types that need specific treatment.
Notes:
- Need to treat unsigned char and signed char differently:
  - Mathworks-autocoded bool_t types get typedef'd to unsigned char, not bool.
  - There may be other signed char data types coming from Grok structures
  - Need to avoid logging ASCII 8 (backspace), 10 (linefeed/newline), or 13
     (carriage return) as chars
  - cast unsigned char and signed char to int before logging.
- log_value and log_value_fmt without a specified index will log the
  variable's name, followed by recorded values for this variable all on a
  single line, then add a line-break. This is used to log the values of a
  specific variable for all conditions.
- log_value and log_value_fmt with a specified index will log only the
  value at the specified index; line management is handled by the calling
  method. This is used to record the values associated with a specific
  condition across all variables in the group.
*****************************************************************************/

template<> void EnhancedLogging_SummaryVariableT<unsigned char>::log_value( std::ofstream & stream) const
{
  stream << name;
  for (size_t ii = 0; ii < recorded_vals.size(); ++ii) {
    stream << ", " << static_cast<int>(recorded_vals[ii]);
  }
  stream << std::endl;
}
/****************************************************************************/
template<> void EnhancedLogging_SummaryVariableT<unsigned char>::log_value( size_t index, std::ofstream & stream) const
{
  stream << ", " << static_cast<int>(recorded_vals[index]);
}
/*****************************************************************************
Name: log_value_fmt
Purpose: Sends a formatted value to ofstream
Exceptions:
*****************************************************************************/
template<> void EnhancedLogging_SummaryVariableT<unsigned char>::log_value_fmt( std::ofstream & stream) const
{
  stream << name << std::setprecision(field_width) <<
                                                    std::setw(field_width+4);
  for (size_t ii = 0; ii < recorded_vals.size(); ++ii) {
    stream << ", " << static_cast<int>(recorded_vals[ii]);
  }
  stream << std::endl;
}
/****************************************************************************/
template<> void EnhancedLogging_SummaryVariableT<unsigned char>::log_value_fmt( size_t index, std::ofstream & stream) const
{
  stream << ", " << std::setw(field_width+4) <<
            std::setprecision(field_width) << static_cast<int>(recorded_vals[index]);
}
template<> void EnhancedLogging_SummaryVariableT<signed char>::log_value( std::ofstream & stream) const
{
  stream << name;
  for (size_t ii = 0; ii < recorded_vals.size(); ++ii) {
    stream << ", " << static_cast<int>(recorded_vals[ii]);
  }
  stream << std::endl;
}
/****************************************************************************/
template<> void EnhancedLogging_SummaryVariableT<signed char>::log_value( size_t index, std::ofstream & stream) const
{
  stream << ", " << static_cast<int>(recorded_vals[index]);
}
/*****************************************************************************
Name: log_value_fmt
Purpose: Sends a formatted value to ofstream
Exceptions:
*****************************************************************************/
template<> void EnhancedLogging_SummaryVariableT<signed char>::log_value_fmt( std::ofstream & stream) const
{
  stream << name << std::setprecision(field_width) <<
                                                    std::setw(field_width+4);
  for (size_t ii = 0; ii < recorded_vals.size(); ++ii) {
    stream << ", " << static_cast<int>(recorded_vals[ii]);
  }
  stream << std::endl;
}
/****************************************************************************/
template<> void EnhancedLogging_SummaryVariableT<signed char>::log_value_fmt( size_t index, std::ofstream & stream) const
{
  stream << ", " << std::setw(field_width+4) <<
            std::setprecision(field_width) << static_cast<int>(recorded_vals[index]);
}

// Empty methods, no meaningful resolution.
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs() {};
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_max() {};
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_min() {};
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_max() {};
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_min() {};
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_max_abs() {};
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_min_abs() {};
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_avg() {};
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_avg() {};
template<>
void EnhancedLogging_SummaryVariableT<std::string>::set_var_avg_excl_zeros() {};
