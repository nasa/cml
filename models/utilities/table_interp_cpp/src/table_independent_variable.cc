/*******************************************************************************
PURPOSE:
  (Table lookup model component representing the independent variable and its
   lookup)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Dec 2015) (New implementation))
   ((Bingquan Wang) (OSR) (Apr 2017) (Fixed the compilation warning of float-point
                                      number equality comparison))
   ((Bingquan Wang) (OSR) (Aug 2017) (IVV code cleaup and refactored))
  )
*******************************************************************************/

#include <cmath> // floor
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/table_independent_variable.hh"

/*****************************************************************************
TableIndependentVariable
Constructors
*****************************************************************************/
TableIndependentVariable::TableIndependentVariable(
    const double    &variable_in,
    double           frac_eps_in)
  :
  perform_full_search(false),
  fraction(0.0),
  prox_override(true),
  index_prox(0),
  index(0),
  variable(variable_in),
  name(),
  size(0),
  data_loaded(false),
  initialized(false),
  table_values_increasing(true),
  off_table_front(false),
  off_table_back(false),
  continuity(TableIndependentVariable::Linear),
  back_value(0.0),
  front_value(0.0),
  max_value(0.0),
  min_value(0.0),
  delta(0.0),
  modified_value(0.0),
  FRAC_EPS(frac_eps_in)
{
  data.clear();
}
//*****************************************************************************
TableIndependentVariable::TableIndependentVariable(
    const std::string &name_in,
    const double &variable_in,
    double frac_eps_in)
  :
  TableIndependentVariable(variable_in, frac_eps_in)
{
  name = name_in;
}
//*****************************************************************************
TableIndependentVariable::TableIndependentVariable(
    const double    &variable_in,
    const Continuity continuity_in,
    double           frac_eps_in)
  :
  TableIndependentVariable(variable_in, frac_eps_in)
{
  continuity = continuity_in;
}
//*****************************************************************************
TableIndependentVariable::TableIndependentVariable(
    const std::string &name_in,
    const double &variable_in,
    const Continuity continuity_in,
    double frac_eps_in)
  :
  TableIndependentVariable(variable_in, frac_eps_in)
{
  name = name_in;
  continuity = continuity_in;
}

/*****************************************************************************
load_data
Purpose:(loads the data from an input 1-d array to the class's 1-d vector after
         making appropriate validity checks. Size of load is determined by
         input parameter, it may be a subset of the input array.)
*****************************************************************************/
bool
TableIndependentVariable::load_data(
       const double* const data_in,
       size_t size_in)
{
  if (!data_in) {
    CMLMessage::error(
      __FILE__, __LINE__, "Data load error.\n",
      "The given pointer to data table for independent variable ",name,
      " cannot be NULL.\n");
    return false;
  }

  if (!size_in) {
    CMLMessage::error(
      __FILE__, __LINE__, "Data load error.\n",
      "The given size for data for independent variable ",name,
      " must be > 0.\n");
    return false;
  }

  DoubleVec scratch(data_in, data_in+size_in);
  return load_data(scratch);
}
/****************************************************************************/
bool
TableIndependentVariable::load_data(
       const DoubleVec & data_in)
{
  if (data_loaded) {
    CMLMessage::error(
      __FILE__, __LINE__, "Data load error.\n",
      "Data for independent variable ",name," has already been loaded.\n"
      "Second call to load_data() fails without first clearing out the "
      "existing data.\nTo Overwrite data, first call clear_data().\n"
      "Data load failed.\n");
    return false;
  }
  size = data_in.size();
  if ( size < 1 ) {
    CMLMessage::error(
      __FILE__,__LINE__,"Data load error:\n",
      "Data must not be empty\n" );
      return false;
  }

  // Check the monotonic nature of the data table of independent variable
  if (!check_monotonicity( data_in)) {
    return false;
  }

  // sanity check
  if (size == 1) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Data load warning:\n",
      "The data table to load for independent variable ",name," has only one "
      "data point.\n"
      "There is nothing wrong, just nothing to look up; output value "
      "is constant.\n");
  }
  // copy the data.
  data = data_in;

  data_loaded = true;

  return true;
}

/*****************************************************************************
initialize
Purpose:(Runs the first lookup, which always uses the binary search.)
*****************************************************************************/
bool
TableIndependentVariable::initialize()
{
  if (!data_loaded) {
    CMLMessage::error(
      __FILE__, __LINE__, "Update error.\n",
      "Data for independent variable (",name,") has not been loaded yet.\n"
      "It cannot be used.\n");
    return false;
  }
  if (size > 1) {
    index = 0;
    if ( determine_modified_value()) {
      binary_search();
      generate_fraction();
    }
  }
  else {
    index = 0;
    fraction = 0.0;
  }
  initialized = true;
  return true;
}

