/*******************************************************************************
PURPOSE:
  (Table lookup model for a special case where the dependent variable(s)
  represent angles and consequently may wrap around.  This table-type is
  restricted to a single independent variable but may still have multiple
  dependent angular variables)

ASSUMPTIONS:
  (All dependent variables are angles, and are either all degrees or all
  radians, no mixing.)

LIMITATIONS:
  (Limited to a single independent variable.  Interpolation across
  multiple wraps is not universally defined.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2014) (New implementation for C++ models))
   ((Bingquan Wang) (OSR) (August 2014) (IV&V cleanup and refactored))
  )
*******************************************************************************/

#define _USE_MATH_DEFINES // M_PI
#include<cmath>  // M_PI

#include "../include/single_input_table_for_angles.hh"


/*****************************************************************************
Constructors
*****************************************************************************/
SingleInputTableForAngles::SingleInputTableForAngles(
     bool output_in_radians_) // optional, default to true
  :
  GenericSingleInputTable(),
  output_in_radians(output_in_radians_),
  half_circle(M_PI)
{}
/****************************************************************************/
SingleInputTableForAngles::SingleInputTableForAngles(
     double *dependent_variables,
     size_t num_vars,
     bool output_in_radians_) // optional, default to true
  :
  GenericSingleInputTable( dependent_variables, num_vars),
  output_in_radians(output_in_radians_),
  half_circle(M_PI)
{}
/****************************************************************************/
SingleInputTableForAngles::SingleInputTableForAngles(
     double & dependent_var,
     bool output_in_radians_) // optional, default to true
  :
  GenericSingleInputTable( dependent_var),
  output_in_radians(output_in_radians_),
  half_circle(M_PI)
{}
/****************************************************************************/
SingleInputTableForAngles::SingleInputTableForAngles(
     const DoublePtrVec & dependent_variables,
     bool output_in_radians_) // optional, default to true
  :
  GenericSingleInputTable( dependent_variables),
  output_in_radians(output_in_radians_),
  half_circle(M_PI)
{}

/*****************************************************************************
initialize
Purpose:(initializes the table.)
  Override of the GenericSingleInputTable::initialize() method.
  Extends it by setting the half-circle value to either pi (default,
  construction) or 180 depending on whether the angle outputs are in
  radians or degrees.
*****************************************************************************/
bool
SingleInputTableForAngles::initialize()
{
  if (!output_in_radians) {
    half_circle = 180.0;
  }

  return GenericSingleInputTable::initialize();
}

/*****************************************************************************
generate_output
Purpose:
  Override of the GenericSingleInputTable::generate_output() method,
  extending it to account for wrap-around conditions.
*****************************************************************************/
bool
SingleInputTableForAngles::generate_output()
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
      double output_value = data[data_ix];
      bound_to_half_circle( output_value);
      *out_ptr = output_value;

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
      // Typically, the interpolated value would be:
      // data[ix] * (1-frac) + data[ix+1] * frac
      //
      // However, when the data wraps between data[ix] and data[ix1], this
      // method fails.  Instead, compute the difference between the two data
      // points and modify the numerical value so that it represents the
      // shortest "distance" between the two angles.
      // e.g. the difference between +-170 degrees is 20 deg, not 340 deg
      double delta_angle = data[data_ix+1] - data[data_ix];
      bound_to_half_circle( delta_angle);

      // Interpolate between the two reference values using the shortest
      // route, as just determined.
      // With Dq = delta_angle:
      //  data[ix] * (1-frac) + data[ix1] * frac can be replaced with:
      //  data[ix] * (1-frac) + (data[ix]+Dq) * f
      //
      //  which can be simplified:
      //  data[ix] + Dq * f
      double output_value = data[data_ix] + frac * delta_angle;

      // Make sure the interpolated result lies in the range (-pi, pi]
      bound_to_half_circle( output_value);
      *out_ptr = output_value;

      // Move on to the next dependent variable.
      data_ix += num_data_elements_per_increment_of_index[0];
    }
  }
  return true;
}

/*****************************************************************************
bound_to_half_circle
Purpose:(sets the value to be in the range (-pi, pi])
*****************************************************************************/
void
SingleInputTableForAngles::bound_to_half_circle(
   double & angle)
{
  while (angle > half_circle) {
    angle -= 2 * half_circle;
  }
  while (angle <= -half_circle) {
    angle += 2 * half_circle;
  }
}
