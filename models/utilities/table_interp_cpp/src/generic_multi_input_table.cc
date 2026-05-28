/*******************************TRICK HEADER***********************************
PURPOSE:
  (definitions for the single-variable methods for the n-dimensional lookup tables)

PROGRAMMERS:
  (((Gary Turner) (OSR) (dec 2015) (Antares) (initial version))
   ((Bingquan Wang) (OSR) (aug 2017) (Antares) (IVV code cleanup and refactored))
  )
*******************************************************************************/

#include <cstddef> // NULL

#include "../include/generic_multi_input_table.hh"

// NOTE - using [index] rather than .at(index) to index STL-vectors primarily
//        because doing so is much faster.
//        The .at(index) notation is generally considered safer because it
//        includes checking for index being inside bounds; that checking is
//        managed in the code independently so that index is checked only
//        once per setting instead of at every usage, and the checking is
//        typically implicit to how the loop is configured rather than
//        explicit on the value itself.


/*****************************************************************************
Constructor
*****************************************************************************/
GenericMultiInputTable::GenericMultiInputTable()
  :
  trivial_case(true),
  output_ptrs_set(false),
  data_loaded(false),
  initialized(false),
  data(),
  size_of_dimension(),
  data_point_weight(),
  data_point_index(),
  num_data_elements_per_increment_of_index(),
  output(),
  independents()
{}
/****************************************************************************/
GenericMultiInputTable::GenericMultiInputTable(
          double *dependent_variables, // array of variables
          size_t num_vars)
  :
  GenericMultiInputTable()
{
  if (dependent_variables == nullptr) {
    CMLMessage::fail(__FILE__,__LINE__,"Construction error: ",
       "The input parameter \"dependent_variables\" cannot be NULL. \n");
  }
  for (unsigned int ii = 0; ii < num_vars; ++ii) {
    output.push_back(dependent_variables + ii);
  }
  output_ptrs_set = true;
}
/****************************************************************************/
GenericMultiInputTable::GenericMultiInputTable(
          double & dependent_var)
  :
  GenericMultiInputTable()
{
  output.push_back(&dependent_var);
  output_ptrs_set = true;
}
/****************************************************************************/
GenericMultiInputTable::GenericMultiInputTable(
          const DoublePtrVec & dependent_variables)
  :
  GenericMultiInputTable()
{
  populate_output(dependent_variables);
}

/*****************************************************************************
load_data
Purpose:(Loads the data into the class structure)
*****************************************************************************/
bool
GenericMultiInputTable::load_data(
            const double *data_in,
            const SizeVec &dim_list)
{
  // run common checks for validity
  if( !load_data_internal_check(dim_list)) {
    return false;
  }
  // Copy the input data to the class
  return copy_data(data_in);  //NULL check in copy_data()
}
/****************************************************************************/
bool
GenericMultiInputTable::load_data(
            const DoubleVec  & data_in,
            const SizeVec &dim_list)
{
  // run common checks for validity
  if( !load_data_internal_check(dim_list)) {
    return false;
  }
  // Copy the input data to the class
  return copy_data(data_in);
}

/*****************************************************************************
load_data_internal_check
Purpose:(Perform internal checks on data, common to both methods of loading
         data)
*****************************************************************************/
bool
GenericMultiInputTable::load_data_internal_check(
            const SizeVec &dim_list)
{
  if (dim_list.size() < 2) {
    CMLMessage::error(
      __FILE__,__LINE__,"Table data-load failure:\n",
      "There must be at least 2 dimensions to the data table so the\n"
      "2nd argument must be a vector of at least size 2.\n"
      "The first entry of this vector indicates how many dependent variables\n"
      "are populated from this table; each subsequent entry indicates how\n"
      "many calibration points there are for each of the independent "
      "variables.\nA vector without at least 2 entries is incomplete.\n"
      "Aborting load_data().\n");
    return false;
  }

  if (dim_list.at(0) != output.size()) {
    CMLMessage::error(
      __FILE__,__LINE__,"Table data-load failure:\n",
      "The specified number of dependent variables (",dim_list.at(0),
      ") does not match\n"
      "with the number of interpolation outputs (", output.size(), ").\n"
      "Aborting load_data().\n");
    return false;
  }

  if (data_loaded) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Table data-load error\n",
      "Data has already been loaded for this table.\n"
      "Rewriting data.\n");
    data.clear();
  }

  // data file is loaded as a single-dimensioned array; this model keeps track
  // of the range of indices at which each index on each dimension accesses
  // that array.  The size_of_dimension vector records the number of elements
  // on each dimension.
  size_of_dimension = dim_list;
  return true;
}

