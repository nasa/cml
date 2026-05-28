/*******************************TRICK HEADER******************************
PURPOSE: (Provide a simple table-lookup capability for winds)

LIBRARY DEPENDENCY:
   ((../src/simple_lookup_wind.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (July 2018) (Antares) (initial))
**********************************************************************/

#ifndef CML_SIMPLE_LOOKUP_WIND_HH
#define CML_SIMPLE_LOOKUP_WIND_HH

#include "cml/models/utilities/table_interp_cpp/include/table_lookup_set.hh"
#include "cml/models/utilities/table_interp_cpp/include/table_independent_variable.hh"
#include "cml/models/utilities/table_interp_cpp/include/single_input_table_for_angles.hh"
#include "cml/models/utilities/table_interp_cpp/include/generic_multi_input_table.hh"
#include "cml/models/utilities/table_interp_cpp/include/simple_table_lookup.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/math/include/vector3.hh"

// NOTES - the table-lookup capabilities assume that the driving independent
//         variable is altitude;
//       - the general application of this model has the wind-model being
//         associated with the planet, and accessed by one or more vehicles;
//       - the altitude dependency of the model is a function therefore of the
//         vehicles, but planetary-associations typically have no knowledge of
//         altitude -- they typically have no knowledge even of vehicles!!
//       - furthermore, even if the model reference could be assigned to access
//         a vehicle's altitude directly, that would unnecessarily limit the
//         model to 1 vehicle.
//       - Therefore, the update method receives the altitude as an input
//         argument.  But the tables require a reference to the altitude.  That
//         reference is provided to a local variable, which is populated by the
//         value passed in with the update(...) argument.

class SimpleLookupWind : public SubscriptionBase
{
 protected:
  double altitude_placeholder; /* (m)
        Copy of the altitude; used to support multi-vehicle capabilities.
        The data tables must see this variable.*/

 public:
  bool wind_components_specified; /* (--)
        Flag to distinguish the cases where the winds are specified in a
        direction-magnitude-up set versus a North-East-Down set.
        Default: false (specified as direction-magnitude-Up).*/
  bool include_vertical_component; /* (--)
        Flag to indicate whether the winds are purely horizontal or whether
        a vertical component should also be included.
        Default: False (horizontal only).*/

  double wind_blowing_from; /* (rad)
        Angle relative to North specifying the orientation from which
        the wind is blowing.*/
  double wind_magnitude_horizontal; /* (m/s)
        Magnitude of the HORIZONTAL COMPONENT of the current wind.
        Note this is not populated if data is provided as components.*/
  double wind_magnitude; /* (m/s)
        Magnitude of the current wind.*/
  double wind_vertical_up; /* (m/s)
        Additional component oriented "upward" positive.
        Note this is not populated if data is provided as components.*/

  TableLookupSet dir_mag_table_set; /* (--)
        Table set to be used when the data is specified as dir-mag*/
  TableIndependentVariable altitude_table; /* (--)
        Altitude table used as the independent variable.  Altitude comes
        from the planetary-derived state, so units are SI.
        Whether altitude is topocentric wrt sphere, topocentric wrt ellipsoid,
        or topodetic depends on which variable this instance accesses.
        There are no checks; take care with configuration. */
  SingleInputTableForAngles wind_blowing_from_table; /* (--)
        Wrap-around interpolation table for generating orientation of wind. */
  GenericMultiInputTable wind_magnitude_table; /* (--)
        Interpolation table for generating magnitude of the horizontal
        components of the wind velocity.*/
  GenericMultiInputTable wind_vertical_up_table;/* (--)
        Interpolation table for generating the vertical component of the
        wind velocity.*/
  double wind_component[3]; /* (m/s)
        A 3-vector representing the wind in the North-East-Down frame.
        NOTE - whether this is topocentric NED or Topodetic NED is left to the
        discretion of the user.  The difference can be specified in the
        AtmosphereExecutive model, where both values are generated.*/
  SimpleTableLookup component_table_set; /* (--)
       The table-system for generating the wind components from the altitude.*/
  bool wind_blowing_from_warning; /* (--)
        Indicates zero wind components in the North and East directions */
       

  SimpleLookupWind();
  virtual ~SimpleLookupWind(){};

  void initialize();
  void update(double altitude);


  // input file helper functions
  void assign_component_data( const double * data_array,
                              size_t         num_components,
                              size_t         num_elem_per_variable);
  void assign_dir_mag_data( const double * data_array,
                            size_t         num_elem_per_variable);
  void assign_dir_mag_vert_data( const double * data_array,
                                 size_t         num_elem_per_variable);

 private:
  // private and unimplemented; cannot be used.
  SimpleLookupWind (const SimpleLookupWind&);
  SimpleLookupWind & operator = (const SimpleLookupWind&);
};
#endif