/*******************************TRICK HEADER******************************
PURPOSE: (Provides a table-lookup-set when the data is presented in a
          transpose  presentation.  The format assumed for data presentation
          by the table-interpolation model is variable-by-variable:
              {var1(x_1), var1(x_2), var1(x_3), ...,
               var2(x_1), var2(x_2), var2(x_3), ...,
               var3(x_1), var3(x_2), var3(x_3), ...}
          This format easily supports multi-variable interpolation.
          However, in the case of only one independent variable, the data may
          often be presented line-by-line based on the value of the independent
          variable:
            {{var1(x_1), var2(x_1), var3(x_1), ...}
             {var1(x_2), var2(x_2), var3(x_2), ...}
             {var1(x_3), var2(x_3), var3(x_3), ...}}

          This class supports data presented in the latter format.  The data
          may be subdivided into as many tables as necessary or desired.

PROGRAMMERS:
  (((Gary Turner) (OSR) (Nov 2017) (Antares) (initial)))
**********************************************************************/
#include <fstream> // ifstream
#include <sstream> // istringstream
#include <iterator>// istream_iterator
#include <cstring> // strcmp
#include <algorithm> // max_element


#include "../include/table_lookup__transpose_data.hh"

/*****************************************************************************
constructors
*****************************************************************************/
TableLookupTransposeDataSet_TableConfig::
                                     TableLookupTransposeDataSet_TableConfig()
  :
  name(),
  block_independent_addition(false),
  index_low(0),
  index_high(0),
  indices(),
  table_ptr(nullptr),
  dependent_variables(),
  table_type(AbstractTableLookup::Generic),
  lookup_method( TableIndependentVariable::Interp),
  data_scratch(),
  num_variables(0),
  total_num_lines(0)
{}
/****************************************************************************/
TableLookupTransposeDataSet::TableLookupTransposeDataSet()
  :
  table_config(),
  independent_var(nullptr),
  indep_continuity(TableIndependentVariable::Linear),
  populate_independent_from_file(false),
  indep_index(0),
  min_length(0),
  min_length_line(0),
  data_processed(false)
{}

/*****************************************************************************
initialize
Purpose:
  Initializes the internal values of the TableLookupTransposeDataSet_TableConfig
Notes:
  Called from TableLookupTransposeDataSet::process_data() after the same method
  has called this class's check_indices() method.
  It can be assumed that the indices vector is populated regardless of whether
  the indices were specified asa vector or a [low,high] range.
*****************************************************************************/
void
TableLookupTransposeDataSet_TableConfig::initialize( size_t num_lines)
{
  total_num_lines = num_lines;
  num_variables = indices.size();
  // check that num_variables matches dependent_variables.size()
  if (num_variables != table_ptr->get_num_dep_vars()) {
    CMLMessage::fail( __FILE__,__LINE__,
      "Invalid configuration\n",
      "Table is configured to extract data for ",num_variables,
      " dependent variables from the data file.\n"
      "Table is configured to populate ",
      table_ptr->get_num_dep_vars()," dependent variables with those data.\n"
      "These numbers are inconsistent.\n");
  }
  data_scratch.resize( num_variables * total_num_lines);
}
/*****************************************************************************
check_indices
Purpose:
- Compares index_low, index_high and the indices vector for consistency.
- If user has specified the indices vector, use those values and extract the
  maximum value of those indices for later verification that the data file
  contains sufficient data to read to that index.
- Otherwise, rely on the {index_low, index_high} values to specify the range
  of indices:
  - if index_high < index_low, switch them
  - build the indices vector from all values between index_low and index_high
    (inclusive)
 Notes:
  Called from TableLookupTransposeDataSet_TableConfig::process_data()
*****************************************************************************/
void
TableLookupTransposeDataSet_TableConfig::check_indices()
{
  if (!indices.empty()) {
    index_high = *std::max_element(indices.begin(),indices.end());
  }
  else {
    if (index_high < index_low) {
      size_t index_scratch = index_high;
      index_high = index_low;
      index_low = index_scratch;
    }

    size_t size = index_high - index_low + 1;
    indices.resize(size);
    for (size_t ii = 0; ii < size; ++ii){
      indices[ii] = index_low + ii;
    }
  }
}

/*****************************************************************************
parse_line
Purpose:(Parses a line of data, extracting the appropriate pieces.)
*****************************************************************************/
void
TableLookupTransposeDataSet_TableConfig::parse_line(
    size_t line_index, // first line is numbered 0
    const DoubleVec & data_line)
{
  // take the data off this line and put it in the scratch storage space.
  for (size_t ix = 0; ix < num_variables; ++ix) {
    data_scratch[ ix*total_num_lines + line_index] = data_line[indices[ix]];
  }
}

