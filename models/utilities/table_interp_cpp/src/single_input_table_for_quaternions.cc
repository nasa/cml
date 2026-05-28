/*******************************************************************************
PURPOSE:
  (Table lookup model for a special case where the dependent variables
  represent one quaternion sets.  This table has access to the
  spherical interpolation algorithm)

ASSUMPTIONS:
  (This table contains 4 dependent variables - a quaternion - and a single
  independent variable.)

LIMITATIONS:
  (Limited to a single independent variable.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (November 2017) (New implementation))
  )
*******************************************************************************/

#include "../include/single_input_table_for_quaternions.hh"

/*****************************************************************************
Constructors
*****************************************************************************/
SingleInputTableForQuaternions::SingleInputTableForQuaternions()
  :
  GenericSingleInputTable(),
  sph_interp()
{}
/****************************************************************************/
SingleInputTableForQuaternions::SingleInputTableForQuaternions(
     jeod::Quaternion & quat)
  :
  GenericSingleInputTable(),
  sph_interp()
{
  add_dependent( quat.scalar);
  add_dependent( quat.vector[0]);
  add_dependent( quat.vector[1]);
  add_dependent( quat.vector[2]);
}
/****************************************************************************/
SingleInputTableForQuaternions::SingleInputTableForQuaternions(
     double *dependent_variables,
     size_t num_vars)
  :
  GenericSingleInputTable( dependent_variables, num_vars),
  sph_interp()
{}
/****************************************************************************/
SingleInputTableForQuaternions::SingleInputTableForQuaternions(
     const DoublePtrVec & dependent_variables)
  :
  GenericSingleInputTable( dependent_variables),
  sph_interp()
{}

/*****************************************************************************
initialize
Purpose:(initializes the table.)
*****************************************************************************/
bool
SingleInputTableForQuaternions::initialize()
{
  if (output.size() != 4) {
    CMLMessage::error(
      __FILE__,__LINE__,"Initialization error:\n",
      "This type of table can only populate a quaternion but there are ",
      output.size()," outputs specified.\nThere must be 4 outputs.\n");
    return false;
  }

  return GenericSingleInputTable::initialize();
}

/*****************************************************************************
generate_output
Purpose:(generates table output.)
*****************************************************************************/
bool
SingleInputTableForQuaternions::generate_output()
{
  // If there is no
  // interpolation to perform, default back to the parent class method.
  if ( data_point_index.size() == 1 ) {
    return GenericSingleInputTable::generate_output();
  }
  // else, attempt the spherical interpolation:


  // Run prechecks, abort if fail.
  //    This is not likely to return false, due to other tests
  if (!precheck_output()) {
    return false;
  }
  // Each dependent variable has an identically-sized data table embedded
  // within the full data table.
  // For each dependent variable, access the same relative data set.

  // copy the data into the spherical interpolator.

  // data-access index for each variable is equal to the sum of:
  //   - how far into each data set the current lookup has progressed
  //     - data_point_index[0] and [1]
  //   - the basis-index for each variable.
  //     - These are an integer multiple of the number of values each
  //       variable has available.
  //     - for scalar (the first variable), this value is 0.
  sph_interp.quat_0.scalar = data[ data_point_index[0]];
  sph_interp.quat_1.scalar = data[ data_point_index[1]];

  for (size_t ii = 0; ii<3; ++ii) {
    size_t var_base_ix = (ii+1) * num_data_elements_per_increment_of_index[0];
    sph_interp.quat_0.vector[ii] = data[ var_base_ix + data_point_index[0]];
    sph_interp.quat_1.vector[ii] = data[ var_base_ix + data_point_index[1]];
  }
  sph_interp.frac = independents[0].first->fraction;//data_point_weight[1];

  // run the interpolator:
  sph_interp.update();

  // copy the data back out:
  *output[0] = sph_interp.quat_out.scalar;
  *output[1] = sph_interp.quat_out.vector[0];
  *output[2] = sph_interp.quat_out.vector[1];
  *output[3] = sph_interp.quat_out.vector[2];

  return true;
}
