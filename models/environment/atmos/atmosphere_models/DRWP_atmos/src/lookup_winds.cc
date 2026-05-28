/*********************************************************************************
PURPOSE:
    (Doppler Radar Wind Pairs model using CML table interpolation routines)

PROGRAMMERS:
    (((Scott Schuh)  (NASA) (Jan 2015) (Initial model implementation and verif))
     ((Edgar Medina) (NASA) (Feb 2015) (Integration into SIM_em1))
     ((Edgar Medina) (NASA) (Mar 2015) (Custom binary file format))
     ((Susan Stachowiak) (NASA) (May 2017) (Added option for vertical wind component))
     ((Brian Birmingham) (OSR) (April 2022) (Added CML Table Interpolation)))

**********************************************************************************/

#define _USE_MATH_DEFINES  // M_PI
#include <cmath>           // M_PI, atan2, sqrt
#include <vector>
#include <cstdint> // uint32_t
#include <fstream> // ifstream
//#include <cstring> // strerror
//#include <iostream>

#include "../include/lookup_winds.hh"

/*****************************************************************************
Constructors
*****************************************************************************/
DRWPTableLookup::DRWPTableLookup()
  :
  drwpFileName(""),
  profile_number(2),
  initialized_with_vertical_component(false),
  verified(false)
{}

/****************************************************************************/
LookupAtmosWinds::LookupAtmosWinds()
  :
  drwpFileName(""),
  wind_number(2),
  include_vertical_component(false),
  block_warnings(false),
  altitude(0.0),
  alt_bias(0.0),
  u(0.0),
  v(0.0),
  w(0.0),
  wind_angle_blowing_to(0.0),
  wind_angle_blowing_from(0.0),
  wind_vmag(0.0),
  wind_velocity_tc{0.0, 0.0, 0.0},
  average_wind{0.0, 0.0, 0.0},
  rho(0.0),
  T(0.0),
  P(0.0),
  SOS(0.0),
  SOS_fair_lo_alt(85344.0),
  SOS_fair_hi_alt(91440.0),
  SOS_hi_alt_const(274.61),
  gamma(1.4),
  warning_issued(false),
  current_index(0),
  number_of_datasets(0),
  TableLookup_array()
{}

/*****************************************************************************
verify
Purpose:
  Verifies the contents of a DRWPTableLookup. Initializes and tests the
  activation of the table. Called from LookupAtmosWinds::initialize() to
  verify that all loaded tables are at least minimally configured.
Note:
 - model subscribes and unsubscribes to itself because this is really an
   extension of LookupAtmosWinds.
*****************************************************************************/
void
DRWPTableLookup::verify( bool leave_active)
{
  // Verify that the table can be initialized:
  initialize(); // goes to SimpleTableLookup::initialize
  if (!is_initialized()) {
    // Not obviously reachable, testable in gdb by setting enabled = false
    // in initialize() above.
    CMLMessage::fail( __FILE__, __LINE__,
      "Data profile defined by:\n"
      "  file: ", drwpFileName,
      "\n  profile: ", profile_number,
      "\ndid not initialize.");
  }

  // Verify that the table can be activated:
  subscribe();
  if (!is_active()) {
    // Not obviously reachable, testable in gdb by setting enabled = false
    // in subscribe() above.
    CMLMessage::fail( __FILE__, __LINE__,
      "Data profile defined by:\n"
      "  file: ", drwpFileName,
      "\n  profile: ", profile_number,
      "\ndid not activate.");
  }
  // and then deactivate it again
  if (!leave_active) unsubscribe();

  // mark it as verified:
  verified = true;
}

/*****************************************************************************
initialize
Purpose:  (Initializes the model)
*****************************************************************************/
void
LookupAtmosWinds::initialize()
{
  // If model has already been initialized, skip it.
  // Initialization of the model includes initialization of all the data
  // tables loaded into the model.
  if (initialized || !enabled) return;


  // Make sure that all loaded tables are fully formed and ready to use.
  // This protects against mid-run data reloads failing -- it moves the
  // failure to the front of the execution.
  for (size_t ii = 0; ii < number_of_datasets; ii++) {
    DRWPTableLookup & table_ = TableLookup_array[ii];
    table_.verify(false);
  }

  SubscriptionBase::initialize();
}

