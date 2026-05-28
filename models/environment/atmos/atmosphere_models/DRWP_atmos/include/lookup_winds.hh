/******************************** TRICK HEADER **********************************
PURPOSE:
   (Doppler Radar Wind Pairs model using CML table interpolation routines)

LIBRARY DEPENDENCY:
    ((../src/lookup_winds.cc))

PROGRAMMERS:
    (((Scott Schuh)  (NASA) (Jan 2015) (Initial model implementation and verif))
     ((Edgar Medina) (NASA) (Feb 2015) (Integration into SIM_em1))
     ((Edgar Medina) (NASA) (Mar 2015) (Custom binary file format))
     ((Susan Stachowiak) (NASA) (May 2017) (Added option for vertical wind component))
     ((Brian Birmingham) (OSR) (April 2022) (Added CML Table Interpolation)))
********************************************************************************/
#ifndef CML_LOOKUP_ATMOS_WINDS_HH
#define CML_LOOKUP_ATMOS_WINDS_HH

#include "cml/models/utilities/table_interp_cpp/include/simple_table_lookup.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include <string>


/*****************************************************************************
DRWPTableLookup
Purpose:
  An extension of SimpleTableLookup that adds a record of the
  source of the data to facilitate switching between tables mid-sim.
Notes:
 - drwpFileName, profile_number, and initialized_with_vertical_component
   are recorded at the time the table data gets populated and can be used
   later to identify the desired data table.
 - the data elements in here are public, but the only instances of this class
   are in LookupAtmosWinds, and those instances are private to the
   LookupAtmosWinds instance. So these data are not publicly accessible.
   This is deliberate.
*****************************************************************************/
class DRWPTableLookup : public SimpleTableLookup
{

public:
  std::string drwpFileName; /* (--)
    Name of the binary data file from which the table data was extracted.*/
  unsigned int profile_number;   /* (--)
    The profile number within the binary data file (drwpFileName) that was
    used to populate the data tables. */
  bool initialized_with_vertical_component; /* (--)
    The data table includes verrtical wind components.*/
  bool verified; /* (--)
    Flag to indicate that the table has been loaded, initialized, and tested
    at initialization of LookupAtmosWinds, and is now ready to be activated.*/

  DRWPTableLookup();
  virtual ~DRWPTableLookup(){};
  void verify( bool leave_active = false);
};

/*****************************************************************************
LookupAtmosWinds
Purpose:
  The model used to extract atmospheric and winds data from the provided binary
  files.
Notes:
  - DRWP (Doppler Rader Winds Profile) started as a tool for specifying
    wind values as measured by Doppler Radar. It has since evolved into a
    mechanism for reading binary data files that include wind profiles and
    atmospheric data as a function of altitude.
    The name "LookupWinds" morphed to "LookupAtmosWinds" to better reflect
    the more comprehensive application of this model.
*****************************************************************************/
class LookupAtmosWinds : public SubscriptionBase
{
public:
  // Configuration:
  std::string drwpFileName;/* (--)
    For the profile currently in use, this is the name of the binary data file
    from which the data was extracted.
    This gets assigned automatically when changing to a new profile.
    A new profile can be identified by setting this manually mid-run.
    A new profile can be loaded by setting this manually prior to
    initialization.*/
  unsigned int wind_number; /* (--)
    For the profile currently in use, this is the profile number in
    drwpFileName from which the data was extracted.
    This gets assigned automatically when changing to a new profile.
    A new profile can be identified by setting this manually mid-run.
    A new profile can be loaded by setting this manually prior to
    initialization.*/
  bool include_vertical_component; /* (--)
    This flag indicates whether the profile currently in use, has been
    configured to populate vertical winds data.
    This gets assigned automatically when changing to a new profile.
    This is also used as a user-settable flag prior to initialization when
    the profile is to be loaded during the initialization process; in this
    context it indicates that the profile includes vertical wind data.*/
  bool block_warnings; /* (--)
    Flag to restrict out-of-domain warnings for all data sets. Only accessed
    as the model switches to a new data profile.*/


  // Current state
  double altitude; /* (m)
    The altitude used as the independent variable in the table interpolation
    algorithms. May be offset from the sim-provided input altitude to account
    for different zero-references. */
  double alt_bias; /* (m)
    Geoidal Separation. Provides a conversion from the altitude_in accessed
    from the sim, and altitude_lookup, the value used as the independent
    variable in the table-lookups. Altitude_lookup = altitude_in + alt_bias.*/


