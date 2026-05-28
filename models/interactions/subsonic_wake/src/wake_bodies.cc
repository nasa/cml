/*******************************TRICK HEADER******************************
PURPOSE: (Definition of the bodies in the wake profile.)

PROGRAMMERS:
  ( ((Gary Turner) (OSR) (Mar 2016) (Antares)
                (New adaptation to meet Antares coding standards))
    ((Bingquan Wang) (OSR) (Apr 2017) (Antares) (Fixed the compilation
                  warning of float-point number equality comparison)))
**********************************************************************/

#include <cmath> // sqrt
#include <cstring> // NULL
#include <limits> // numeric_limits<double>::min
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"
#include "../include/wake_bodies.hh"



/*****************************************************************************
Constructors
*****************************************************************************/
WakeGeneratingBody::WakeGeneratingBody(
      const double * inertial_pos_in,
      const double * inertial_vel_in,
      const double * CoM_wrt_struc_in,
      const double (& T_inrtl_to_body_in)[3][3],
      const double (& T_struc_to_body_in)[3][3],
      const double * freestream_vel_in,
      const double & freestream_mach_in,
      const double & freestream_density_in,
      const double & total_alpha_in)
   :
   inertial_pos( inertial_pos_in),
   inertial_vel( inertial_vel_in),
   CoM_wrt_struc( CoM_wrt_struc_in),
   T_inrtl_to_body( T_inrtl_to_body_in),
   T_struc_to_body( T_struc_to_body_in),
   freestream_vel( freestream_vel_in),
   freestream_mach( freestream_mach_in),
   freestream_density( freestream_density_in),
   total_alpha(total_alpha_in),
   freestream_vel_mag(0.0),
   unitvec_freestream{0.0, 0.0, 0.0}
{
  if (inertial_pos == NULL || inertial_vel == NULL || freestream_vel == NULL ||
      CoM_wrt_struc_in == NULL) {
    CMLMessage::fail(
     __FILE__,__LINE__,"Invalid construction\n",
     "Null pointers passed in at construction time.\n");
  }
}
/****************************************************************************/
WakeFollowingBody::WakeFollowingBody(
      const double & area_in,
      const double * inertial_pos_in,
      const double * inertial_vel_in,
      WakeGeneratingBody & objectA_in,
      const double * grid_origin_in)
   :
   grid_origin(grid_origin_in),
   generate_force(false),
   area(area_in),
   inertial_pos( inertial_pos_in),
   inertial_vel( inertial_vel_in),
   objectA( objectA_in),
   separation_distance(0.0),
   trailing_distance(0.0),
   radial_distance(0.0),
   axial_sep_vel(0.0),
   pos_wrt_grid_in_bodyA{ 0.0, 0.0, 0.0},
   pos_wrt_objectA_bodyA{ 0.0, 0.0, 0.0},
   pos_wrt_objectA_inrtl{ 0.0, 0.0, 0.0}

{
  if (inertial_pos == NULL || inertial_vel == NULL || grid_origin == NULL) {
    CMLMessage::fail(
     __FILE__,__LINE__,"Invalid construction\n",
     "Null pointers passed in at construction time.\n");
  }
}
/****************************************************************************/
WakeFollowingBodyNoForce::WakeFollowingBodyNoForce(
      const double & area_in,
      const double * inertial_pos_in,
      const double * inertial_vel_in,
      WakeGeneratingBody & objectA_in,
      const double * grid_origin_in)
   :
   WakeFollowingBody( area_in,
                      inertial_pos_in,
                      inertial_vel_in,
                      objectA_in,
                      grid_origin_in)
{}
/****************************************************************************/
WakeFollowingBodyWithForce::WakeFollowingBodyWithForce(
      const double & area_in,
      const double * inertial_pos_in,
      const double * inertial_vel_in,
      const double (& T_inrtl_to_body_in)[3][3],
      const double (& T_struc_to_body_in)[3][3],
      WakeGeneratingBody & objectA_in,
      const double * grid_origin_in)
   :
   WakeFollowingBody( area_in,
                      inertial_pos_in,
                      inertial_vel_in,
                      objectA_in,
                      grid_origin_in),
   T_inrtl_to_body( T_inrtl_to_body_in),
   T_struc_to_body( T_struc_to_body_in),
   Cd(0.0),
   force{0.0, 0.0, 0.0}
{}