/*****************************************************************************
activate
Purpose: Activates the model when it is actually being called
*****************************************************************************/
void
LookupAtmosWinds::activate()
{
  // if drwpFileName has not been specified, use the profile loaded at the
  // specified current_index (if it has been set), or index 0 if it has not.
  // If there are no profiles available, abort.
  if (drwpFileName.empty()) {
    if (number_of_datasets == 0) {
      CMLMessage::error( __FILE__,__LINE__,
        "Activation of LookupAtmosWinds failed because there are no data\n"
        "tables loaded and no filename specified to load a new table.\n");
        return;
    } else if (!block_warnings) {
      CMLMessage::inform( __FILE__,__LINE__,
        "Unspecified filename for DRWP binary file.\n"
        "Using the profile (pre)selected at index ",current_index,".\n");
    }
    // else no action, use specified index / 0, but do not provide a warning.

    // Configure the model to use the profile at index current_index
    change_datafile_index( current_index);
  }

  // Else, drwpFileName is specified. Use it to identify which profile to use.
  // Use the change_profile() method to search for a pre-loaded profile or, if
  // not found, search for the file and load a new profile.
  else if( !change_profile()) {
    CMLMessage::fail( __FILE__, __LINE__,
      "Failed to initialize DRWP Table Lookup with specifications:"
      "\n  filename: ", drwpFileName,
      "\n  profile: ", wind_number,
      "\n  vertical winds: ", include_vertical_component,
      "\nTable did not load.");
  }
  // else change_profile() succeeded, no action needed.

  SubscriptionBase::activate();
}

/*****************************************************************************
update
Purpose:  (Performs the table interpolation)
*****************************************************************************/
void
LookupAtmosWinds::update(double altitude_in)
{
  if (!active) return;

  // Apply any required bias to offset the altitude floor-datum as used in
  // the data tables from the altitude floor-datum as provided by the simulation
  altitude = altitude_in + alt_bias;

  //  calls the table interpolation routines for independent variable altitude
  //  returns u, v, (w), T, rho, P

  DRWPTableLookup & table_ = TableLookup_array[current_index];
  table_.update();

  calculate_speed_of_sound();
  calculate_wind_mag_dir();

  // checks for altitude outside table range...issues warning
  if (!block_warnings &&
      !warning_issued &&
      table_.independent->is_off_table()) {
    CMLMessage::warn(
      __FILE__, __LINE__,
      "Current altitude ",altitude_in, " (modified to ", altitude, ") "
      "is outside tabulated domain.\n"
      "Using data from edge point.\nQuieting Warnings.\n");
    warning_issued = true;
  } // else no action, warnings not valid or not desirable
}


/*****************************************************************************
change_datafile_index
Purpose:
  Updates the index of the desired data set
Note:
 - Default behavior on use of vertical wind components:
     the member flag include_vertical_component will be assigned from
     the value of the new table's flag initialized_with_vertical_component.
     So if the new table has vertical wind components, the updates from this
     new table will populate w, the vertical wind.
 *****************************************************************************/
void LookupAtmosWinds::change_datafile_index(size_t index)
{
  if (index >= number_of_datasets ){
    CMLMessage::fail( __FILE__, __LINE__,
      "Cannot change to data profile at index ", index,
      "\nThere are only ",number_of_datasets, " profiles available.");
    return;
  }
  // else:
  // deactivate the old table (if it is active)
  if (active) {
    TableLookup_array[current_index].unsubscribe();
  }

  // update all the model records from the new index.
  DRWPTableLookup & table_ = TableLookup_array[index];
  current_index = index;
  wind_number = table_.profile_number;
  drwpFileName = table_.drwpFileName;
  include_vertical_component = table_.initialized_with_vertical_component;
  // if not populating vertical speed, set it to zero.
  if (!include_vertical_component) {
    w = 0.0;
  }
  // reset warning-issued flag to model-level master setting
  warning_issued = false;

  // activate the new table if it has already been verified, or verify (and
  // activate) if it has not.
  if (table_.verified) {
    table_.subscribe();
  } else {
    table_.verify(true);
  }

  CMLMessage::inform(
    __FILE__, __LINE__,
    "Change in DRWP atmosphere profile to"
    "\n  index:          ", current_index,
    "\n  file:           ", drwpFileName,
    "\n  profile number: ", wind_number);
}