/*****************************************************************************
populate_table
Purpose:(creates a GenericMultiInputTable with the specified dependent
         variables and specified table-type in the event that the table was
         not created externally.)
*****************************************************************************/
void
TableLookupTransposeDataSet_TableConfig::populate_table()
{
  // Note - By this point in the sequence, table_ptr should be pointing
  // to something real or the code should have already failed.
  // This message may be unreachable for as long as all earlier faults
  // remain critical (terminal) faults.  But the ptr should be
  // checked here to safeguard against the risk that future changes,
  // especially in create_table(...) may allow a nullptr assignment.
  if (table_ptr == nullptr) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid table management sequence\n",
    "Allocation for a GenericMultiInputTable failed.\n");
  }
  // define internal sizing of vector
  SizeVec dim_list(2);
  dim_list[0] = num_variables;
  dim_list[1] = total_num_lines;
  // load processed data onto table.
  table_ptr->load_data( data_scratch, dim_list);
}


/*****************************************************************************
process_data
Purpose:(Main access point.
         Process the transposed data expressed as a list of vectors,
         dispersing the data to the appropriate tables)
NOTES: (It is not assumed that all of the incoming data is sent to tables;
        redundant data will be ignored.)
       (Call to add_independent_variable(TableIndependentVariable &) must be
        made before this call if it is going to be made externally.
        Otherwise, a new TableIndependentVariable will be created internally.)
*****************************************************************************/
bool
TableLookupTransposeDataSet::process_data(
    const DoubleVecList & data_file)
{
  if (!enabled) {
    CMLMessage::error(
      __FILE__,__LINE__,"Configuration error.\n",
      "Instruction received to process data but the model has been disabled.\n"
      "Data-processing skipped.\n");
    return false;
  }

  if (initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"Execution sequence error.\n",
      "Processing the data after the tables have been initialized\n"
      "with a different data set.\n"
      "Retaining original data.\n");
    return false;
  }


  // Get a count of the number of lines and length of the shortest line
  size_t data_lines_total_count = data_file.size();
  min_length = data_file.front().size();
  size_t line_num = 1;
  for (const DoubleVec & line_iter : data_file) {
    if (line_iter.size() < min_length) {
      min_length = line_iter.size();
      min_length_line = line_num;
    }
    ++line_num;
  }

  // Find the max specified index, start with indep_index and check the
  // TableConfigs:
  size_t max_index = indep_index;
  for (TableLookupTransposeDataSet_TableConfig & config_iter : table_config) {
    config_iter.check_indices();
    max_index = std::max( max_index, (config_iter.index_high));
  }

  // Verify that the data file is adequately sized
  if ( max_index >= min_length) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid independent data\n",
    "At line ",min_length_line,", there are ",min_length," data elements.\n"
    "The highest specified index for data extraction is ",max_index,
    "(requires ",max_index+1," data elements).\n"
    "There are insufficient data elements to obtain the\n"
    "data for all variables for this line of data.\n");
  }

  //***************************************************************************
  // PART A: Check and process independent data.
  //***************************************************************************

  // verify that there is 1 and only 1 independent.  If there is none, create
  // one.
  check_independent();

  // If the data file includes the data for the independent variable, parse it
  // out:
  if (populate_independent_from_file) {
    DoubleVec indep_data( data_lines_total_count);
    line_num = 0;
    for (const DoubleVec & line_iter : data_file) {
      // already confirmed that all lines are adequately sized to contain
      // this datum
      indep_data[line_num] = line_iter[indep_index];
      ++line_num;
    }
    // Push data onto the TableIndependentVariable.
    if (!independents[0]->load_data( indep_data)) {
      return false;
    }

  }


  //***************************************************************************
  // PART B: Check and process dependent data.
  //***************************************************************************
  // Verify that no tables have already been assigned to this manager:
  if (tables.size() != 0) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Unexpected configuration.\n",
      "Found data tables already added to this manager.\n"
      "Having externally-added tables is unexpected, but not critical.\n"
      "Make sure any externally-added tables are correctly associated with\n"
      "this manager's independent variable.\n");
  }

  // for each table-config instance:
  //   - create (and add) the GenericMultiInputTable (or its derivative) if
  //     not already created, or
  //   - add the table (if it already exists) to this manager
  //   - initialize it with information about the incoming data
  //   - send the incoming data line-by-line for parsing
  //   - populate the new table with the parsed data
  //   - associate the newly added table and the independent.
  for (TableLookupTransposeDataSet_TableConfig & config_iter : table_config) {
    if (config_iter.table_ptr == nullptr) {
      // "dependent_variables" is size-checked and elements are NULL-checked
      //  down-stream.
      config_iter.table_ptr = AbstractTableLookup::create_table(
                                              config_iter.dependent_variables,
                                              config_iter.table_type);
      associate_table_and_independent( config_iter.lookup_method);
    }
    else if (!config_iter.block_independent_addition) {
      add_table( *(config_iter.table_ptr));
      associate_table_and_independent( config_iter.lookup_method);
    }
    // else, no action needed -- table is configured for indirect processing
    // only and blocked from direct processing.
    // Specifically, DO NOT CALL associate_table_and_independent

    config_iter.initialize( data_lines_total_count );

    size_t line_index = 0;
    for (const DoubleVec & line_iter : data_file) {
      config_iter.parse_line( line_index,
                              line_iter );
      ++line_index;
    }

    config_iter.populate_table();

//    associate_table_and_independent( config_iter.lookup_method);
  }
  data_processed = true;
  return true;
}