/*****************************************************************************
update
Purpose:(Generate the current index and fraction from the value of variable)
*****************************************************************************/
bool
TableIndependentVariable::update()
{
  if (!initialized) {
    CMLMessage::error(
      __FILE__, __LINE__, "Update error.\n",
      "Independent variable ",name," has not been initialized.\n"
      "It cannot be used.\n");
    return false;
  }

  // check for the case of single data point.  Don't need to do anything
  // in this case, there is nothing to look up and the data cannot change.
  if (1 == size) {
    return true;
  }

  if (!determine_modified_value()) {
    // determine_modified_value returns false if off-table, in which case
    // there is nothing new to do here.
    return true;
  }
  // We now have an independent variable in modified_value that is
  // guaranteed to be in range, with wrapping or limiting already done
  // if needed.

  // Identify where in the data vector the latest value resides relative to
  // the previous value.
  int direction = 0;
  if ( in_front_of( modified_value, data[index] ) ) {
    direction = -1;
  }
  else if ( in_back_of( modified_value, data[index+1] ) ) {
    direction = 1;
  }

  // If not in the current bin:
  if (direction != 0) {
    // if user has specified that a full search should be performed every
    // time (e.g. data is highly random), perform a binary search
    if (perform_full_search) {
      binary_search();
    }
    // Otherwise sweep from the last known cell.
    else if (direction == 1) {
      sweep_up();
    }
    else {
      sweep_down();
    }
  }

  generate_fraction();
  return true;
}

/*****************************************************************************
bias_data
Purpose:(Bias specified elements in the data array by the specified offset.)
*****************************************************************************/
void
TableIndependentVariable::bias_data(
        double bias,
        size_t ix_start,
        size_t ix_stop)
{
  // Note - data_loaded implies data.size() > 0.
  if (!data_loaded) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid external initialization sequence\n",
      "Call made to bias data but data has not yet been loaded.\n"
      "There is no data to bias.\n"
      "Check sequencing.\n");
    return;
  }
  if (ix_start > ix_stop) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid arguments\n",
      "Call made to bias data between two indices with the start index (",
      ix_start,")\n"
      "higher than the stop index (",ix_stop,").  This could be an error.\n"
      "Bias will be applied to the data values between these indices.\n");
    size_t ix_scratch = ix_start;
    ix_start = ix_stop;
    ix_stop = ix_scratch;
  }

  // Make a copy of the existing data, modify it and recheck it for validity
  // before changing the real data
  std::vector<double> data_copy (data);
  bool tab_val_incr_copy = table_values_increasing;

  // modify it:
  for (size_t ii = ix_start; ii <= ix_stop; ++ii) {
    data_copy[ii] += bias;
  }
  if (!check_monotonicity(data_copy)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Bias application\n",
      "Bias being applied to data would result in an invalid data set for\n"
      "the TableIndependentVariable data.\n"
      "Application of bias failed.\n"
      "Keeping original data.\n");
    // Revert table_values_increasing (which was potentially overwritten in
    // check_monotonicity(...)).
    table_values_increasing = tab_val_incr_copy;
  }
  else {
    // The specified changes produce a valid data set.
    // Apply the same change to the original data.
    // Note - this trivial computation is faster than a lookup to
    //        reassign value from data_copy.
    for (size_t ii = ix_start; ii <= ix_stop; ++ii) {
      data[ii] += bias;
    }
  }
}

/*****************************************************************************
scale_data
Purpose:(scale specified elements in the data array by the specified factor.)
*****************************************************************************/
void
TableIndependentVariable::scale_data(
        double scale,
        size_t ix_start,
        size_t ix_stop)
{
  if (!data_loaded) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid external initialization sequence\n",
      "Call made to scale data but data has not yet been loaded.\n"
      "There is no data to scale.\n"
      "Check sequencing.\n");
    return;
  }
  if (ix_start > ix_stop) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid arguments\n",
      "Call made to scale data between two indices with the start index (",
      ix_start,")\n"
      "higher than the stop index (",ix_stop,").  This could be an error.\n"
      "Scale will be applied to the data values between these indices.\n");
    size_t ix_scratch = ix_start;
    ix_start = ix_stop;
    ix_stop = ix_scratch;
  }

  // Make a copy of the existing data, modify it and recheck it for validity
  // before changing the real data
  std::vector<double> data_copy (data);
  bool tab_val_incr_copy = table_values_increasing;

  for (size_t ii = ix_start; ii <= ix_stop; ++ii) {
    data_copy[ii] *= scale;
  }
  if (!check_monotonicity(data_copy)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Bias application\n",
      "Scale being applied to data would result in an invalid data set for\n"
      "the TableIndependentVariable data.\n"
      "Application of scale (multiplicative bias) failed.\n"
      "Keeping original data.\n");
    // Revert table_values_increasing (which was potentially overwritten in
    // check_monotonicity(...)).
    table_values_increasing = tab_val_incr_copy;
  }
  else {
    // The specified changes produce a valid data set.
    // Apply the same change to the original data.
    // Note - this trivial computation is faster than a lookup to
    //        reassign value from data_copy.
    for (size_t ii = ix_start; ii <= ix_stop; ++ii) {
      data[ii] *= scale;
    }
  }
}