/*****************************************************************************
add_dependent
Purpose:(Adds a dependent variable.)
*****************************************************************************/
void
GenericMultiInputTable::add_dependent(
    double & new_dep_var)
{
  if (initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"Table-variable connection error:\n",
      "Cannot add new dependent variables after initialization.\n"
      "Addition failed.\n");
      return;
  }
  output.push_back(&new_dep_var);
  output_ptrs_set = true;
}

/*****************************************************************************
add_dependent_data
Purpose:(Adds a dependent variable and its associated data.)
*****************************************************************************/
void
GenericMultiInputTable::append_dependent_data(
    double & new_dep_var,
    const DoubleVec & data_vec,
    const SizeVec &dim_list)
{
  if (initialized) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot add new dependent variables after initialization.\n"
      "Addition failed.\n");
      return;
  }
  if (!data_loaded) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot append data to a table until it has data already loaded.\n"
      "Addition failed.\n");
      return;
  }
  if ( dim_list.size() != size_of_dimension.size()-1) {
    CMLMessage::error( __FILE__,__LINE__,
      "Dimension of new data array does not match that of existing data"
      "sub-array.\n"
      "New data is specified to have ",dim_list.size()," dimensions:\n"
      "Existing data is specified to have ", size_of_dimension.size()-1,
      "dimensions for each dependent variable:\n"
      "Addition failed.\n");
      return;
  }
  for (size_t ii = 0; ii < dim_list.size(); ++ii) {
    if ( dim_list[ii] != size_of_dimension[ii+1] ) {
      CMLMessage::error( __FILE__,__LINE__,
        "Number of elements in dimension ",ii," of new data does not\n"
        "match with that of existing data.\n"
        "New data specifies      ",dim_list[ii]," elements.\n"
        "Existing data specifies ",size_of_dimension[ii+1]," elements.\n"
        "Addition failed.\n");
        return;
    }
  }
  // Checks passed
  output.push_back(&new_dep_var);
  output_ptrs_set = true;
  size_of_dimension[0]++;
  data.insert(data.end(), data_vec.begin(), data_vec.end());
}

/*****************************************************************************
add_independent
Purpose:(Adds an independent variable to either:
         - the back of the list of independents, or
         - to a specified location)
*****************************************************************************/
void
GenericMultiInputTable::add_independent(
        TableIndependentVariable             & var,
        TableIndependentVariable::LookupMethod lookup_method)
{
  // simply add to the end of the list.
  add_independent( var, independents.size(), lookup_method);
}
//*****************************************************************************
void
GenericMultiInputTable::add_independent(
        TableIndependentVariable & var,
        size_t index,
        TableIndependentVariable::LookupMethod lookup_method)
{
  // Only succeed if model has not been initialized already
  if (initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"Table-variable connection error:\n",
      "Cannot add new independent variables after initialization.\n"
      "Addition failed.\n");
    return;
  }

  // size() is used multiple times, so just store it:
  const size_t size = independents.size();

  // check input for duplication
  for (size_t ii = 0; ii < size; ++ii) {
    if (&var == independents[ii].first) {
      CMLMessage::error(
        __FILE__,__LINE__,"Redundant configuration\n",
        "Attempted to add an TableIndependentVariable instance (name: ",
        var.get_name_char(),")\n"
        "to a GenericMultiInputTable at index ", index, "\n"
        "but the table already has access to that TableIndependentVariable, "
        "at index ",ii,"\n"
        "Aborting redundant addition.\n"
        "TableIndependentVariable remains at index ",ii,"\n");
      return;
    }
  }

  if (index < size) {
    if (independents[index].first != NULL) {
      CMLMessage::warn(
      __FILE__,__LINE__,"Table-variable connection error:\n",
      "Connecting dimension ",index, " of the table with an independent "
      "variable (", var.get_name_char(),").\n"
      "This dimension was previously populated with a different independent\n"
      "variable (",independents[index].first->get_name_char(),"\n");
    }
    independents[index] = IndepPair(&var, lookup_method);
  }
  else {
    // Fill the independents with NULL values up to the specified index.
    TableIndependentVariable * null_ptr = NULL;
    for (size_t ii = size; ii < index; ++ii) {
      independents.push_back(IndepPair(null_ptr, lookup_method));
    }
    // Then add the specified independent at the back.
    independents.push_back(IndepPair(&var, lookup_method));
  }
}

