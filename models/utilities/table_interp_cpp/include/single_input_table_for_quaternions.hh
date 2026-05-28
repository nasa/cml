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

 LIBRARY DEPENDENCY:
    ((../src/single_input_table_for_quaternions.cc)
    )

PROGRAMMERS:
  (((Gary Turner) (OSR) (November 2017) (New implementation))
  )
*******************************************************************************/
#ifndef ANTARES_SINGLE_INPUT_TABLE_FOR_QUATERNIONS_HH
#define ANTARES_SINGLE_INPUT_TABLE_FOR_QUATERNIONS_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/quaternion/include/quat.hh"

#include "generic_single_input_table.hh"
#include "quaternion_spherical_interpolator.hh"

class SingleInputTableForQuaternions : public GenericSingleInputTable
{
 protected:
  QuaternionSphericalInterpolator sph_interp; /* (--)
      Instance of the interpolator.*/
 public:
  SingleInputTableForQuaternions( );
  SingleInputTableForQuaternions( jeod::Quaternion & quat);
  SingleInputTableForQuaternions( double *dependent_variables,
                                  size_t num_vars);
  explicit SingleInputTableForQuaternions(
                                    const DoublePtrVec & dependent_variables);
  virtual ~SingleInputTableForQuaternions() {};

  virtual bool initialize() override;

  void set_spherical_interp( bool new_flag) {
    sph_interp.use_spherical_interp = new_flag;
  }
  void set_epsilon_interp( double new_epsilon) {
    sph_interp.set_epsilon_interp(new_epsilon);
  }
  void set_epsilon_delta( double new_epsilon) {
    sph_interp.set_epsilon_delta(new_epsilon);
  }

 protected:
  virtual bool generate_output() override;
 private:
  // Disable the copy/assignment operators
  SingleInputTableForQuaternions (const SingleInputTableForQuaternions&);
  SingleInputTableForQuaternions& operator = (
                                  const SingleInputTableForQuaternions&);
};
#endif