/*****************************************************************************
change_profile
Purpose:
  Changes the profile to another pre-loaded profile based on drwpFileName and
  wind_number.
Note:
 - The class members drwpFileName and wind_number will be matched against the
   values stored in the available profiles. If no match is found, the profile
   change fails.
 - The new filename and profile number are pulled from the class members.
*****************************************************************************/
bool LookupAtmosWinds::change_profile()
{
  for (size_t ii = 0; ii < number_of_datasets; ++ii) {
    if ((TableLookup_array[ii].drwpFileName   == drwpFileName) &&
        (TableLookup_array[ii].profile_number == wind_number)) {
      change_datafile_index (ii);
      return true;
    }
  }

  CMLMessage::inform( __FILE__,__LINE__,
    "Could not identify a pre-loaded profile with specified filename: ",
    drwpFileName, "\nand profile: ", wind_number, ".\n"
    "Attempting to find and load one.");

  if (load_DRWP_file( drwpFileName,
                      include_vertical_component,
                      wind_number)){
    // if the loading succeeded, use this new profile.
    change_datafile_index (number_of_datasets - 1);
    return true;
  }

  CMLMessage::error( __FILE__, __LINE__,
    "Could not identify a pre-loaded profile with specified filename: ",
    drwpFileName, "\nand profile number: ", wind_number, ".\n"
    "The attempt to find and load a profile with those specifications also "
    "failed.");
  return false;
}