/*****************************************************************************
process_data
Purpose:(processes a text file into the configured tables.
         Front-end to the main process_data method which takes a list of
         vectors.)
*****************************************************************************/
bool
TableLookupTransposeDataSet::process_data(
    const std::string & filename,
    size_t terminate_on_line)
{
  // create a list of vectors of data; this will be gradually built up
  // from the data file.
  DoubleVecList data_read;

  // if conversion works, process the list of vectors, otherwise return the
  // failure signal.
  if (convert_txt_to_list_vec( filename,
                               data_read,
                               terminate_on_line)) {
    return process_data( data_read);
  }
  else {
    return false;
  }
}

/*****************************************************************************
convert_txt_to_list_vec
Purpose:(reads a text file organized as follows:
           - each line represents a single calibration point for all dependent
             variables; the value of the independent variable may also be
             included.
           - each column consistently represents values of one variable.
         converts that text file into a list of vectors, with each vector
         representing one line of the text file.)
*****************************************************************************/
bool
TableLookupTransposeDataSet::convert_txt_to_list_vec(
    const std::string & filename,
    DoubleVecList & data_list, // output
    size_t terminate_on_line)
{
  std::ifstream file_in( filename);
  /* Check to make sure file is readable */
  if ( !file_in.is_open() ) {
    CMLMessage::error( __FILE__,__LINE__,
    "Invalid file selection\n"
    "Filename ",filename," could not be opened.\n"
    "Disabling model.\n");
    disable();
    return false;
  }


  // Local variable to monitor the number of variables per line:
  size_t prev_num_data = 0;// number of values on previous line of data

  // Reusable local variables representing current line:
  std::string line;
  DoubleVec line_vals;

  // As long as there are lines in the file:
  // - read the line to a std::string,
  // - send that string to istringstream for splitting into a vector of
  //   individual values
  // - push that vector of data values onto the accumulating list of vectors.
  // Run one line outside the loop to obtain the initial size
  std::getline( file_in, line );
  std::istringstream istream( line );
  line_vals.assign( ( std::istream_iterator<double>( istream ) ),
                      std::istream_iterator<double>() );
  data_list.push_back(line_vals);

  // count the number of values in this line, assign it to the local
  // variable-counters
  prev_num_data = min_length = line_vals.size();
  min_length_line = 1;


  // then run all the other lines.
  size_t line_num = 1;
  while ( std::getline( file_in, line )) {
    // Test for file truncation:
    ++line_num;
    if (line_num > terminate_on_line) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Data read: file length exceeded specifications.\n",
        "WARNING: Maximum number of data sets (",terminate_on_line,
        ") reached while reading '",filename,"'.\n"
        "Stopping reading at this point.\n");
      break;
    }

    std::istringstream istream_( line );
    // assign the elements of the split line to the vector line_vals.
    // Note:  "assign" deletes the previous entries so this gets the current
    //        line only.
    line_vals.assign( ( std::istream_iterator<double>( istream_ ) ),
                        std::istream_iterator<double>() );
    data_list.push_back(line_vals);

    // Error checking:
    // Irregular line length:
    if (line_vals.size() != prev_num_data) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Inconsistent data file\n",
        "Previously found ",prev_num_data," values per line in file '",
        filename,"'.\n"
        "This line contains ",line_vals.size()," values.\n");
      prev_num_data = line_vals.size();
    }
  }
  return true;
}