/*****************************************************************************
initialize
Purpose:(Checks for data availability and consistency)
*****************************************************************************/
bool
GenericMultiInputTable::initialize()
{
  if (initialized) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Redundant initialization:\n",
      "Called the initialize method redundantly.\n"
      "The initialize method has already completed successfully.\n");
    return true;
  }

  if (!data_loaded) {
    CMLMessage::error(
      __FILE__,__LINE__,"Initialization error:\n",
      "Called the initialize method before the data table\n"
      "has been populated with data.\n"
      "Must call load_data first.\n"
      "Initialization failed.\n");
    return false;
  }

  if (!output_ptrs_set) {
    CMLMessage::error( __FILE__,__LINE__,
      "initialization failed:\n",
      "The output pointers have not been set, data has nowhere to go.\n");
    return false;
  }

  // number of dimensions of data in the array:
  size_t data_dimension = size_of_dimension.size();
  // number of independent variables:
  size_t num_independents = independents.size();

  // NOTE:
  // The first "dimension" of the table is always reserved to provide 1
  // "level" per dependent variable.  So size_of_dimension[0] provides
  // the number of dependent variables populated by this table. This is
  // true even in the simplest case with only 1 dependent variable. The
  // second "dimension" contains data associated with the first independent
  // variable, with each additional independent variable adding another
  // "dimension".
  // E.g. think of this as an array:
  //   data[3][7][1]
  // addresses a value of the 4th (index-3) dependent variable when the
  // first independent variable has index 7 and the second independent
  // variable has index 1.
  if (num_independents != (data_dimension - 1)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Initialization error:\n",
      "The number of independent variables(",num_independents,
      ") is inconsistent with the\n"
      "dimension of the data table for each dependent variable (",
      data_dimension - 1,").\n"
      "Initialization failed.\n");
    return false;
  }
  // Now lock down the number of independents so we don't have to do that
  // again.
  for (size_t ii = 0; ii < num_independents; /*incremented internally*/) {
    // elements of independents comprise a pair:
    //   {pointer to the IndependentVariable, method used for that variable}
    // independents[ii].first is the pointer to the TableIndependentVariable.

    // Check for NULL pointer:
    if (independents[ii].first == nullptr) {
      CMLMessage::error(
        __FILE__,__LINE__,"Initialization error:\n",
        "The independent variable at location ",ii," in the independents vector"
        " has not been populated.\nThe pointer to it is still NULL.\n"
        "Initialization failed.\n");
      return false;
    }
    // Check that the TableIndependentVariable has data
    if (!independents[ii].first->is_data_loaded()) {
      CMLMessage::error(
        __FILE__,__LINE__,"Initialization error:\n",
        "The independent array at index ",ii," (named ",
        independents[ii].first->get_name_char(),") has not had data loaded."
        "\nInitialization failed.\n");
      return false;
    }
    // Check that the size of the TableIndependentVariable's data set matches
    // the size of the corresponding dimension of output data.
    if (independents[ii].first->get_size() !=
                         size_of_dimension.at(ii + 1)) {
      CMLMessage::error(
        __FILE__,__LINE__,"Initialization error:\n",
        "The size of independent variable[",ii,"] (",
        independents[ii].first->get_size(),") is inconsistent with the\n"
        "size of dimension[",ii+1,"] (",
        size_of_dimension.at(ii + 1),") of the data table.\n"
        "Initialization failed.\n");
      return false;
    }
    // Any independents with a single data point serve no purpose, they can
    // be removed from the table at this point.
    // First apply a sanity check.  This should be unreachable.  For this to be
    // hit, the size of the independent must have changed since dependent data
    // was loaded, but in that case the previous test would have picked it up.
    if (independents[ii].first->get_size() <= 1) {
      if (num_data_elements_per_increment_of_index[ii] !=
          num_data_elements_per_increment_of_index[ii+1]) {
        CMLMessage::fail(
          __FILE__,__LINE__,"Internal error:\n",
          "Failed sanity check on the structure of the "
          "num_data_elements_per_increment_of_index\n"
          "array for index ",ii,".\n"
          "Before removing this index, needed to check that it\n"
          "had not been wrongly factored into the indexing.\n"
          "This independent is registered with a single data point, but the\n"
          "indexing suggests that it should have ",
            num_data_elements_per_increment_of_index[ii] /
            num_data_elements_per_increment_of_index[ii+1],
          " data-points.\n");
      }
      // Sanity check passed, go ahead and remove it.
      CMLMessage::warn(
        __FILE__,__LINE__,"Single-valued Independent:\n",
        "At index ",ii,", the independent variable has only 1 data value.\n"
        "There is nothing to interpolate or look up for this value; it is\n"
        "being removed from the set of independents.\n");
      independents.erase(independents.begin()+ii);
      size_of_dimension.erase(size_of_dimension.begin()+ii+1);
      num_data_elements_per_increment_of_index.erase(
            num_data_elements_per_increment_of_index.begin()+ii+1);
      num_independents = independents.size();
      // Notes -
      //   - no increment, hold ii steady for next value which is now at
      //     the same index.
      //   - data_dimension also changes with the erasure of a dimension,
      //     but it isn't used again so does not need to be changed.
      //     data_dimension = size_of_dimension.size();
    }
    else { // with at least 1 independent, the lookup/interpolation process
           // will involve real calculations.
      ++ii; // move onto next item in vector.
      trivial_case = false;
    }
  }


  // If all inputs are 1-element in size, there is nothing to interpolate or
  // lookup.  Set the output once to the only data point for that variable and
  // leave it there.  Initialization of lookup is incomplete, updates will not
  // proceed.
  if (trivial_case) {
    generate_trivial_output();
  }
  else {
    configure_support_arrays();
  }
  initialized = true;
  return true;
}