/*****************************************************************************
load_DRWP_file
Purpose:  (loads an individual DRWP binary file into a vector of
           SimpleTableLookup objects)
Note:
 - it is assumed that a new profile will be inserted at index
     [number_of_datasets] and that number_of_datasets will be incremented.
     All failures to populate the new table currently result in termination.
     If that should ever change, and a failure results in the index not being
     populated, this method should return false.
 - drwpFileName comes in as a string instance rather than a reference to
     support on-the-fly calling from the likes of an input file.
 - the second argument indicates whether the file contains vertical-component
     wind data; this is used to parse the binary data and populate the
     interpolation tables.
 - the third argument identifies which profile to extract from drwpFileName.
     This is a profile number, not an index to the ordering of profiles
     within the file.
*****************************************************************************/
bool
LookupAtmosWinds::load_DRWP_file( std::string  drwpFileName_,
                                  bool         contains_vertical_component_,
                                  unsigned int wind_number_)
{
  if (!enabled) return false;

  if (initialized && !block_warnings) {
    CMLMessage::inform( __FILE__,__LINE__,
      "It is recommended practice to load all data files before initialization"
      "\nFiles are all checked for errors at initialization, avoiding terminal"
      "\nfaults from data-loading errors mid-sim.\n");
  }

  // This method loads atmos/wind data based on the desired atmos/wind profile
  // number.  The data is loaded from a custom binary file format that is
  // packed as follows:
  //
  //    NumBytes     Type   Description
  //    ===========  =====  ====================================================
  //    4            int    Number of profiles stored in binary file (nProfiles)
  //    4            int    Number of altitude breakpoints per profile (nAlts)
  //    4*nAlts      float  Array containing the altitude breakpoints
  //    4*nProfiles  int    Array containing the atmos/wind profile numbers
  //
  //    Then for each of the nProfiles atmos/wind profiles:
  //    4            int    Atmos/wind profile number for the following data
  //    4*nAlts      float  U wind velocity for wind profile N
  //    4*nAlts      float  V wind velocity for wind profile N
  //    4*nAlts      float  W wind velocity for wind profile N (optional)
  //    4*nAlts      float  Temperature for wind profile N
  //    4*nAlts      float  Density for wind profile N
  //    4*nAlts      float  Pressure for wind profile N
  //
  // Notes:
  // - The "magic number" 4 is used in this comment to represent the size of
  //     the integer and floating-point values stored in the binary data.
  //     It would be feasible to templatize the reading of the binary file in
  //     case the structure of the binary files ever changed, but this is not
  //     anticipated. Rather than templatize, we have made this file easily
  //     modifiable. This requires a recompile if the data file structure
  //     changes, but templates would incur overhead that is not justifiable
  //     given the infrequency with which this file structure is likely to vary.
  // - The class member "size_of_source_integer" is used to instruct the binary
  //     file reader how much memory to allow for reading an integer. This is
  //     set to 4 (bytes).
  // - The class member "size_of_source_float" is used to instruct the binary
  //     file reader how much memory to allow for reading a floating-point.
  //     This is set to 4 (bytes).
  // - The data type "data_int" is used for allocating space within this
  //     method to accommodate the data as it comes in from the binary reader.
  //     This is typedef'd from unit32_t (4 byte).
  // - The data type "data_float" is used for allocating space within this
  //     method to accommodate the data as it comes in from the binary reader.
  //     This is typedef'd from float (4 byte).
  // - To change to a different binary file format, change the typedef on the
  //     lines below and the setting of "size_of_source*".
  typedef uint32_t data_int;
  typedef float    data_float;
  if ((size_of_source_integer != sizeof(data_int)) ||
      (size_of_source_float   != sizeof(data_float))) {
    // unreachable sanity check. Tested in gdb by setting size_of_source_float.
    CMLMessage::fail( __FILE__, __LINE__,
      "DRWP load_file data type mismatch:\n",
      "Need to typedef data_int and data_float to be consistent with the "
      "expected data sizes."
      "\n  sizeof(data_int)       : ", sizeof(data_int),
      "\n  size_of_source_integer : ", size_of_source_integer,
      "\n  sizeof(data_float)     : ", sizeof(data_int),
      "\n  size_of_source_float   : ", size_of_source_float);
  }

  // If we already have filled our allocated space for datasets, we can't add
  // another one. Otherwise, the index of the table being populated must be
  // number_of_datasets -- if we already have 3 datasets, we will be
  // populating the 4th, at index 3.
  if (number_of_datasets == MaxDatasets) {
    CMLMessage::fail(__FILE__, __LINE__,
        "Maximum number of binary data files reached");
  }

  //  open the raw data file
  std::ifstream BinFile;
  BinFile.open(drwpFileName_);
  if( BinFile.fail()) {
    stream_error(__LINE__, drwpFileName_);
    return false;
  }

  // Record this successful filename in the new table:
  DRWPTableLookup & table_ = TableLookup_array[number_of_datasets];
  table_.drwpFileName = drwpFileName_;

  // Position the reader to the beginning of the file
  BinFile.seekg (0, std::ios::beg);

  //  Read the number of profiles and the number of altitudes
  // (the independent variable). There are constraints on these values because
  // they will be used to size new data arrays. The numbers are read in as
  // unsigned integers, so if the number in the file is negative, it will read
  // in as a large positive integer.
  data_int number_of_profiles_;
  BinFile.read(  (char *) &number_of_profiles_, size_of_source_integer );
  if( BinFile.fail()) {
    stream_error(__LINE__, drwpFileName_);
    return false;
  }
  if (number_of_profiles_ < 1 || number_of_profiles_ > 1e6) {
    CMLMessage::fail(__FILE__, __LINE__,
      "DRWP file read produced bad number of profiles : ", number_of_profiles_,
      "\nNumber must be positive and not excessively large.");
  }

  data_int number_of_altitudes_;
  BinFile.read(  (char *) &number_of_altitudes_, size_of_source_integer );
  if( BinFile.fail()) {
    stream_error(__LINE__, drwpFileName_);
    return false;
  }
  if (number_of_altitudes_ < 1 || number_of_altitudes_ > 1e6) {
    CMLMessage::fail(__FILE__, __LINE__,
      "DRWP file read produced bad number of altitude calibration points : ",
      number_of_altitudes_,
      "\nNumber must be positive and not excessively large.");
  }

  // dependent variables are u, v, T, P, rho.
  // w is included if contains_vertical_component_ is True
  unsigned int number_of_dependent_variables_ =
                                            contains_vertical_component_ ? 6 : 5;

  /********************   Loading independent variable data  ******************/
  // Define vector of floats to receive data from binary file
  // These data are the altitudes - the independent variable
  // The array input_data will be used twice, hence the large dimension
  data_float altitude_array_[number_of_altitudes_];
  BinFile.read( (char *) altitude_array_,
                number_of_altitudes_ * size_of_source_float);
  if( BinFile.fail()) {
    stream_error(__LINE__, drwpFileName_);
    return false;
  }

  // The table interpolation routines require the data to be of type double
  // cast from float to double is automatic, but cannot pass the float array
  // to load_independent_data, we need to convert it first.
  std::vector<double> altitude_data_( altitude_array_,
                                      altitude_array_ + number_of_altitudes_);

  // Now try loading the independent data onto the table.
  if (!table_.load_independent_data( altitude,
                                     altitude_data_)) {
    CMLMessage::fail(__FILE__, __LINE__,
      "Independent data failed to load\n",
      "See Table Interpolation error code");
  } /******   Independent variable data is loaded  ************/


  // Next read in the wind profile numbers; these might not be sequential
  data_int wind_profiles_[number_of_profiles_];
  BinFile.read( (char *) wind_profiles_,
                number_of_profiles_ * size_of_source_integer );
  if( BinFile.fail()) {
    stream_error(__LINE__, drwpFileName_);
    return false;
  }

  // Determine the index for a particular atmos/wind profile-number.
  // Note -- the order in which profiles appear in the data file is not
  //         constrained -- there is no requirement that their identifying
  //         numbers be used to record profiles consecutively, nor even
  //         monotonically. We cannot use the profile-number (wind_number_) to
  //         directly step to the correct place in the data file.
  // First, search for the presence of the intended profile-number
  // (wind_number_) in the array of profile-numbers just read in.
  // If a match is found, this data file includes the desired profile-number:
  //   - record the value of profile-number in the new table, and
  //   - record the index of where among the profiles in this file the desired
  //     profile-number is located (profile_ix_)
  size_t profile_ix_;
  for (profile_ix_ = 0; profile_ix_ < number_of_profiles_; profile_ix_++) {
      if ( wind_number_ == wind_profiles_[profile_ix_]) {
        table_.profile_number = wind_number_;
        break;
      }
  }
  // if no match was found, exit.
  if (profile_ix_ == number_of_profiles_) {
    CMLMessage::fail(__FILE__, __LINE__,
      "Could not find selected atmos/wind profile ", wind_number_,
      "\nin data file ", drwpFileName_, ".");
  }

  // Now we can use that index and the size of each profile to step through the
  // binary file to the appropriate starting point of the desired profile.
  // Each profile has a wind number (integer) and a block of floats
  // representing the 5 (or 6) dependent variables for each calibrated altitude
  // There are (number of dependent variables) * (number of altitudes) data
  // points, each of which occupies size_of_source_float bytes,
  unsigned int pointsPerBlock_ =
                         number_of_altitudes_ * number_of_dependent_variables_;
  unsigned int bytesPerBlock_  = size_of_source_float * pointsPerBlock_;

  // For each profile, make sure that the first entry matches with the profile
  // number read in earlier (to wind_profiles_). This is a sanity check that we
  // are in the right place in the data file.
  // Note -- the reader has just finished reading the wind_profiles_ array;
  //         the next entry should be the profile number for the first profile.
  for (size_t ii = 0; ii < number_of_profiles_; ii++) {
    data_int profile_number_;
    BinFile.read( (char *) &profile_number_, size_of_source_integer );
    if( BinFile.fail()) {
      stream_error(__LINE__, drwpFileName_);
      return false;
    }
    if (profile_number_ != wind_profiles_[ii]) {
      CMLMessage::fail(__FILE__, __LINE__,
         "Profile number mismatch at index ", ii, " in data file.\n"
         "Expected value for profile number of ", wind_profiles_[ii], " but\n"
         "read value for profile number of ", profile_number_, "\n",
         "File is not structured as expected.");
    }
    // Then advance the file from the current location past bytesperBlock
    // bytes to bypass the actual data so we can check the next profile number.
    BinFile.seekg(bytesPerBlock_, std::ios::cur);
  }

  // We should now be at the end of the file, having rread the last expected
  // block. This can be verified.
  // Grab a record of the current position, wind the file to the end and grab
  // a record of the position there. These should match. If they don't, the
  // parsing went awry.
  int file_pos_ = BinFile.tellg();
  BinFile.seekg (0, std::ios::end);
  int file_end_ = BinFile.tellg();
  if (file_pos_ != file_end_) {
    CMLMessage::fail(__FILE__, __LINE__,
      "DRWP binary file size does not match size of all data read");
  }

  // At this point, the file sanity checks have passed. Now we need to
  // position the reader at the correct place to start reading the intended
  // profile.
  // At the front of the file, we have:
  //  - an integer specifying the number_of_profiles
  //  - another integer specifying the number of altitudes
  //  - an array of <number_of_altitudes_> floating-point values
  //      representing the altitude data
  //  - an array of <number_of_profiles_> integers representing the wind numbers.
  // This adds up to this much space:
  int bytesAtBeginning_ = size_of_source_integer * (2 + number_of_profiles_) +
                          size_of_source_float   * number_of_altitudes_;

  // Then each profile occupies memory for:
  // - an integer to confirm the profile number
  // - bytesperBlock for the actual data.
  int bytesPerProfile_ = bytesPerBlock_ + size_of_source_integer;

  // then we also bypass the integer at the front of the profile we actually
  // want; we have already checked that this integer has value = wind_number_
  int bypass_ = bytesAtBeginning_ + (profile_ix_ * bytesPerProfile_) +
                size_of_source_integer;

  //  advance from the front of the file this many bytes; this puts us at the
  //  front of the desired profile.
  BinFile.seekg(bypass_, std::ios::beg);

  /*********  Loading profile data  ***********************/
  data_float input_data_[pointsPerBlock_];
  BinFile.read((char *) input_data_, bytesPerBlock_);
  // Possibly unreachable: causing a failure here is difficult because we
  // have already verified that the file can be read through this memory space.
  // Tested in gdb by manually setting bytesPerBlock_.
  if( BinFile.fail()) {
    stream_error(__LINE__, drwpFileName_);
    return false;
  }

  // As before, to load the data we must convert it to double.
  std::vector<double> dependent_variables_( input_data_,
                                            input_data_ + pointsPerBlock_);
  std::vector<double *> dep_vec_;
  dep_vec_.push_back( &u );
  dep_vec_.push_back( &v );
  // If including vertical wind-speed, include w in the dependent variables,
  // and record that in the table.
  if (contains_vertical_component_) {
    dep_vec_.push_back( &w );
    table_.initialized_with_vertical_component = true;
  }
  dep_vec_.push_back( &T );
  dep_vec_.push_back( &rho );
  dep_vec_.push_back( &P );

  if (!table_.load_dependent_data( dep_vec_,
                                   dependent_variables_,
                                   number_of_altitudes_)) {
    // Possibly unreachable. The data has been confirmed to be all intact.
    // Tested in gdb by manually overriding number_of_altitudes_.
    CMLMessage::fail(__FILE__, __LINE__,
      "Dependent data failed to load\n",
      "See Table Interpolation error code");
  }
  /*********   Dependent variable data is successfully loaded  **********/
  BinFile.close();
  number_of_datasets++;
  return true;
}


