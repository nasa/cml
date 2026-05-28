/*******************************TRICK HEADER***********************************
 PURPOSE:
    ( Provide a simplified form of GenericMultiInputTable for the simple
      case of using only one independent variable.
      This simplifies the form of the method generate_base_values() and
      generate_output.)

 PROGRAMMERS:
   (((Gary Turner) (OSR) (May 2023) (Antares) (initial version))
   )
*******************************************************************************/

#include "../include/generic_single_input_table.hh"


/*****************************************************************************
Constructors omitted
Notes: - Class inherits constructors from GenericMultiInputTable
*****************************************************************************/




/*****************************************************************************
initialize
Purpose:
  Override of the GenericMultiInputTable::initialize() method,
  extending it by making an additional check that there is only one
  independent variable
*****************************************************************************/
bool
GenericSingleInputTable::initialize()
{
  if (independents.size() > 1) {
    CMLMessage::error( __FILE__,__LINE__,
      "Invalid configuration: the GenericSingleInputTable has been configured"
      "with ",independents.size(), " independent variables.\n"
      "To use a multi-input interpolation, use the GenericMultiInputTable "
      "instance instead.\nInitialization failed.\n");
    return false;
  }
  return GenericMultiInputTable::initialize();
}



/*****************************************************************************
generate_base_values
Purpose:
  Override of the GenericMultiInputTable::generate_base_values() method.
  This is a much simpler method, simplified because there is only one
  independent variable to consider.
Notes:
 - Much of this switch block is copied from GenericMultiInputTable,
   but separating the common content out of the GenericMultiInputTable flow
   to be callable from both locations would be challenging because the content
   is embedded in a for loop (for all independents) in the
   GenericMultiInputTable (this is required because the independent variables
   are independently configured, they are not all required to use the same
   lookup-type.
*****************************************************************************/
void
GenericSingleInputTable::generate_base_values()
{
  TableIndependentVariable & TIV = *independents[0].first;
  size_t index =   TIV.get_index();

  switch(independents[0].second) {
  case TableIndependentVariable::Interp:
    if( TIV.is_off_table()) {
      break; // use default index.  No other actions
    }
    // Interpolating between 2 points, identified from the update to the
    // independent variable as "index", and "index"+1
    data_point_index.resize (2);
    data_point_index[0] = index;
    data_point_index[1] = index+1;
    return;

  case TableIndependentVariable::Prev:
    if ( TIV.prox_override) {
      index = TIV.index_prox;
    }
    break;
  case TableIndependentVariable::Next:
    // Use prox-index if in proximity
    if ( TIV.prox_override) {
      index = TIV.index_prox;
    }
    // increment index if not off the table.
    else if( !TIV.is_off_table())  {
      ++index;
    }
    break;
  case TableIndependentVariable::Floor:
    if ( TIV.prox_override) {
      index = TIV.index_prox;
    }
    // if table is decreasing and value is not off table, use the next index
    else if( !TIV.is_off_table() &&
             !TIV.is_table_increasing()) {
      ++index;
    }
    break;
  case TableIndependentVariable::Ceil:
    if ( TIV.prox_override) {
      index = TIV.index_prox;
    }
    // if table is increasing and value is not off table, use the next index
    else if( !TIV.is_off_table() &&
              TIV.is_table_increasing()) {
      ++index;
    }
    break;
  case TableIndependentVariable::Round:
    if ( TIV.fraction > 0.5) {
      ++index;
    }
    break;
  default:
    // NOTE - this should be unreachable.  The LookupMethod is assigned as
    // part of the add_independent method, which requires a valid
    // enumeration (or assuumes one if none is provided).  THereafter, that
    // value is protected.  This is a fail-safe.
    CMLMessage::fail(
      __FILE__,__LINE__,
      "Specification of LookupMethod is invalid.\n");
  }

  // All cases except Interp mode while on table (already returned), and
  // default (already terminated) just lookup a single value from the table:
  data_point_index.assign(1, index);
}


/*****************************************************************************
generate_output
Purpose:
  Override of the GenericMultiInputTable::generate_output() method.
  This is a much simpler method, simplified because there is only one
  independent variable to consider.
*****************************************************************************/
bool
GenericSingleInputTable::generate_output()
{
  // Note -- not using data_point_weight, so do not need precheck_output()
  // There is no mechanism by which this could fail. Return true always.
  // Note -- return value only because base-class (GenericMultiInputTable)
  //         method returns a value.

  size_t data_ix = data_point_index[0];

  // If, for whatever reason, the generate_base_values method
  // identified that we are using a lookup rather than an interpolation,
  // then simply lookup the values for the variables and their derivatives.
  if (data_point_index.size() == 1) {
    for (double* out_ptr : output) {
      *out_ptr = data[data_ix];
      data_ix += num_data_elements_per_increment_of_index[0];
    }
  }
  else {
    // independent's fraction value is the fraction of the bin while
    // moving from the lower index to the upper index. So the upper index
    // should be weighted by fraction, and the lower index weighted by
    // (1-fraction)
    double frac =  independents[0].first->fraction;
    for (double* out_ptr : output) {
      *out_ptr = data[data_ix]   * (1-frac) +
                 data[data_ix+1] * frac;
      data_ix += num_data_elements_per_increment_of_index[0];
    }
  }
  return true;
}