/*****************************************************************************
update
Purpose:(The master executive call to generate the data)
*****************************************************************************/
bool
GenericMultiInputTable::update()
{
  if (!initialized) {
    CMLMessage::error(
      __FILE__, __LINE__, "Lookup error.\n",
      "Table has not been initialized; cannot proceed to perform interpolation "
      "or lookup.\n");
    return false;
  }
  if (trivial_case) { // nothing to do, already solved.
    // call this because something outside the table model could have changed
    // the value of any dependent variables.
    generate_trivial_output();
    return true;
  }

  generate_base_values();

  return generate_output();
}

/*****************************************************************************
generate_output
Purpose:(Interpolate between the various interpolation points.)
Notes: (( In the case that all independent variables are being interpreted
          as discrete values, there is no interpolation to conduct.
          In this case, the data_point_weight and data_point_index vectors
          are both limited to a single data point with a weight of 1.0.)
        ( In the case that all independent variables are being interpreted
          as continuous variables, there are 2^n interpolation points)
        ( In the case of a mix of discrete and continuous treatments, there
          will be 2^p interpolation points, where p = the number of continuous
          variables.  The (n-p) discrete variables will all collapse to a
          single combination, used at all 2^p interpolation points.)
       )
*****************************************************************************/
bool
GenericMultiInputTable::generate_output()
{
  // Run prechecks, abort output generation if prechecks fail.
  // However, there does not appear to be a way to fail the prechecks, so this
  // is a failsafe.
  if (!precheck_output()) {
    return false;
  }

  // Each dependent variable has an identically-sized data table embedded
  // within the full data table.
  // For each dependent variable, access the same relative data set,
  // multiplying each of these raw data values by its weighting;
  // increment them to produce the total output for that dependent variable.

  const size_t num_data_points_interp = data_point_weight.size();
  size_t index_basis = 0;// index_basis marks the first data point
                         // for each dependent variable

  // For each dependent variable:
  DoublePtrVec::iterator out_iterator = output.begin();
  for (; out_iterator != output.end(); ++out_iterator) {
    (**out_iterator) = 0.0; // initialize output value to 0.0 for increments

    // For each of the 2^n interpolation points
    for (size_t jj = 0; jj < num_data_points_interp; ++jj) {
      //calculate the correct index in the data array.
      //  NOTE - the loop is over the size of the data_point_weight
      //         but the index is also used to access data_point_index.
      //         The sizes of the two vectors have been confirmed
      //         to match in precheck_output().
      const size_t data_index = index_basis + data_point_index[jj];
      (**out_iterator) += data_point_weight[jj] * data[data_index];
    }
    // Move on to the next dependent variable.
    index_basis +=  num_data_elements_per_increment_of_index[0];
  }
  return true;
}