/*****************************************************************************
compute_average_winds
Purpose: Computes the average wind velocity between two specified altitudes
Note: Average in this sense is the mean value of the calibrated values that
      lie between the lower and upper bounds. The average_wind output vector
      is in the topocentric frame, being the average of wind_velocity_tc at the
      calibrated points.
*****************************************************************************/
void
LookupAtmosWinds::compute_average_wind()
{
  size_t target_index = (active)? current_index : 0;
  compute_average_wind( target_index, true);
}
/****************************************************************************/
void
LookupAtmosWinds::compute_average_wind(
    size_t table_index)
{
  compute_average_wind( table_index, true);
}
/****************************************************************************/
void
LookupAtmosWinds::compute_average_wind(
    double min_alt,
    double max_alt)
{
  size_t target_index = (active)? current_index : 0;
  compute_average_wind( target_index, false, min_alt, max_alt);
}
/****************************************************************************/
void
LookupAtmosWinds::compute_average_wind(
    size_t table_index,
    double min_alt,
    double max_alt)
{
  compute_average_wind( table_index, false, min_alt, max_alt);
}
/****************************************************************************/
void
LookupAtmosWinds::compute_average_wind(
    size_t table_index,
    bool   full_domain,
    double min_alt,
    double max_alt)
{
  if (table_index >= number_of_datasets) {
    CMLMessage::error( __FILE__,__LINE__,
      "Error computing average wind velocity for profile at index ",
      table_index, ".\nThis index has not been populated with data.\n"
      "Aborting computation.\n");
    return;
  }

  double original_altitude = altitude;
  jeod::Vector3::initialize(average_wind);
  DRWPTableLookup & table_ = TableLookup_array[table_index];
  size_t num_alts = 0;
  // accessing the winds data directly is challenging, so just run the update.
  if ( full_domain) {
    for (double const & altitude_ : table_.independent->data) {
      altitude = altitude_;
      table_.update();
      calculate_wind_mag_dir();
      jeod::Vector3::incr( wind_velocity_tc,
                     average_wind);
      num_alts++;
    }
  } else { // using min-alt and max_alt
    bool in_domain = false;
    for (double const & altitude_ : table_.independent->data) {
      if ((altitude_ > min_alt) && (altitude_ < max_alt)) {
        in_domain = true;
        altitude = altitude_;
        table_.update();
        calculate_wind_mag_dir();
        jeod::Vector3::incr( wind_velocity_tc,
                       average_wind);
        num_alts++;
      } else if (in_domain) {
        // Was in-domain, but now out-of-domain. No need to check the rest
        // of the data, it's all going to be out-of-table because the data
        // are monotonic. So leave.
        break;
      }
      // else, still looking for the first point that is within the
      // specified domain. Keep going.
    }
  }

  if (num_alts == 0) {
    CMLMessage::error( __FILE__,__LINE__,
      "Error computing average wind velocity within specified bounds\n",
      "No altitude breakpoints found in between 'min_alt' (", min_alt,
      ")\nand 'max_alt' (", max_alt, ") for given DRWP Binary file.");
    // Leave average at zero-vector
  }
  else {
    jeod::Vector3::scale( (1.0/num_alts),
                    average_wind);
    // We just populated the model's output data with values from an altitude
    // that is not the current altitude. If the model is currently active, that
    // might cause some problems with data logging.
    // Reset model outputs to current table and altitude.
    if (active) {
      update(original_altitude);
    }
  }
}