  // Wind Values
  double u; /* (m/s)
    Wind velocity, horizontal from West to East.*/
  double v; /* (m/s)
    Wind velocity, horizontal from South to North. This is assumed to be a
    topocentric North. */
  double w; /* (m/s)
    Wind velocity, vertical: Up positive. This is assumed to be
    topocentric Up, i.e. radially "out". */
  double wind_angle_blowing_to; /* (rad)
    The azimuth angle the wind is blowing towards.
    The azimuth angle is relative to North as measured in the
    topocentric horizontal plane, from North towards East.*/
  double wind_angle_blowing_from; /* (rad)
    The azimuth angle the wind is blowing from.
    The azimuth angle is relative to North as measured in the
    topocentric horizontal plane, from North towards East.
    wind_angle_blowing_from is Offset from wind_angle_blowing_to by pi. */
  double wind_vmag;           /* (m/s)
    Magnitude of the current wind vector.*/
  double wind_velocity_tc[3]; /* (m/s)
    Wind velocity vector expressed in a topocentric NED frame.
    Note:  This is a NED vector populated from u,v,w as [v, u, -w] */
  double average_wind[3];     /* (m/s)
    Average wind velocity between two specified altitudes.
    Populated in the utility function compute_average_wind and typically
    accessed immediaately thereafter. The altitude bounds within which the wind
    velocity was averaged are not recorded, and are required for correct
    interpretation of this vector.*/

  // Atmosphere Values
  double  rho;              /* (kg/m3) Atmospheric density */
  double  T;                /* (K)     Atmospheric temperature */
  double  P;                /* (N/m2)  Atmospheric pressure */
  double  SOS;              /* (m/s)   Speed of sound */
  double  SOS_fair_lo_alt;  /* (m)     Low altitude for SOS fairing */
  double  SOS_fair_hi_alt;  /* (m)     High altitude for SOS fairing */
  double  SOS_hi_alt_const; /* (m/s)   Constant SOS value for high altitudes */
  double  gamma;            /* (--)    Ratio of specific heats Cp/Cv of air */

 private:
  bool warning_issued; /* (--)
    Used to block repetitive warnings about altitude being outside the domain
    of the lookup table.
    Defaults to false and is set to true when an out-of-domain warning is
    issued.  This prevents re-issuance.
    Resets to block_warnings when a new profile is loaded.*/
  unsigned int current_index;    /* (--)
    The index (within the TableLookup_array) of the active data set.
    This can only be modified via the method change_datafile_index(size_t).*/
  unsigned int number_of_datasets; /* (--)
    Total number of data-profiles stored. */
  const size_t size_of_source_integer = 4; /* (--)
    This value specifies the number of bytes occupied by the integer values
    in the binary data files.
    The raw data files (binary format) are currently composed of integer
    and floating point values. The default expectation is that these have been
    recorded in the binary files as 4-byte int and 4-byte float values.
    When reading data from the binary files, it is necessary to know where to
    separate the data stream into individual variables. If a binary file were
    to be written using a different format, it would be necessary to change
    the size that these variables occupy.*/
  const size_t size_of_source_float = 4; /* (--)
    This value specifies the number of bytes occupied by the integer values
    in the binary data files.
    See size_of_source_integer description for details. */
  static const size_t MaxDatasets = 10; /* (--)
    Maximum number of data sets / data-files that can be loaded for mid-sim
    switching.*/
  DRWPTableLookup TableLookup_array[MaxDatasets]; /* (--)
    A C-style array of DRWPTableLookup objects.
    There is an upper limit of MaxDatasdets (default 10) different
    atmosphere profiles per run.  Profiles are currently 21k in size.*/


 public:
  LookupAtmosWinds();
  virtual ~LookupAtmosWinds(){};

  void initialize();
  void update(double altitude_in);
  void change_datafile_index(size_t index);
  bool change_profile();
  bool load_DRWP_file(std::string  drwpFileName,
                      bool         file_contains_vertical_wind_component,
                      unsigned int wind_number);
  void compute_average_wind();
  void compute_average_wind( size_t table_index);
  void compute_average_wind( double min_altitude,
                             double max_altitude);
  void compute_average_wind( size_t table_index,
                             double min_altitude,
                             double max_altitude);

  // Legacy capability, deprecated.
  void test_for_reinitialize();
  void set_include_vertical_component(bool);

protected:
  void activate();
  void compute_average_wind( size_t table_index,
                             bool   full_domain,
                             double min_altitude=0,
                             double max_altitude=0);
  void calculate_speed_of_sound();
  void calculate_wind_mag_dir();
  void stream_error(int, const std::string&);
private:
  // private and unimplemented; cannot be used.
  LookupAtmosWinds (const LookupAtmosWinds&);
  LookupAtmosWinds & operator = (const LookupAtmosWinds&);
};
#endif