/*****************************************************************************
set_name
Purpose:(Adds a name to the instance)
*****************************************************************************/
void
TableIndependentVariable::set_name (const std::string &new_name)
{
  if (!name.empty()) {
    CMLMessage::warn(
      __FILE__,__LINE__,"set_name warning:\n",
      "Changing the name of independent variable ",name," to ",new_name,"\n");
  }
  name.assign(new_name);
}

/*****************************************************************************
in_back_of
Purpose:(Determines if val1 is after val2, with values either monotonically
  increasing or decreasing.)
*****************************************************************************/
bool
TableIndependentVariable::in_back_of(
  const double val1,
  const double val2 ) const
{
  return (  table_values_increasing && ( val1 >= val2 ) ) ||
         ( !table_values_increasing && ( val1 <= val2 ) );
}

/*****************************************************************************
in_front_of
Purpose:(Determines if val1 is before val2, with values either monotonically
  increasing or decreasing.)
*****************************************************************************/
bool
TableIndependentVariable::in_front_of(
  const double val1,
  const double val2 ) const
{
  return (  table_values_increasing && ( val1 < val2 ) ) ||
         ( !table_values_increasing && ( val1 > val2 ) );
}

/*****************************************************************************
binary_search
Purpose:(performs a full search, blocking half of he remaing data table at
         each iteration)
*****************************************************************************/
void
TableIndependentVariable::binary_search()
{
  // do the binary search starting from a central index
  index = (size - 1) / 2;
  size_t low_ix = 0;
  size_t high_ix = size-1;
  while (high_ix - low_ix > 1) {
    // if the independent variable is to the "right" of the "left-hand" side
    // of the current bin, set low_ix to the current index.  We cannot move to
    // the left of this bin.
    if ( in_back_of( modified_value, data[index])) {
      low_ix = index;
    }
    // Otherwise set the high-index to the current index; we cannot move to
    // the right of this index.
    // Note - in both cases, this index is still potentially valid.
    else {
      high_ix = index;
    }
    index = (high_ix + low_ix) / 2; // integer arithmetic
  }
}

/*****************************************************************************
determine_modified_value
Purpose:(Determine the value for the independent variable.)
Return: (true if a value is found within the domain and false if it
         lies outside the domain. With wrap-around on, a value initially
         outside the domain but wrapped into the domain will return true.)
*****************************************************************************/
bool
TableIndependentVariable::determine_modified_value()
{
  modified_value = variable;

  // if value is within bounds, don't need to wrap or limit
  if(  in_front_of( variable, back_value) &&
       in_back_of(  variable, front_value) ) {
    // Check for coming back in bounds from previously being out of bounds
    if (off_table_front) { // we are no longer off-table-front
      index = 0;
      off_table_front = false;
    }
    else if (off_table_back) { // we are no longer off-table-back
      index = size-2;
      off_table_back = false;
    }
  }

  // if value is out of bounds with Linear continuity, tag as out of bounds
  else if ( Linear == continuity ) {
    if( in_front_of( modified_value, front_value ) ) {
      tag_as_off_table_front();
    }
    else { // out of bounds back
      tag_as_off_table_back();
    }
    return false;
  }

  // if value is out of bounds with WrapAround continuity, wrap value
  else {
    modified_value -= delta * std::floor( (variable - min_value) / delta);
    // One final check that we are in-bounds and have not picked up some
    // numerical glitch.  This should not be reached.
    if ( in_back_of( modified_value, back_value) ||
         in_front_of( modified_value, front_value) ) {
      modified_value = front_value;
      index = 0;
      fraction = 0.0;
      return false;
    }
  }
  return true;
}

/*****************************************************************************
sweep_up
Purpose:(sweeps to higher indices)
*****************************************************************************/
void
TableIndependentVariable::sweep_up()
{
  // only get here is not in current bin.
  // The back bin (far "right") has index size-2, extending from the value
  // associated with ix=size-2 to ix=size-1.
  // We have already confirmed that the modified-data value is within the
  // allowed domain, so the highest index available is size-2.
  // Start by pushing up the index.
  ++index;
  while( index < size -2 &&
         in_back_of( modified_value, data[index+1])) {
    ++index;
  }
}

