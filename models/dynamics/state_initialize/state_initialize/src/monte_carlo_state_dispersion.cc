/********************************* TRICK HEADER *******************************
PURPOSE: (Disperses the state directly.  Allows the application of dispersions
          specified in one coordinate system with nominal state values specified
          in another.)

PROGRAMMERS:
    (((Gary Turner) (OSR) (Jan 2019) (Antares) (initial)))
    )
******************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"

#include "../include/monte_carlo_state_dispersion.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
MonteCarloStateDispersion::MonteCarloStateDispersion(
    jeod::DynBodyInitTransState & trans_init_in)
  :
  trans_init(trans_init_in),
  frame(Inertial),
  disperse_pv(false)
{
  jeod::Vector3::initialize(position_dispersion);
  jeod::Vector3::initialize(velocity_dispersion);
  jeod::Vector3::initialize(position_dispersion_inertial);
  jeod::Vector3::initialize(velocity_dispersion_inertial);
}

/*****************************************************************************
disperse_trans_state
Purpose:(applies dispersions to the state-initialize's instance of trans-init.)
*****************************************************************************/
void
MonteCarloStateDispersion::disperse_trans_state()
{
  switch( frame){
  case Inertial:
    // Copy specified values to internal variables
    jeod::Vector3::copy( position_dispersion, position_dispersion_inertial);
    jeod::Vector3::copy( velocity_dispersion, velocity_dispersion_inertial);
    break;
  case Lvlh:
    double T_inrtl_lvlh[3][3];
    MathUtils::generate_inertial_to_lvlh( trans_init.position,
                                          trans_init.velocity,
                                          T_inrtl_lvlh);
    jeod::Vector3::transform_transpose( T_inrtl_lvlh,
                                  position_dispersion,
                                  position_dispersion_inertial);
    jeod::Vector3::transform_transpose( T_inrtl_lvlh,
                                  velocity_dispersion,
                                  velocity_dispersion_inertial);
    break;
  case UVW:
    double T_inrtl_uvw[3][3];
    MathUtils::generate_inertial_to_uvw( trans_init.position,
                                          trans_init.velocity,
                                          T_inrtl_uvw);
    jeod::Vector3::transform_transpose( T_inrtl_uvw,
                                  position_dispersion,
                                  position_dispersion_inertial);
    jeod::Vector3::transform_transpose( T_inrtl_uvw,
                                  velocity_dispersion,
                                  velocity_dispersion_inertial);
    break;
  default:
    CMLMessage::fail(
      __FILE__, __LINE__, "Invalid Coordinate frame\n",
      "Specified frame option not found.\n"
      "Valid options are Inertial Lvlh, and UVW.\n");
  }
  jeod::Vector3::incr( position_dispersion_inertial,
                 trans_init.position);
  jeod::Vector3::incr( velocity_dispersion_inertial,
                 trans_init.velocity);
}
