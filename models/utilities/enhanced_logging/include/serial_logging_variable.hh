/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides the class template for the data-type-specific implementation
  of a serial-logging-variable.)

LIBRARY DEPENDENCY:
   ((../src/serial_logging_variable.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/
#ifndef ENHANCED_LOGGING_SERIAL_VARIABLE_HH
#define ENHANCED_LOGGING_SERIAL_VARIABLE_HH

#include <string>
#include <fstream>
#include <limits> //numeric_limits

#include "base_logging_variable.hh"

// Silly appeal to symmetry. The non-templated form of the serial-variable is
// just a base-variable.
typedef EnhancedLogging_BaseVariable EnhancedLogging_SerialVariable;
/*****************************************************************************
EnhancedLogging_SerialVariable
Purpose: A type-specific extension of EnhancedLogging_SerialVariableBase.
*****************************************************************************/
template< typename T>
class EnhancedLogging_SerialVariableT : public EnhancedLogging_SerialVariable
{
 protected:
  const T& var_ref; /* (--)
    Reference to the variable being logged. This reference is used to
    populate the class member var, and var is logged. This separation of
    actual variable and logged variable allows for the population of the
    logged value according to criteria satisfaction (such as max value)
    and subsequent logging of that value at a later time.*/

 public:
  EnhancedLogging_SerialVariableT( const T& var_ref_,
                                   std::string alias_,
                                   std::string units_ = "",
                                   std::string name_ = "")
    :
    EnhancedLogging_SerialVariable( std::move(alias_),
                                    std::move(units_),
                                    std::move(name_)),
    var_ref(var_ref_)
  {
    set_field_width();
  }
  virtual ~EnhancedLogging_SerialVariableT(){};

 private: // remove operator= fnd copy-constructor completely
  EnhancedLogging_SerialVariableT (const EnhancedLogging_SerialVariableT&);
  EnhancedLogging_SerialVariableT operator=(
                                   const EnhancedLogging_SerialVariableT&);
 public:
/*****************************************************************************
Name: set_field_width
Purpose: Sets the field_width variable
Exceptions:
  T = std::string
*****************************************************************************/
  void set_field_width()
  {
    field_width =std::numeric_limits<T>::max_digits10+1;
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
                        std::vector<EnhancedLogging_SerialVariable *> & ret_list)
  {
    /* If the variable is atmic -- i.e. does not represent the head of an array
     * of variables -- just create 1 new instance and leave.*/
    if (array_size.empty()) {
      ret_list.push_back( new EnhancedLogging_SerialVariableT<T>( *ptr,
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

      ret_list.push_back( new EnhancedLogging_SerialVariableT<T>(
                                                         ptr[ii],
                                                         alias + append,
                                                         units,
                                                         name + append));
    }
  }

/*****************************************************************************
Name: log_value
Purpose: Sends an unformatted value to ofstream
*****************************************************************************/
  void log_value( size_t, std::ofstream & stream) const override
  {
    log_value(stream);
  }
  void log_value( std::ofstream & stream) const override
  {
    stream << ", " << var_ref;
  }
/*****************************************************************************
Name: log_value_fmt
Purpose: Sends a formatted value to ofstream
*****************************************************************************/
  void log_value_fmt( size_t, std::ofstream & stream) const override
  {
    log_value_fmt(stream);
  }
  void log_value_fmt( std::ofstream & stream) const override
  {
    stream << ", " << std::setprecision(field_width) <<
              std::setw(field_width+4) << var_ref;
  }
};

/**************************************************************************
Special treatments, implementations are provided in serial_logging_variable.cc
**************************************************************************/
template<> void EnhancedLogging_SerialVariableT<std::string>::set_field_width();
#endif
