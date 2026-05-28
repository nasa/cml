/*******************************************************************************
PURPOSE:
   (Extends the planet-state model by including additional topodetic and
    topocentric functionality.  The methods used for topodetic and topocentric
    are identical, so thery are grouped into a separate class)

LIBRARY DEPENDENCIES:
   ((topo_container.o)
    (velocity_set.o))

PROGRAMMERS:
   (((Gary Turner) (OSR) (February 2015)
                      (New implementation of PlanetaryDerivedState for Antares)))
*******************************************************************************/

#include "jeod/models/utils/math/include/matrix3x3_inline.hh"
#include "jeod/models/utils/orientation/include/orientation.hh"

#include "../include/topo_container.hh"


/*****************************************************************************
TopoContainer
Purpose:Constructor
*****************************************************************************/
TopoContainer::TopoContainer(
         const jeod::AltLatLongState & state_in,
         const int & calc_rel_vel_in)
   :
   T_pfix_to_this(),
   T_inrtl_to_this(),
   T_this_to_body(),
   E_this_to_body_YPR(),
   relative_vel(T_inrtl_to_this),
   inertial_vel(T_inrtl_to_this),
   initialized(false),
   calc_rel_vel(calc_rel_vel_in),
   state(state_in),
   planet_rot_state(NULL),
   vehicle_rot_state(NULL)
{}

/*****************************************************************************
initialize
Purpose:(Sets the necessary pointers)
*****************************************************************************/
void
TopoContainer::initialize(
    const jeod::RefFrameRot & planet_rot_state_in,
    const jeod::RefFrameRot & vehicle_rot_state_in,
    const double * inrtl_inrtl_vel_in,
    const double * inrtl_rel_vel_in)
{
  planet_rot_state = &planet_rot_state_in;
  vehicle_rot_state = &vehicle_rot_state_in;

  // Give inertial_vel the inertial velocity
  inertial_vel.initialize(inrtl_inrtl_vel_in); // NULL check performed there.
  // Give relative_vel the relative velocity (expressed in inertial frame)
  relative_vel.initialize(inrtl_rel_vel_in);   // Likewise

  initialized = true;
  return;
}


/*****************************************************************************
update
Purpose:(Computes the topo* representations)
*****************************************************************************/
void
TopoContainer::update()
{
  double angle_pfix_to_this[3] = { state.longitude,
                                -(state.latitude + M_PI_2),
                                0.0 };
  // Compute transformation from pfix (ECEF) to topo* (NED)
  jeod::Orientation::compute_matrix_from_euler_angles( jeod::Orientation::Yaw_Pitch_Roll,
                                                       angle_pfix_to_this,
                                                       T_pfix_to_this);

  // Compute transformation from inertial to topo* (NED)
  jeod::Matrix3x3::product(T_pfix_to_this,
                     planet_rot_state->T_parent_this,
                     T_inrtl_to_this);

  // Compute transformation from topo* (NED) to body
  jeod::Matrix3x3::product_right_transpose(
                       vehicle_rot_state->T_parent_this,
                       T_inrtl_to_this,
                       T_this_to_body);
  // generate Euler angles of same:
  jeod::Orientation::compute_euler_angles_from_matrix( T_this_to_body,
                                                       jeod::Orientation::Yaw_Pitch_Roll,
                                                       E_this_to_body_YPR);

  if (calc_rel_vel>0) {
    relative_vel.update();
  }

  inertial_vel.update();

  return;
}
