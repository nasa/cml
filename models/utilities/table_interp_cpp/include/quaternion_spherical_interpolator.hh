/*******************************************************************************
PURPOSE:
  (Spherical interpolation algorithm for handling quaternion interpolation.)

LIBRARY DEPENDENCY:
    ((../src/quaternion_spherical_interpolator.cc)
    )

PROGRAMMERS:
  (((Gary Turner) (OSR) (November 2017) (New implementation))
  )
*******************************************************************************/
#ifndef ANTARES_QUATERNION_SPHERICAL_INTERPOLATOR_HH
#define ANTARES_QUATERNION_SPHERICAL_INTERPOLATOR_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/quaternion/include/quat.hh"

// NOTE - this does not attempt to get any values or put any values anywhere.
//        It is the responsibility of the table to populate and extract data
//        from this class because the table class knows where the
//        interpolation data is located and where the interpolated values
//        need to go.

class QuaternionSphericalInterpolator
{
 public:
  // input
  bool use_spherical_interp; /* (--)
       Flag indicating that the interpolation will be attempted using the
       spherical interpolation algorithm.  Default True.*/
  jeod::Quaternion quat_0; /* (--)
        quaternion at one end of interpolation domain.*/
  jeod::Quaternion quat_1; /* (--)
        quaternion at other end of interpolation domain.*/
  double frac; /* (--) fraction of cell from quat_0 to quat_1.*/

  // output
  jeod::Quaternion quat_out; /* (--) interpolated quaternion.*/

 protected:
  double epsilon_interp; /* (--)
          small value used to determine whether to perform the trig function
          aspects of the spherical-interpolation for the quaternion
          determination.  Avoids the computation if within this
          fraction of either end of the interpolation domain.*/
  double epsilon_delta; /* (--)
          small value used to determine whether to perform the trig function
          aspects of the spherical-interpolation for the quaternion
          determination.  Avoids the computation if the scalar component of
          the combined quaternion is within this value of 1 ... which occurs
          if the two quaternions on either end of the interval are almost
          identical. */

 public:
  QuaternionSphericalInterpolator();
  virtual ~QuaternionSphericalInterpolator() {};

  void update();
  void set_epsilon_interp( double new_epsilon);
  void set_epsilon_delta( double new_epsilon);

 private:
  // Class not copyable:
  QuaternionSphericalInterpolator (const QuaternionSphericalInterpolator&);
  QuaternionSphericalInterpolator& operator = (
                                    const QuaternionSphericalInterpolator&);
};
#endif