/*************************************************************************
calculate_speed_of_sound
Purpose:  (computes SOS based on pressure and density at a given altitude)
 **************************************************************************/
void LookupAtmosWinds::calculate_speed_of_sound()
{
  if ( altitude < SOS_fair_lo_alt ) {
    SOS = std::sqrt( gamma * P / rho );
  }
  else if ( altitude < SOS_fair_hi_alt ) {
    // Interpolate between the theoretical SOS (sqrt(gamma P / rho) and the
    // high-altitude SOS, using an interpolation fraction that is the
    // fractionala ltitude between the two altitude bounds. This function
    // gradually diverges from the theoretical profile to hit the
    // high-altitude value at the top of the interval.
    double SOS_no_fair = std::sqrt( gamma * P / rho );
    // Note -- div-0 protected because SOS_fair_hi_alt must be greater than
    //         SOS_fair_lo_alt by structure of if-statements in getting to
    //         this point.
    SOS = SOS_no_fair + (altitude - SOS_fair_lo_alt) /
                        (SOS_fair_hi_alt - SOS_fair_lo_alt) *
                        (SOS_hi_alt_const - SOS_no_fair);
  }
  else SOS = SOS_hi_alt_const;
}

/*************************************************************************
calculate wind magnitude & wind direction & wind velocity vector
Purpose:
  Computes magnitude, direction, wind velocity cartesian vector in topocentric
  NED frame, and wind velocity vector components in topocentric ENU frame:
    u wind velocity from West to East.
    v wind velocity from South to North.
    w wind velocity - vertical - up is positive.
 **************************************************************************/