/*****************************************************************************
sweep_down
Purpose:(sweeps to lower indices)
*****************************************************************************/
void
TableIndependentVariable::sweep_down()
{
  // only get here is not in current bin.
  // The front bin has index 0, its domain runs from [index-0, index-1)
  // We have already confirmed that the modified-data value is within the
  // allowed domain so there should be no danger of running off the end of the
  // table.  Nevertheless, because index is unsigned, we really, really do not
  // want to be inadvertently decrementing from 0.  So make the check first,
  // even though it will be redundant on the first step.
  while (index > 0 &&
         in_front_of( modified_value, data[index])) {
    --index;
  }
}

/*****************************************************************************
tag_as_off_table_back
Purpose:(Sets the parameters appropriately for the case where the input data
         has gone off the end of the table, beyond the value at index size-1.)
*****************************************************************************/
void
TableIndependentVariable::tag_as_off_table_back()
{
  index = size-1;
  fraction = 0.0;
  off_table_front = false; // Just to be sure!
  off_table_back = true;
  prox_override = false;
}

/*****************************************************************************
tag_as_off_table_front
Purpose:(Sets the parameters appropriately for the case where the input data
         has gone off the end of the table, beyond the value at index 0.)
*****************************************************************************/
void
TableIndependentVariable::tag_as_off_table_front()
{
  index = 0;
  fraction = 0.0;
  off_table_back = false; // Just to be sure!
  off_table_front = true;
  prox_override = false;
}

/*****************************************************************************
generate_fraction
Purpose:(Generates the output-values for all dependent variables.)
*****************************************************************************/
void
TableIndependentVariable::generate_fraction()
{
  if ((index + 1) >= size) {
    // We should never hit this case, but protect against it just in case.
    CMLMessage::error(
      __FILE__, __LINE__, "Binary search error for independent variable ",
      name, ":\n",
      "The search has failed and we're attempting to interpolate off the "
      "table's back. Marking this variable as off the table's back.");
    tag_as_off_table_back();
    return;
  }
  fraction = (modified_value - data[index])/(data[index+1]-data[index]);

  // Make sure the arithmetic kept fraction within [0.0,1.0]
  // For these statements to be executed, the independent data
  //    must have been modified externally or have been corrupted.
  //    This is unlikely
  if (fraction < 0.0) {
    fraction = 0.0;
  }
  else if (fraction > 1.0) {
    fraction = 1.0;
  }
  // To support the discrete LookupMethods, check for proximity to a calibrated
  // point.  When variable is close to a calibrated point, PREV, NEXT, FLOOR,
  // CEIL should all take that point.  ROUND also does, but that is handled as
  // part of the rounding without additional logic.
  if (fraction < FRAC_EPS) {
    prox_override = true;
    index_prox = index;
  }
  else if (fraction > 1.0 - FRAC_EPS) {
    prox_override = true;
    index_prox = index+1;
  }
  else {
    prox_override = false;
  }
}

/*****************************************************************************
check_monotonicity
Purpose:(Checks whether the data is monotonic and identifies the direction
         in which its value increases.)
*****************************************************************************/
bool
TableIndependentVariable::check_monotonicity(
    const std::vector<double> & data_in)
{
  // If there is only 1 data value, assign
  //   table_values_increasing to be true.
  // Otherwise, set it to true if the second data element is larger than the
  // first and false if the second element is smaller (or equal to) the first.
  table_values_increasing = size ==1? true : data_in[1] > data_in[0];

  // Check for duplicated data values.  It is only necessary to check adjacent
  // values because the mononicity test (next) prevents values from reversing.
  for (size_t test_ix = 1; test_ix < size; ++test_ix) {
    if (MathUtils::is_near_equal(data_in[test_ix], data_in[test_ix-1])) {
      CMLMessage::error(
        __FILE__,__LINE__,"Data load error:\n",
        "Two or more of the values in the data table for independent variable ",
        name," are identical.\n"
        "Match found at indices ",test_ix-1," and ",test_ix," with value ",
        data_in[test_ix-1],".\n"
        "The independent array (data table) must be monotonic.\n");
      return false;
    }
    // Check monotonicity of the data.  It is only necessary to check that the
    // next data point has a value that is in the same direction as the
    // established trend.
    if ((data_in[test_ix] > data_in[test_ix-1]) != table_values_increasing) {
      CMLMessage::error(
        __FILE__,__LINE__,"Data error:\n",
        "The direction of increasing values in the data table for independent "
        " variable ",name," switched at index ",test_ix-1," with sequence ",
        data_in[test_ix-2]," : ",data_in[test_ix-1]," : ", data_in[test_ix],
        ".\nThe independent array (data table) must be monotonic.\n");
      return false;
    }
  }
  // Got to the end with no problems.  Data is good
  // Set these frequently used values so we don't have to calculate them
  // repeatedly
  front_value = data_in.front();
  back_value  = data_in.back();
  max_value   = std::max( front_value, back_value );
  min_value   = std::min( front_value, back_value );
  delta       = max_value - min_value;
  return true;
}
