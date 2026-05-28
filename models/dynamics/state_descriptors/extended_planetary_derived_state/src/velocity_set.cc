/*******************************************************************************
PURPOSE:
   (The velocity-set provides data and methods for generating velocity,
    azimuth, flight-path angle, and altitude rate for a given velocity.
    The two intended velocities are inertially- and ECEF- referenced.
    The altitude rates between these two are very close, but not identical.

ASSUMPTIONS AND LIMITATIONS
   ((The velocity is intended to be expressed in the inertial reference frame.)
    (The model expects a reference to a transformation matrix from inertial to
      the reference frame in which the results will be expressed.
      The output reference frame will typically be topocentric or topodetic.)
    (The orientation of the output frame is not computed in this model;
      it is expected that it will be computed externally, and this model
      will access it via the transformation matrix reference.)
   )

PROGRAMMERS:
   (((Gary Turner) (OSR) (February 2015)
                      (New implementation of PlanetaryDerivedState for Antares)))
 ******************************************************************************/

#include <cmath>  // atan2, sqrt
#include <cstring> // NULL
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/velocity_set.hh"

/*****************************************************************************
VelocitySet
Purpose:(Constructor)
*****************************************************************************/
VelocitySet::VelocitySet(
        const double (&T_inrtl_this_in)[3][3])
   :
   velocity(),
   vel_xy(0.0),
   flight_path(0.0),
   azimuth(0.0),
   altitude_rate(0.0),
   initialized(false),
   T_inrtl_to_this(T_inrtl_this_in),
   inrtl_velocity(NULL)
{}


/*****************************************************************************
initialize
Purpose:(sets the pointer to the input velocity expressed in the inertial frame)
*****************************************************************************/
void
VelocitySet::initialize(
       const double * vel_in)
{
  if (initialized) {
    return;
  }
  if (vel_in == NULL) {
    CMLMessage::fail(
    __FILE__, __LINE__, "Invalid initialization\n",
    "Attempted to set the inrtl_velocity pointer to NULL.\n");
  }
  // Else (not already set and target is valid.
  inrtl_velocity = vel_in;
  initialized = true;
  return;
}

/*****************************************************************************
update
Purpose:(computes the class variables)
NOTE - Relying on the extended-planetary-derived-state call and the
       initialization routine to verify that the pointers are non-NULL
*****************************************************************************/
void
VelocitySet::update()
{
  // Compute the topo* (NED) flight path angle and azimuth for
  // the velocity vector.  Remember inrtl_velocity is expressed in the inertial
  // frame (even if it's a planet-relative velocity), so first transform
  // this vector into the topo* frame.  T_inrtl_to_this is a reference
  // to the transformation managed at the TopoContainer level
  jeod::Vector3::transform( T_inrtl_to_this,
                      inrtl_velocity,
                      velocity);

  vel_xy = std::sqrt( velocity[0] * velocity[0] + velocity[1] * velocity[1] );

  azimuth = std::atan2( velocity[1],
                        velocity[0] );
  flight_path = std::atan2( -velocity[2],
                             vel_xy );
  altitude_rate = -velocity[2];
  return;
}