void LookupAtmosWinds::calculate_wind_mag_dir()
{
  //  calculates the components and magnitude
  // atan2 can handle u=0 or v=0.  It also determines the correct quadrant
  // It returns values from [-pi, +pi], so there is no need to check
  //     for wind_angle_blowing_from > 2pi
  wind_angle_blowing_to = std::atan2(u,v);
  wind_angle_blowing_from = wind_angle_blowing_to + M_PI;
  wind_vmag = std::sqrt(u*u + v*v + w*w);
  wind_velocity_tc[0] = v;
  wind_velocity_tc[1] = u;
  wind_velocity_tc[2] = -w;
}

/*****************************************************************************
stream_error
Purpose:
  A shared errror-message handler for all places where the binary reader
  might fail.
*****************************************************************************/
void
LookupAtmosWinds::stream_error( int line,
                                const std::string & drwpFileName_)
{
  // If initialized and the number of available datasets is greater than 0,
  // then failing to add the new file is just an error.
  if (initialized && (number_of_datasets > 0)) {
    CMLMessage::error( __FILE__,line,
      "DRWP Binary file reader failed to read from data file ", drwpFileName_,
      ".\nNew data is not available.\n");
  }
  // otherwise it's terminal
  else {
    CMLMessage::fail( __FILE__,line,
      "DRWP Binary file reader failed to read from data file ", drwpFileName_,
      ".\nNew data is not available.");
  }

}

