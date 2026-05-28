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

LIBRARY DEPENDENCIES:
   ((../src/velocity_set.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (February 2015)
                      (New implementation of PlanetaryDerivedState for Antares)))
 ******************************************************************************/

#ifndef ANTARES_TOPO_CONTAINER_VELOCITY_SET_HH
#define ANTARES_TOPO_CONTAINER_VELOCITY_SET_HH

/*****************************************************************************
VelocitySet
Purpose:(Collection of velocity elements for topocentric and topodetic frame
         representations of the inertial and relative velocities.)
*****************************************************************************/
class VelocitySet{
 public:
  double velocity[3];   /* (m/s) 3-vector velocity in arbitrary frame */
  double vel_xy;        /* (m/s) x-y-plane component of velocity */
  double flight_path;   /* (rad) flight-path angle */
  double azimuth;       /* (rad) azimuth */
  double altitude_rate; /* (m/s) Altitude rate */
 protected:
  bool initialized;     /* (--) Indicates that all necessary inputs are set */
  const double (&T_inrtl_to_this)[3][3]; /* (--)
            reference to the inertial-to-topo* (NED) transformation matrix.
            This is computed in TopoContainer::update() */
  const double * inrtl_velocity;         /* (m/s)
            pointer to either the relative- or inertial- velocity 3-vector */

 public:
  explicit VelocitySet(const double (&T_inrtl_topo_in)[3][3]);
  void initialize(const double * vel_in);
  void update();

 private:
   VelocitySet (const VelocitySet & rhs);
   void operator = (const VelocitySet & rhs);
};
#endif