/*****************************************************************************
compute_freestream_vel_mag
Purpose:(computes the magnitude of the freestream velocity)
*****************************************************************************/
void
WakeGeneratingBody::compute_freestream_vel_mag()
{
  freestream_vel_mag = jeod::Vector3::vmag( freestream_vel);
  if (MathUtils::is_near_equal(freestream_vel_mag, 0.0)) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid external connecctions\n",
      "The magnitude of the freestream velocity vector (", freestream_vel_mag, ") is very small.\n"
      "This should be protected against by evaluating the freestream mach "
      "number,\nbut the mach number (", freestream_mach, ") is within acceptable bounds.\n"
      "This suggests that the external connections are inconsistent.\n"
      "Check configuration.\n"
      "Cannot proceed without a freestream velocity.\n");
  }
  jeod::Vector3::scale( freestream_vel,
                  (1/freestream_vel_mag),
                  unitvec_freestream);
}

/*****************************************************************************
compute_relative_state
Purpose:(Computes the state relative to objectA)
*****************************************************************************/
void
WakeFollowingBody::compute_relative_state()
{
  double scratch[3]; // temporary 3-vector for internal use.

  //Relative position, B wrt A
  jeod::Vector3::diff( inertial_pos,
                 objectA.inertial_pos,
                 pos_wrt_objectA_inrtl); // useful in force calc also
                                             // add to class, not scratch
  jeod::Vector3::transform( objectA.T_inrtl_to_body,
                      pos_wrt_objectA_inrtl,
                      pos_wrt_objectA_bodyA);


  // Position of body B in the wake grid and trailing distance
  // scratch = offset of wake grid origin wrt objectA CoM in strucA frame
  jeod::Vector3::diff( grid_origin,
                 objectA.CoM_wrt_struc,
                 scratch);
  // scratch = offset of wake grid origin wrt objectA CoM in bodyA frame
  jeod::Vector3::transform( objectA.T_struc_to_body,
                      scratch);
  // relative position of bodyB CoM wrt grid origin in bodyA frame
  jeod::Vector3::diff( pos_wrt_objectA_bodyA,
                 scratch,
                 pos_wrt_grid_in_bodyA);


  // Freestream calculations:
  objectA.compute_freestream_vel_mag();

  // Relative velocity calculations, B wrt A
  jeod::Vector3::diff( inertial_vel,
                 objectA.inertial_vel,
                 scratch); // Relative velocity, B wrt A, in inertial
  jeod::Vector3::transform( objectA.T_inrtl_to_body,
                      scratch); // Relative velocity, B wrt A in bodyA

  // compute the values relevant for bodyB:
  axial_sep_vel = -jeod::Vector3::dot( scratch, // Rel vel, B wrt A in A
                                 objectA.unitvec_freestream);

  separation_distance = jeod::Vector3::vmag( pos_wrt_grid_in_bodyA);
  trailing_distance =  -jeod::Vector3::dot(  pos_wrt_grid_in_bodyA,
                                       objectA.unitvec_freestream);
  radial_distance = std::sqrt( (separation_distance * separation_distance) -
                               (trailing_distance   * trailing_distance));
}

/****************************************************************************/
void
WakeFollowingBodyWithForce::compute_relative_state()
{
  WakeFollowingBody::compute_relative_state();
};

/*****************************************************************************
set_generate_force
Purpose:(Error message, present for consistency with WakeFollowingBodyWithForce.)
*****************************************************************************/
void
WakeFollowingBodyNoForce::set_generate_force(bool target)
{
  if (target) {
    CMLMessage::error(
    __FILE__,__LINE__,"Invalid configuration request \n",
    "This object is not configured for computing forces acting on it.\n"
    "Request failed.\n");
  }
}
/****************************************************************************/
void
WakeFollowingBodyWithForce::set_generate_force(bool target)
{
  generate_force = target;
  if (!generate_force) {
    jeod::Vector3::initialize( force);
  }
}

/*****************************************************************************
compute_force
Purpose:(computes the wake force on the trailing body)
*****************************************************************************/
void
WakeFollowingBodyWithForce::compute_force(double force_mag)
{
  if (!generate_force) {
    return;
  }

  if ( std::fabs(force_mag) < std::numeric_limits<double>::min()) {
    jeod::Vector3::initialize( force);
    return;
  }

  // else get the unit vector from bodyB to bodyA,
  // expressed in bodyB structural frame.
  double scratch[3];
  jeod::Vector3::transform( T_inrtl_to_body,
                      pos_wrt_objectA_inrtl,
                      scratch); // A to B in bodyB
  jeod::Vector3::transform_transpose( T_struc_to_body,
                                scratch); // A to B in strucB

  jeod::Vector3::normalize(scratch); // now a unit vector but still wrong direction
  jeod::Vector3::scale( scratch,
                  -force_mag, // switch direction
                  force);     // in B structural frame.
}