/*****************************************************************************
precheck_output
Purpose:(Checks that should be made for all inherited versions of
         generate_output)
*****************************************************************************/
bool
GenericMultiInputTable::precheck_output()
{
  // Going to be looping through both the data_point_weight and
  // data_point_index STL-vectors.  These are always assigned to the same
  // size, and there is no reason they would not be the same size.  But check
  // it anyway in case the code gets modified at a later date.
  if (data_point_index.size() != data_point_weight.size()) {
    CMLMessage::error(
      __FILE__,__LINE__,"Internal error:\n",
      "The sizes of the data_point_weight and data_point_index vectors "
      "differ.\nThey should always be identical, something has gone wrong\n"
      "in the assignment to these two vectors.\n"
      "Continuing runs the risk of accessing data out of bounds.\n"
      "Aborting table-lookup\n");
    return false;
  }
  return true;
}

/*****************************************************************************
generate_trivial_output
Purpose:(Extract only the basic data without any interpolation.
         Called only as a result of a failed initialization process when it
         has been identified that all of the independent variables have only
         1 value, and the data array contains only 1 value for each output
         variable.)
*****************************************************************************/
void
GenericMultiInputTable::generate_trivial_output()
{

  if (!output_ptrs_set) {
    // NOTE this is a safety check, it may not be reachable.
    // This method is called from update, which requires initialization, which
    //  requires certain configuration checks.  To get here, the model would have
    //  had to pass through initialization without having gone through
    //  add_dependents or populate_output (both set output_ptrs_set = true),
    //  but initialization cannot succeed if the output vector is empty and those
    //  are the only ways to populate the output vector.
    CMLMessage::error(
      __FILE__,__LINE__,"generate output error:\n",
     "The output pointers have not been set, data has nowhere to go.\n");
    return;
  }

  for (size_t ii = 0; ii < size_of_dimension.size(); ++ii) {
    *output[ii] = data[ii];
  }
}

/*****************************************************************************
populate_output
Purpose:(Store off a collection of output variables)
*****************************************************************************/
void
GenericMultiInputTable::populate_output(
       const DoublePtrVec &var_ptr_list)
{
  if (output_ptrs_set) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Construction error:\n",
      "The output pointers have already been set.\n"
      "This action will remove all current output pointers.\n");
  }

  const size_t num_vars = var_ptr_list.size();
  if (!num_vars) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Construction error:\n",
     "There should be at least 1 output for each table.  A zero-sized vector\n"
     "was passed in for configuring table outputs.\n");
  }

  // Check the dependent-variable pointers for NULL; if they all pass,
  // record them permanently in the output vector.
  for (size_t ii = 0; ii < num_vars; ++ii) {
    if (var_ptr_list[ii] == nullptr) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Construction error:\n",
        "Table must be provided with pointers to the variables to be "
        "populated.  'nullptr' is not a valid target.\n");
    }
  }
  // All outputs are legit.  Assign them and record that they are legit so
  // that we don't have to keep testing them all.
  output = var_ptr_list;
  output_ptrs_set = true;
}