/*****************************************************************************
set_include_vertical_component
Purpose:
  Deprecated method that used to modify the way in which the data was read
  from the binary. This strategy was fragile against efforts to change
  behavior mid-run.
Note:
 - If there is a future need to block population of the vertical wind from a
   data file that contains those data, the easiest place to insert this would
   be in calculate_wind_mag_dir; at the start, set w to 0.0 if its population
   should be blocked
*****************************************************************************/
void LookupAtmosWinds::set_include_vertical_component(bool)
{
  CMLMessage::error(
    __FILE__,__LINE__,"Call made to "
    "LookupAtmosWinds::set_include_vertical_component( bool )\n"
    "but this method is deprecated.\n"
    "The variable include_vertical_component is used to specify whether\n"
    "the model should parse the binary file to include the vertical wind\n"
    "component. This variable is public and directly settable; \n"
    "it is only used in parsing the binary data file.\n"
    "Once the binary data has been read in, the model will populate\n"
    "the vertical wind component if and only if the vertical wind data\n"
    "are present in the parsed data.\n"
    "There is no mechanism to deselect the population of the vertical wind\n"
    "output when a vertical wind profile is available, nor to populate\n"
    "the vertical wind output without a data profile being available.\n");
}

/*****************************************************************************
test_for_reinitialize
Purpose:  (Legacy method for calling for a data reload)
*****************************************************************************/
void LookupAtmosWinds::test_for_reinitialize()
{
  CMLMessage::error(
    __FILE__,__LINE__,"Call made to LookupAtmosWinds::test_for_reinitialize() "
    "is invalid. This method is obsolete and has been deprecated.\n"
    "Check intent on why this method was called, and review documentation for "
    "alternative approach.\n");
}
