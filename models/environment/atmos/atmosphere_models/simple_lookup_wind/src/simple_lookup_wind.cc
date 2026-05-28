/*******************************TRICK HEADER******************************
PURPOSE: (Provide a simple table-lookup capability for winds)

PROGRAMMERS:
  (((Gary Turner) (OSR) (July 2018) (Antares) (initial))
   ((Brian Birmingham) (OSR) (July 2022) (Antares) (wind angle)))  
**********************************************************************/
#define _USE_MATH_DEFINES // M_PI
#include <cmath>           // M_PI, sin, cos, atan2
#include <vector>

#include "cml/models/utilities/math_utils/include/math_utils.hh"
#include "../include/simple_lookup_wind.hh"
/*****************************************************************************
Constructor
*****************************************************************************/
SimpleLookupWind::SimpleLookupWind()
  :
  altitude_placeholder(0.0),

  wind_components_specified(false),
  include_vertical_component(false),

  wind_blowing_from(0.0),
  wind_magnitude_horizontal(0.0),
  wind_magnitude(0.0),
  wind_vertical_up(0.0),
  dir_mag_table_set(),
  altitude_table(altitude_placeholder),
  wind_blowing_from_table(wind_blowing_from),
  wind_magnitude_table(wind_magnitude_horizontal),
  wind_vertical_up_table(wind_vertical_up),
  
  wind_component{0.0, 0.0, 0.0},
  component_table_set(),
  wind_blowing_from_warning(false)
{}

/*****************************************************************************
initialize
Purpose:(Initializes the model)
*****************************************************************************/
void
SimpleLookupWind::initialize()
{
  if (initialized) return;

  // Split based on which set of tables to use.
  if (wind_components_specified) {
    // component_table_set configured with the load_data command; should have
    // already been run.
    component_table_set.initialize();
    component_table_set.subscribe();
  }
  else {
    dir_mag_table_set.add_independent_variable(altitude_table);

    dir_mag_table_set.add_table(wind_blowing_from_table);
    dir_mag_table_set.associate_table_and_independent();

    dir_mag_table_set.add_table(wind_magnitude_table);
    dir_mag_table_set.associate_table_and_independent();

    if (include_vertical_component) {
      dir_mag_table_set.add_table(wind_vertical_up_table);
      dir_mag_table_set.associate_table_and_independent();
    }
    dir_mag_table_set.initialize();
    dir_mag_table_set.subscribe();
  }

  SubscriptionBase::initialize();
}

/*****************************************************************************
update
Purpose:(Main executive.  Executes the table-lookup and converts to desired
           outputs.
         The no-argument version assumes that the reference to altitude in
           the table-sets has been set to an external variable.
         The 1-argument version assumes that the reference to altitude in
           the table-sets has been set to the internal altitude_placeholder
           variable; this implementation allows the model to apply the
           specified data tables to multiple independent altitudes)
*****************************************************************************/
void
SimpleLookupWind::update(double altitude_in)
{
  if (!active) return;

  // Make a copy of the argument; this is the variable that the table-lookups
  // will use as an input.
  altitude_placeholder = altitude_in;

  if (wind_components_specified) {
    // generate the 2 horizontal components of the wind and
    // (if applicable) the vertical component
    component_table_set.update();
    // the wind_blowing_to angle can be represented as arctan( u / v ),
    //     where v is the south-to-north direction 
    //     and u is the west-to-east direction
    // atan2 can handle either argument equal 0.  Not both
    if (MathUtils::is_near_equal(wind_component[0], 0.0) && MathUtils::is_near_equal(wind_component[1], 0.0)) {
      if (!wind_blowing_from_warning) {
        CMLMessage::warn(__FILE__, __LINE__, "Undefined arctan calculation\n", 
          "Wind angle undefined with u & v components = 0. Setting wind_blowing_from = 0.0. Quieting Warnings...");
         wind_blowing_from_warning = true;
      }
      wind_blowing_from = 0.0;
    } else {
      wind_blowing_from = M_PI + std::atan2( wind_component[1], wind_component[0]);
    }
    wind_magnitude = jeod::Vector3::vmag(wind_component);
  }
  else {
    // generate the direction and magnitude of the horizontal wind and
    // (if applicable) the vertical wind component
    dir_mag_table_set.update();

    // Translate into component form (NED).
    wind_component[0] = -wind_magnitude_horizontal * std::cos(wind_blowing_from);
    wind_component[1] = -wind_magnitude_horizontal * std::sin(wind_blowing_from);
    wind_component[2] = -wind_vertical_up;
    if (include_vertical_component) {
      wind_magnitude = jeod::Vector3::vmag(wind_component);
    }
    else {
      wind_magnitude = wind_magnitude_horizontal;
    }
  }
}

/*****************************************************************************
assign_component_data
Purpose:(A helper function to be called from the input file to help with
         loading data defined in the input file.  Note that the independent
         variable reference is protected, so this cannot be assigned from
         the input file directly.)
*****************************************************************************/
void
SimpleLookupWind::assign_component_data(
    const double * data,
    size_t         num_components,
    size_t         num_elem_per_component)
{
  component_table_set.load_independent_data( altitude_placeholder,
                                             data,
                                             num_elem_per_component);
  if (num_components > 2) {
    if (num_components > 3) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid request\n",
        "Call made to assign component data to ", num_components, " dependent variables.\n"
        "This model can only handle 3 components -- the 3 axes of the\n"
        "wind velocity vector.\n"
        "Changing num_components to 3.\n");
      num_components = 3;
    }
    include_vertical_component = true;
  }

  component_table_set.load_dependent_data( wind_component,
                                           num_components,
                                           data+num_elem_per_component,
                                           num_elem_per_component);
  wind_components_specified = true;
}

/*****************************************************************************
assign_dir_mag_data
Purpose:(A helper function to be called from the input file to help with
         loading data defined in the input file.  Note that the dependent
         data requires use of STL-vectors, so this cannot be assigned from
         the input file directly.)
*****************************************************************************/
void
SimpleLookupWind::assign_dir_mag_data(
    const double * data,
    size_t         num_elem)
{
  altitude_table.load_data(
                data,
                num_elem);
  std::vector<size_t> dim_list;
  dim_list.push_back(1);
  dim_list.push_back(num_elem);
  wind_blowing_from_table.load_data(
                data+num_elem,
                dim_list);
  wind_magnitude_table.load_data(
                data+2*num_elem,
                dim_list);
  wind_components_specified = false;
}

/*****************************************************************************
assign_dir_mag_vert_data
Purpose:(A helper function to be called from the input file to help with
         loading data defined in the input file.  Note that the dependent
         data requires use of STL-vectors, so this cannot be assigned from
         the input file directly.)
*****************************************************************************/
void
SimpleLookupWind::assign_dir_mag_vert_data(
    const double * data,
    size_t         num_elem)
{
  assign_dir_mag_data(data, num_elem);

  include_vertical_component = true;
  std::vector<size_t> dim_list;
  dim_list.push_back(1);
  dim_list.push_back(num_elem);
  wind_vertical_up_table.load_data(
              data+3*num_elem,
              dim_list);
}