/*****************************************************************************
copy_data
Purpose:(configures the data, allocating the necessary blocks)
*****************************************************************************/
bool
GenericMultiInputTable::copy_data(
           const double *data_in)
{
  if (data_in == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Table data-load error\n",
      "No data is given for the data-load; its pointer is NULL.\n"
      "Without data, this table cannot function.\n"
      "Aborting table configuration.\n");
    return false;
  }
  // Configure internal data structure, abort on error
  size_t total_data_elements = configure_internal_data_structure();
  if (total_data_elements == 0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Table data-load error\n",
      "The internal data structure contains no elements."
      "Without data, this table cannot function.\n"
      "Aborting table configuration.\n");
    return false;
  }

  // Now we have the total number of data elements that should be contained in
  // the data_in array.  It should be equal to the product of the multiple
  // dimensional sizes.
  data.resize(total_data_elements);
  data.assign(data_in, data_in+total_data_elements);
  data_loaded = true;
  return true;
}
/****************************************************************************/
bool
GenericMultiInputTable::copy_data(
           const DoubleVec & data_in)
{
  // Configure internal data structure, abort on error
  size_t total_data_elements = configure_internal_data_structure();
  if (total_data_elements == 0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Table data-load error\n",
      "The internal data structure contains no elements."
      "Without data, this table cannot function.\n"
      "Aborting table configuration.\n");
    return false;
  }

  if (data_in.size() != total_data_elements) {
    CMLMessage::error(
      __FILE__,__LINE__,"Table data-load error\n",
      "Size of the incoming data is inconsistent with the specified "
      "dimensions.\n"
      "-- Incoming data has ", data_in.size(), " elements.\n"
      "-- Specified dimension sizes require ", total_data_elements,
      "elements (distributed over ",size_of_dimension.size(), " dimensions.\n"
      "Aborting load-data.\n");
    num_data_elements_per_increment_of_index.clear();
    return false;
  }
  data = data_in;
  data_loaded = true;
  return true;
}

/*****************************************************************************
configure_internal_data_structure
Purpose:(Sets the location of the internal boundaries to divide the 1-d data
         storage into its constituent dimensions.)
*****************************************************************************/
size_t
GenericMultiInputTable::configure_internal_data_structure()
{
  const size_t data_dimension = size_of_dimension.size();
  // The n-dimensional data array gets loaded up into a 1-d vector.
  // It is vital to know how many places in the vector to skip over when the
  // index ticks up on an arbitrary dimension.
  // When the last index ticks up by 1, the vector advances one position, so
  // start with the last index.
  // e.g. data[2][4][3] the vector will advance:
  //       1 position when the 3rd index upticks
  //       3 positions when the 2nd index upticks
  //       3*4=12 positions when the 1st index upticks
  num_data_elements_per_increment_of_index.resize(data_dimension);
  size_t total_data_elements = 1;
  size_t index = data_dimension;
  // Note: this is done as a do-while loop instead of a for-next loop because
  // index is of type size_t which is unsigned. A loop such as:
  //     for ( index = data_dimension - 1; index >= 0; index-- )
  // would fail on the final pass.
  do {
    --index;
    num_data_elements_per_increment_of_index[index] = total_data_elements;
    if (0 == size_of_dimension[index]) {
      CMLMessage::error(
        __FILE__,__LINE__,"Table data-load error\n",
        "The dimension-size at index ",index," shoudn't be zero.\n"
        "Critical error.  Aborting data configuration.\n");
      num_data_elements_per_increment_of_index.clear();
      return 0;
    }
    total_data_elements *= size_of_dimension[index];
  } while (index>0);
  return total_data_elements;
}

/*****************************************************************************
configure_support_arrays
Purpose:(configures the data, allocating the necessary blocks)
NOTES: called from initialize, after verification that the number of
       independents matches with the data tables
*****************************************************************************/
void
GenericMultiInputTable::configure_support_arrays()
{
  // The interpolation takes place in n dimensions (where n is the number of
  // independent variables), across 2^n data points.  Each of those data
  // points will receive a certain weighting, according to the proximity of
  // the reference point to the actual data.
  //   e.g. in 2-dimensions:
  //     in first dimension, the data point is 0.6 through the intervali
  //     in second dimension, the data point is 0.3 through the interval
  //   data-point 0 (00) gets a weight (0.7)(0.4) = 0.28
  //   data point 1 (01) gets a weight (0.3)(0.4) = 0.12
  //   data point 2 (10) gets a weight (0.7)(0.6) = 0.42
  //   data point 3 (11) gets a weight (0.3)(0.6) = 0.18
  // So we need an array to store the computed weights; that array contains
  // 2^n data points:

  // 2^n:
  const size_t num_data_points_interp = size_t(1) << independents.size();

  data_point_weight.resize(num_data_points_interp);
  data_point_index.resize(num_data_points_interp);
}

