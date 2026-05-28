/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides the class template for the data-type-specific implementation
  of a summary-logging-variable.)

LIBRARY DEPENDENCY:
   ((../src/summary_logging_variable.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/
#ifndef CML_SUMMARY_LOGGING_SUMMARY_VARIABLE_HH
#define CML_SUMMARY_LOGGING_SUMMARY_VARIABLE_HH

#include <string>
#include <vector>
#include <limits> //numeric_limits

#include "summary_logging_condition.hh"
#include "base_logging_variable.hh"

/*****************************************************************************
EnhancedLogging_SummaryVariable
Purpose:
  Abstract base class for the class-template EnhancedLogging_SummaryVariableT.
  Provides the name, alias, and units basis of a
  EnhancedLogging_SummaryVariableT.
*****************************************************************************/
class EnhancedLogging_SummaryVariable : public EnhancedLogging_BaseVariable
{
 protected:
  size_t condition_index; /* (--)
    The index identifying which element of recorded_vals and first_pass
    should be adjusted. Set upon entry to the populate_condition(...) method,
    and then used in the templated form of this class.*/
  std::vector<unsigned int> num_vals; /* (--)
    A set of the counts of number of values used in the averaging process.
    There is a unique count associated with each logging condition. */
  std::vector<bool> first_pass; /* (--)
    Set of flags, one per group condition to indicate whether the associated
    value -- recorded_vals -- in the template-class has been populated
    already.*/

 public:
  EnhancedLogging_SummaryVariable( std::string alias,
                                   std::string units = "",
                                   std::string name = "");
  virtual ~EnhancedLogging_SummaryVariable(){};

  void populate_condition(
                    size_t,
                    EnhancedLogging_SummaryConditionBase::LoggingSpecification)
                    override;
  void reset_summary_val( size_t ix) override;
  bool is_summary() override {return true;}
  virtual EnhancedLogging_SummaryVariable * clone_variable() const = 0;
  virtual void set_var() = 0;
  virtual void set_var_abs() = 0;
  virtual void set_var_max() = 0;
  virtual void set_var_min() = 0;
  virtual void set_var_abs_max() = 0;
  virtual void set_var_abs_min() = 0;
  virtual void set_var_abs_max_abs() = 0;
  virtual void set_var_abs_min_abs() = 0;
  virtual void set_var_avg() = 0;
  virtual void set_var_abs_avg() = 0;
  virtual void set_var_avg_excl_zeros() = 0;
};


/*****************************************************************************
EnhancedLogging_SummaryVariableT
Purpose:
  A type-specific extension of EnhancedLogging_SummaryVariable.
Notes:
- In case a variable is used in multiple groups, each group MUST have its
  own instance of the variable.
*****************************************************************************/
template< typename T>
class EnhancedLogging_SummaryVariableT : public EnhancedLogging_SummaryVariable
{
 protected:
  const T& var_ref; /* (--)
    Reference to the variable being logged. This reference is used to
    populate the class member var, and var is logged. This separation of
    actual variable and logged variable allows for the population of the
    logged value according to criteria satisfaction (such as max value)
    and subsequent logging of that value at a later time.*/
  std::vector<double> recorded_vals_dbl; /* (--)
    A representation of type T recorded_vals as type double, used for
    averaging.*/
 public:
  std::vector<T> recorded_vals; /* (--)
    Copy of the variable values logged, with 1 value for each logging-condition.
    This separation of the actual variable -- accessed via var_ref --
    and this logged variable allows for the recording of this variable
    at some arbitrary time according to criteria satisfaction (such as max
    value) and subsequent logging of this recorded value at a later time,
    after the value of var_ref has changed.*/

  /* Note:
     New logging-variable instances may be constructed in one of 3 ways:
     - directly, by providing a reference to the sim-variable and an
       accompanying alias. In this case, the "name" of the sim-variable is
       not needed. Units should be provided as appropriate.
     - indirectly via EnhancedLogging_Variable::generate_summary_variables()
       or EnhancedLogging_SummaryVariableT<>::new_var_array(...) (which is
       called from generate_summary_variables, so these are effectively the
       same process) by specifying the sim-variable name and (optionally)
       an alias.
     - by replicating / cloning the basic components of an existing instance;
       each summary-group that logs a particular sim-variable must have its
       own copy of the logging-variable representing that sim-variable because
       logging-variables must be sized to accommodate the evaluation-conditions
       within the group..
     So we have 2 constructors.
     - The first 2 cases both result in calls to the first constructor.
       - In the first case, it would be unusual to provide the optional name
         string and the optional units string may or may not be desired.
       - In the second case, the new_variable(...) method will provide all 4
         arguments to the constructor.
     - For replicating an existing instance, we need a copy-constructor. This
       must be blocked from SWIG because SWIG can't handle this, and the
       underlying purpose does not require a SWIG interface anyway.
     The "name" field of a logging-variable is not used in logging and is
     not truly necessary. It can be useful when testing for uniqueness of
     variables and aliases.
  */
  EnhancedLogging_SummaryVariableT( const T& var_ref_,
                                    std::string alias_,
                                    std::string units_ = "",
                                    std::string name_ = "")
    :
    EnhancedLogging_SummaryVariable( std::move(alias_),
                                     std::move(units_),
                                     std::move(name_)),
    var_ref(var_ref_)
  {
    set_field_width();
  }
  /***************************************************************************/
  #ifndef SWIG
  EnhancedLogging_SummaryVariableT(
    const EnhancedLogging_SummaryVariableT & var_)
    :
    EnhancedLogging_SummaryVariableT( var_.var_ref,
                                      var_.alias,
                                      var_.units,
                                      var_.name)
  {}
  #endif
 private: // remove operator= completely
  EnhancedLogging_SummaryVariableT operator=(
                                     const EnhancedLogging_SummaryVariableT&);

 public:
  virtual ~EnhancedLogging_SummaryVariableT(){};

/*****************************************************************************
Name: set_field_width
Purpose: Sets the field_width variable
Exceptions:
  T = std::string
*****************************************************************************/
  void set_field_width()
  {
    field_width =std::numeric_limits<T>::digits10+1;
  }


/*****************************************************************************
Name: new_var_array
Purpose: Create an array of instances of this type from a specified variable
         name
***************************************************************************/
  static void new_var_array(
                        const T * ptr,
                        const std::string & alias,
                        const std::string & units,
                        const std::string & name,
                        std::vector<unsigned int> & array_size,
                        std::vector<EnhancedLogging_SummaryVariable *> & ret_list)
  {
    /* If the variable is atomic -- i.e. does not represent the head of ani
       array of variables -- just create 1 new instance and leave.*/
    if (array_size.empty()) {
      ret_list.push_back( new EnhancedLogging_SummaryVariableT<T>( *ptr,
                                                                   alias,
                                                                   units,
                                                                   name));
      return;
    }

    /* array_size identifies the number of entries in each dimension of the
    *  array. Use these values to identify the number of values below each
    *  level.
    *  E.g. if array_size = {4,3,2}, then
    *          count_per_index = {24, 6, 2}
    */
    std::vector<unsigned int> count_per_index = array_size;
    for (int ii = count_per_index.size()-2; ii >= 0; --ii) {
      count_per_index[ii] *= count_per_index[ii+1];
    }

    /* Now create enough variables to cover the entire array, i.e.
    *  count_per_index[0] number of variables.
    *  We need to create the [n][m] suffix to add to the alias.
    *  Naturally, this might be done with a set of embedded for loops, but we
    *  don't know apriori how many levels to work with, so it is easier to work
    *  from 1-dimension and extract the indices from that 1-dimensional count.
    */
    for (unsigned int ii = 0; ii < count_per_index[0]; ++ii) {
      /* ii is the 1-dimensional index equivalent;
      *  ii_modified is the remnant of ii after subtracting off the index
      *  levels as the string is constructed.
      */
      unsigned int ii_modified = ii;
      std::string append = "";

      for (unsigned int jj = 0; jj < count_per_index.size()-1; ++jj) {
        /* jj_ix uses integer division to obtain the whole number of this
        *  level that are satisfied by the remaining ii_modified.
        *  ii_modified is then decremented by that multiple of the number of
        *  variables per index at this level.
        */
        unsigned int jj_ix = ii_modified / count_per_index[jj+1];
        append += "[" + std::to_string(jj_ix) + "]";
        ii_modified -= jj_ix * count_per_index[jj+1];
      }
      append += "[" + std::to_string(ii_modified) + "]";

      ret_list.push_back( new EnhancedLogging_SummaryVariableT<T>(
                                                         ptr[ii],
                                                         alias + append,
                                                         units,
                                                         name + append));
    }
  }

/*****************************************************************************
Name: clone_variable
Purpose: Creates a clone of this variable for addition to a group.
Note: Group is responsible for holding the returned pointer and deleting the
      allocated memory at its destruction.
*****************************************************************************/
  EnhancedLogging_SummaryVariable * clone_variable() const override
  {
    return( new EnhancedLogging_SummaryVariableT(*this));
  }


/*****************************************************************************
Name: initialize
Purpose:
  Resizes the arrays based on how many conditionals the group contains.
*****************************************************************************/
  void initialize( size_t num_conditionals) override
  {
    recorded_vals.resize(num_conditionals);
    recorded_vals_dbl.resize(num_conditionals, 0.0);
    num_vals.resize(num_conditionals, 0);
    first_pass.resize(num_conditionals, true);
  }
/*****************************************************************************
Name: set_var
Purpose:
  Basic operation, records the reference value.
*****************************************************************************/
  void set_var() override
  {
    recorded_vals[condition_index] = var_ref;
  }
/*****************************************************************************
Name: set_var_abs
Purpose:
  Basic operation, records the absolute value of the reference value.
*****************************************************************************/
  void set_var_abs() override
  {
    recorded_vals[condition_index] = std::abs(var_ref);
  }
/*****************************************************************************
Name: set_var_max
Purpose:
  records the reference value if it is larger than the previously recorded
  value.
*****************************************************************************/
  void set_var_max() override
  {
    if (var_ref > recorded_vals[condition_index]) {
      recorded_vals[condition_index] = var_ref;
    }
  }
/*****************************************************************************
Name: set_var_min
Purpose:
  records the reference value if it is smaller than the previously recorded
  value.
*****************************************************************************/
  void set_var_min() override
  {
    if (var_ref < recorded_vals[condition_index]) {
      recorded_vals[condition_index] = var_ref;
    }
  }
/*****************************************************************************
Name: set_var_abs_max
Purpose:
  records the reference value if its absolute value is
  larger than the absolute-value of the previously recorded value.
*****************************************************************************/
  void set_var_abs_max() override
  {
    if (std::abs(var_ref) > std::abs(recorded_vals[condition_index])) {
      recorded_vals[condition_index] = var_ref;
    }
  }
/*****************************************************************************
 * Name: set_var_abs_max_abs
Purpose:
  records the reference value if its absolute value is
  larger than the absolute-value of the previously recorded value.
*****************************************************************************/
  void set_var_abs_max_abs() override
  {
    if (std::abs(var_ref) > std::abs(recorded_vals[condition_index])) {
      recorded_vals[condition_index] = std::abs(var_ref);
    }
  }
/*****************************************************************************
Name: set_var_abs_min
Purpose:
  records the reference value if its absolute value is
  smaller than the absolute-value of the previously recorded value.
*****************************************************************************/
  void set_var_abs_min() override
  {
    if (std::abs(var_ref) < std::abs(recorded_vals[condition_index])) {
      recorded_vals[condition_index] = var_ref;
    }
  }
/*****************************************************************************
 * Name: set_var_abs_min_abs
Purpose:
  records the reference value if its absolute value is
  smaller than the absolute-value of the previously recorded value.
*****************************************************************************/
  void set_var_abs_min_abs() override
  {
    if (std::abs(var_ref) < std::abs(recorded_vals[condition_index])) {
      recorded_vals[condition_index] = std::abs(var_ref);
    }
  }
/*****************************************************************************
Name: set_var_avg
Purpose:
  Records a running average of the values of the reference value.
*****************************************************************************/
  void set_var_avg() override
  {
    /* See documentation for why the averaging is done this way.
     * There is a reasoning for this madness!*/
    unsigned int num_vals_ = ++num_vals[condition_index];
    double var_dbl =
         (recorded_vals_dbl[condition_index] * ((num_vals_-1.0) / num_vals_)) +
         (static_cast<double>(var_ref) / num_vals_);
    recorded_vals_dbl[condition_index] = var_dbl;
    recorded_vals[condition_index] = var_dbl;
  }
/*****************************************************************************
Name: set_var_abs_avg
Purpose:
  Records a running average of the absolute-values of the reference value.
*****************************************************************************/
  void set_var_abs_avg() override
  {
    unsigned int num_vals_ = ++num_vals[condition_index];
    double var_dbl =
         (recorded_vals_dbl[condition_index] * ((num_vals_-1.0) / num_vals_)) +
         (std::abs(static_cast<double>(var_ref)) / num_vals_);
    recorded_vals_dbl[condition_index] = var_dbl;
    recorded_vals[condition_index] = var_dbl;
  }
/*****************************************************************************
Name: set_var_avg_excl_zero
Purpose:
  Records a running average of the values of the reference value.  only
  including those values that are non-zero.
*****************************************************************************/
  void set_var_avg_excl_zeros() override
  {
    if (var_ref != 0.0) {
      set_var_avg();
    }
  }

/*****************************************************************************
Name: log_value
Purpose: Appends the recorded value to the collected data.
Notes:
- log_value and log_value_fmt without a specified index will log the
  variable's name, followed by recorded values for this variable all on a
  single line, then add a line-break. This is used to log the values of a
  specific variable for all conditions.
- log_value and log_value_fmt with a specified index will log only the
  value at the specified index; line management is handled by the calling
  method. This is used to record the values associated with a specific
  condition across all variables in the group.
*****************************************************************************/
  void log_value( std::ofstream & stream) const override
  {
    stream << name;
    for (size_t ii = 0; ii < recorded_vals.size(); ++ii) {
      stream << ", " << recorded_vals[ii];
    }
    stream << std::endl;
  }
/****************************************************************************/
  void log_value( size_t index, std::ofstream & stream) const
  {
    stream << ", " << recorded_vals[index];
  }
/*****************************************************************************
Name: log_value_fmt
Purpose: Sends a formatted value to ofstream
Exceptions:
*****************************************************************************/
  void log_value_fmt( std::ofstream & stream) const override
  {
    stream << name << std::setprecision(field_width) <<
                                                     std::setw(field_width+4);
    for (size_t ii = 0; ii < recorded_vals.size(); ++ii) {
      stream << ", " << recorded_vals[ii];
    }
    stream << std::endl;
  }
/****************************************************************************/
  void log_value_fmt( size_t index, std::ofstream & stream) const override
  {
    stream << ", " << std::setw(field_width+4) <<
              std::setprecision(field_width) << recorded_vals[index];
  }
};

/**************************************************************************
Special treatments, implementations are provided in summary_logging_variable.cc
(NOTE: this is not a convenience thing, template-specializations MUST be
       implemented in the cc file)
**************************************************************************/
template<> void EnhancedLogging_SummaryVariableT<bool>::set_var_abs();
template<> void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_max();
template<> void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_min();
template<> void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_max_abs();
template<> void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_min_abs();
template<> void EnhancedLogging_SummaryVariableT<bool>::set_var_avg();
template<> void EnhancedLogging_SummaryVariableT<bool>::set_var_abs_avg();
template<> void EnhancedLogging_SummaryVariableT<bool>::set_var_avg_excl_zeros();
template<> void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs();
template<> void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs_max();
template<> void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs_min();
template<> void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs_max_abs();
template<> void EnhancedLogging_SummaryVariableT<unsigned int>::set_var_abs_min_abs();
template<> void EnhancedLogging_SummaryVariableT<float>::set_var_avg_excl_zeros();
template<> void EnhancedLogging_SummaryVariableT<double>::set_var_avg_excl_zeros();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_field_width();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_max();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_min();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_max();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_min();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_max_abs();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_min_abs();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_avg();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_abs_avg();
template<> void EnhancedLogging_SummaryVariableT<std::string>::set_var_avg_excl_zeros();
#endif
