/*******************************************************************************
PURPOSE:
   (Extends the planet-state model by including additional topodetic and
    topocentric functionality.  The methods used for topodetic and topocentric
    are identical, so thery are grouped into a separate class)

LIBRARY DEPENDENCIES:
   ((../src/topo_container.cc)
    (../src/velocity_set.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (February 2015)
                      (New implementation of PlanetaryDerivedState for Antares)))
 ******************************************************************************/

#ifndef ANTARES_TOPO_CONTAINER_HH
#define ANTARES_TOPO_CONTAINER_HH

#include "jeod/models/utils/planet_fixed/planet_fixed_posn/include/alt_lat_long_state.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh"

#include "velocity_set.hh"

/*****************************************************************************
TopoContainer
Purpose:(The TopoContainer class provides the velocity-related values that may
         be expressed for topocentric or topodetic, for inertial-referenced
         or relative-referenced.)
*****************************************************************************/
class TopoContainer {
 public:

  double T_pfix_to_this[3][3];   /* (--) ECEF to Topo* (NED) Transformation Matrix */
  double T_inrtl_to_this[3][3];  /* (--) Inertial to Topo* (NED) Transformation Matrix */
  double T_this_to_body[3][3];   /* (--) Topo* (NED) to body  Transformation Matrix */
  double E_this_to_body_YPR[3];  /* (rad)  Euler-angles (YPR) Topo* to body */

  VelocitySet relative_vel;  /* (--) Velocity parameters for relative (pfix) velocity */
  VelocitySet inertial_vel;  /* (--) Velocity parameters for inertial velocity */

 protected:
  bool initialized;
  const int & calc_rel_vel;
  const jeod::AltLatLongState & state;        /* (--) Reference to input state, this state
                                                      (ellip vs. sphere) determines whether
                                                      instance is topodetic vs. topocentric */
  const jeod::RefFrameRot * planet_rot_state;  /* (--) Pointer to planet's  rotational state */
  const jeod::RefFrameRot * vehicle_rot_state; /* (--) Pointer to vehicle's rotational state */

 public:
  TopoContainer( const jeod::AltLatLongState & state_in,
                 const int & calc_rel_vel);
  ~TopoContainer(){};
  void initialize( const jeod::RefFrameRot & planet_rot_state_in,
                   const jeod::RefFrameRot & vehicle_rot_state_in,
                   const double * inrtl_inrtl_vel_in,
                   const double * inrtl_rel_vel_in);
  void update();
 private:
   TopoContainer (const TopoContainer & rhs);
   void operator = (const TopoContainer & rhs);
};
#endif