/*****************************************************************************
generate_base_values
Purpose:(Uses the independent variable fractions to weight the data points
         appropriately.)
*****************************************************************************/
void
GenericMultiInputTable::generate_base_values()
{
  // First go through the list of the independents, computing how many
  // interpolation points are needed for this dependent variable.
  size_t num_independents_interp = 0;
  for( IndepPair independent : independents) {
    if (  independent.second == TableIndependentVariable::Interp &&
         !independent.first->is_off_table()) {
      ++num_independents_interp;
    }
  }
  // Note - carry on even if there is nothing to interpolate.  It is still
  //        necessary to generate the index of the single data point used
  //        for a simple lookup.


  // We have n independent variables, some subset m of which are to be used
  // for interpolation, and the remainder (i.e. n-m) are direct lookups.
  // (Note: m in range [0,n], m=num_independents_interp).
  // Have to interpolate between 2^m points, distributed across n dimensions
  // of data.
  // We need to get the index of each of those data points, and the weight
  // that is to be applied to each for taking the average of them.
  // Start by populating the STL-containers with the correct number of
  // points.
  const size_t num_data_points_interp = size_t(1) << num_independents_interp;
  data_point_weight.assign(num_data_points_interp, 1.0);
  data_point_index.assign(num_data_points_interp, 0);

  size_t dwell = 1;
  // start on the dimension=1, i.e. the first independent variable
  // Note dimension=0 corresponds to the independnt variables.
  // Increment current_dimension as independents are processed.
  size_t current_dimension = 1;

  for( IndepPair independent : independents) {
    TableIndependentVariable & TIV = *independent.first;
    // Trivial case, the independent variable has 1 (or fewer, if that is
    // possible) data point.
    // NOTE - this should not be possible; such an independent should have
    //        already been stripped from the table at initialization.
    if (TIV.get_size() <= 1) {
      // No action; index for this independent is zero so there is nothing to
      // add to the array index and cannot interpolate so no weighting to
      // include.
      continue;
    }

    const size_t index_multiplier =
          num_data_elements_per_increment_of_index[current_dimension];
    size_t index = TIV.get_index();
    bool index_processed = false;

    // Switch on the Lookup Method for how THIS table uses each independent:
    // Note - this is a table-specific value; a single independent may be used
    //        in different ways in different tables.
    switch (independent.second) {
    case TableIndependentVariable::Interp:
      {
      // Only interpolate if there are at least 2 points (already checked)
      // and if the independent variable is still in its table.
      // Otherwise, just use the end-point data at the default index
      if( TIV.is_off_table())  {
        break; // use default index.  No other actions
      }

      // else: special case -- add to the interpolation.
      if (dwell >= num_data_points_interp) {
        CMLMessage::fail(
          __FILE__,__LINE__,"Data Processing error.\n",
          "The value `dwell` (",dwell,") is too large.\n"
          "It can be at most one-half of the number of interpolation points (",
          num_data_points_interp, ").\n"
          "This message is a fail-safe.  It should never be seen.\n");
      }
      // For each of the interpolation points (jj), apply frac weight to the
      //   upper index and (1-frac) weight to the lower index for the
      //   independent variable currently being processed.
      // Alternate between lower and upper indices according to which
      //   independent variable is being worked.
      // The overall goal here is to apply an identifying pattern to the
      //   points in the interpolation-point set, processing one independent
      //   variable at a time.
      //   Apply the pattern (l, u, l, u, l, ...) during the 1st independent
      //   next, go with     (l, l, u, u, l, ...) for the 2nd
      //   then go with      (l, l, l, l, u, ...) for the 3rd
      //   etc.
      //   to produce a unique pattern of {u,l} on each of the interpolation
      //   points: {lll, ull, lul, uul, llu, ulu, luu, uuu}
      // "dwell" provides the dwell-time on each index for the given
      //   independent variable.
      // Notes:
      // - fraction measures the "distance" from index, so if fraction = 0.0,
      //   all the weight (1.0) should be applied to index and none (0.0) to
      //   index+1.
      //   Hence the weighting for the point at index+1 is fraction, and the
      //   weighting for the point at index is 1-frac.
      // - The data_point_index value is the location in the data STL-vector
      //   where the interpolation data-value can be found (for each of the
      //   interpolation points). Because the data vector contains data asa
      //   function of ALL independent variables (even the ones using a "lookup"
      //   rather than an "interpolation"), this index is not finalized for any
      //   data point until all independent variables have processed.
      //   The index value is incremented as each independent gets processed.
      double weight_upper  = TIV.fraction;
      double weight = 1-weight_upper;
      size_t index_upper = index+1;
      for (size_t jj=0; jj<num_data_points_interp; ) {
        // Apply the values from the lower index, applying the parameters to
        // "dwell" points before moving to the upper index.
        // Note:  - jj is incremented in this (and the next) embedded loop,
        //          not in the jj for-loop statement
        for (size_t kk=0; kk<dwell; ++kk, ++jj) {
          data_point_weight[jj] *= weight;
          data_point_index[jj] += index * index_multiplier;
        }
        // apply the values from the upper index.  After another "dwell"
        // points, test whether all points are done and go back to the lower
        // index if not.
        for (size_t kk=0; kk<dwell; ++kk, ++jj) {
          data_point_weight[jj] *= weight_upper;
          data_point_index[jj] += index_upper * index_multiplier;
        }
      }
      dwell *= 2; // set for next iterating dimension
      index_processed = true;
      break;
      }

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
        __FILE__,__LINE__,"Invalid specification\n",
        "Specification of LookupMethod is invalid.\n");
      break;
    }

    // If the data indices have not been adjusted by this independent, do so
    // now.
    if (!index_processed) {
      for (size_t jj=0; jj<num_data_points_interp; ++jj) {
        data_point_index[jj] += index * index_multiplier;
      }
    }

    ++current_dimension; // Move on to the next independent variable.
  } // end for loop through all independents
}