/*****************************************************************************
get_config
Purpose:(Returns a pointer to one of the entries in table_config.
         Used for accessing elements by name to edit pre-set values.)
*****************************************************************************/
TableLookupTransposeDataSet_TableConfig *
TableLookupTransposeDataSet::get_config( const char * name_in)
{
  std::string search_name(name_in);
  return get_config( search_name);
}
/****************************************************************************/
TableLookupTransposeDataSet_TableConfig *
TableLookupTransposeDataSet::get_config( const std::string & name_in)
{

  std::vector<TableLookupTransposeDataSet_TableConfig>::iterator table_it =
                                                           table_config.begin();
  size_t num_matches = 0;
  TableLookupTransposeDataSet_TableConfig * ret_val = nullptr;

  for (; table_it != table_config.end(); ++table_it) {
    if (name_in.compare((*table_it).name) == 0) {
      num_matches++;
      if (ret_val == nullptr) {
        ret_val = &(*table_it);
      }
    }
  }
  if (num_matches == 0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Failed search.\n",
      "No table config with name ",name_in," was found.\n"
      "Returning NULL (nullptr).\n");
  }
  else if (num_matches > 1) {
    CMLMessage::error(
      __FILE__,__LINE__,"Failed search.\n",
      "Found ",num_matches," table config instances with name ", name_in,".\n"
      "Returning the first one.\n");
  }
  return ret_val;
}

/*****************************************************************************
check_independent
Purpose:(Checks the presence of the independent variable and creates it if
         necessary)
*****************************************************************************/
void
TableLookupTransposeDataSet::check_independent()
{
  // The manager can handle only one independent.  If there are more than one
  // already added, that's a problem.
  if (independents.size() > 1) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid independent data\n",
      "This type of table-manager can only use one independent variable.\n"
      "For multiple independents, use a TableLookupSet.\n"
      "Automated parsing of multi-input data is not supported\n");
    return;
  }

  // If none have been added, or an invalid instance has been added,
  // create a new TableIndependentVariable.
  if (independents.size() == 0 || independents.at(0) == nullptr) {
    // Must have knowledge of the independent variable
    if (independent_var == nullptr) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid independent data\n",
        "The table-manager has no record of a TableIndependentVariable,\n"
        "and no specification of which variable to use to create one.\n"
        "Either:\n"
        "- create a TableIndependentVariable and add it to the manager's\n"
        "  'independents' vector via the add_independent_variable method, or\n"
        "- specify the manager's 'independent_var pointer' to allow the\n"
        "  automated creation and addition of a TableIndependentVariable.");
    }
    independents.clear();
    create_independent_variable("Automatic",
                                *independent_var,
                                indep_continuity);
  }
  // Now have 1 TableIndependentVariable loaded onto the vector.
}

/*****************************************************************************
remove_config
Purpose:(Attempts to remove a configuration from the vector.)
Limitation:(Don't do this unless an absolutely last resort.  Adding a config
            *copies* that config, itdoesn't just add a pointer to it.  So
            removing a config means matching up a config's parameters, which is
            non-trivial and risky.)
*****************************************************************************/
bool
TableLookupTransposeDataSet::remove_config(
   TableLookupTransposeDataSet_TableConfig & config_remove)
{
  if (data_processed) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Redundant call.\n",
      "Call made to remove a configuration, but the configuration has\n"
      "already been processed.  There is nothing to do here.\n");
    return false;
  }
  size_t num_matches = 0;
  size_t ix = 0;
  size_t ix_del = 0;
  /* For each config in the table_config vector, test whether the:
     - indices match (test indices vector or hi/lo depending on which is set)
     - name matches
     - table-pointer matches
     If any of these fail to match, the stored config is not a match with the
     one assigned for removal.
  */
  for (const auto & config : table_config) {
    bool indices_match = (config.indices.empty())?
                            ((config_remove.index_low == config.index_low) &&
                             (config_remove.index_high == config.index_high))
                            :
                            (config_remove.indices == config.indices);
    if (   indices_match
        && (config_remove.name == config.name)
        && (config_remove.table_ptr == config.table_ptr)) {
      num_matches++;
      ix_del = ix;
    }
    ix++;
  }
  if (num_matches == 0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Failed search.\n",
      "Call made to remove a configuration, but no table config with\n"
      "matching parameters was found.\n"
      "Removal failed.\n");
      return false;
  }
  else if (num_matches > 1) {
    CMLMessage::error(
      __FILE__,__LINE__,"Failed search.\n",
      "Call made to remove a configuration, but found ",num_matches,
      "\ntable config instances with matching parameters.\n"
      "Removal failed.\n");
      return false;
  }
  else {
    table_config.erase(table_config.begin() + ix_del);
  }
  return true;
}