/*****************************************************************************
bias_data
Purpose:(Bias specified elements in the data array by the specified offset.)
*****************************************************************************/
void
GenericMultiInputTable::bias_data(
        double bias,
        size_t ix_start,
        size_t ix_stop)
{
  if (index_checks(ix_start, ix_stop, "bias")) { return; }
  for (size_t ii = ix_start; ii <= ix_stop; ++ii) {
    data[ii] += bias;
  }
}

/*****************************************************************************
scale_data
Purpose:(scale specified elements in the data array by the specified factor.)
*****************************************************************************/
void
GenericMultiInputTable::scale_data(
        double scale,
        size_t ix_start,
        size_t ix_stop)
{
  if (index_checks(ix_start, ix_stop, "scale")) { return; }
  for (size_t ii = ix_start; ii <= ix_stop; ++ii) {
    data[ii] *= scale;
  }
}

/*****************************************************************************
index_checks
Purpose:(Index checks common to bias_data and scale_data.)
*****************************************************************************/
bool
GenericMultiInputTable::index_checks(
        size_t & ix_start,
        size_t & ix_stop,
        std::string func)
{
  // Note - data_loaded implies data.size() > 0.
  if (!data_loaded) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid external initialization sequence\n",
      "Call made to ",func," data but data has not yet been loaded.\n"
      "There are no data to ",func,".\n"
      "Check sequencing.\n");
    return true;
  }
  if (ix_start > ix_stop) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid arguments\n",
      "Call made to ",func," data between two indices with the start index (",
      ix_start,")\n"
      "higher than the stop index (",ix_stop,").  This could be an error.\n"
      "Will ",func," the data values between these indices.\n");
    size_t ix_scratch = ix_start;
    ix_start = ix_stop;
    ix_stop = ix_scratch;
  }
  if (ix_stop >= data.size()) {
    CMLMessage::warn(__FILE__, __LINE__, "Invalid index\n",
      "Call made to ",func," data with the stop index (",ix_stop,
      ") past the end of the list.\n"
      "Will ",func," all data between the start index and\n"
      "the end of the list.\n");
    if (ix_start >= data.size()) { return true; }
    ix_stop = data.size() - 1;
  }
  return false;
}
