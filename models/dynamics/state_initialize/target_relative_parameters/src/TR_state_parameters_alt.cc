/********************************* TRICK HEADER *******************************
PURPOSE:
  (Provide the capability to transform between position/velocity
   vectors and a set of target relative parameters.)

PROGRAMMERS:
  (
   ((Jeremy Rea) (NASA) (May 2017) (Initial implementation))
  )

*******************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils::*

#include "../include/TR_state_parameters_alt.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
TargetRelative_StateParameter_Alt::TargetRelative_StateParameter_Alt()
  :
  TargetRelative_StateParam(),
  target_point{0.0, 0.0, 0.0},
  omega_planet{0.0, 0.0, 0.0},
  target_r_mag(0.0)
{}

/*****************************************************************************
initialize
Purpose:(Initialize uhat_Ref given a position reference vector.
         Call initialize_internal(...) to initialize all other parameters.)
*****************************************************************************/
void
TargetRelative_StateParameter_Alt::initialize(
     const double R_Ref[3] )
{
  /* Compute unit target position vector */
  if (!Target_Reference_frame.compute_frame( target_point,
                                             R_Ref)) {
    CMLMessage::error( __FILE__,__LINE__,
      "Target and Reference position vectors are nearly aligned.\n",
      "This is an invalid configuration.\n"
      "Pick a different reference vector.\n"
      "Initialization failed.\n");
    return;
  }

  target_r_mag = jeod::Vector3::vmag( target_point);

  omega_mag = jeod::Vector3::vmag(omega_planet);
  if ( MathUtils::is_near_equal( omega_mag, 0) ) {
    CMLMessage::error( __FILE__,__LINE__,
      "Planetary rotation is zero.  Setting unit planetary rotation\n"
      "vector to [0,0,1].  Check configuration.\n");
    omega_planet[2] = 1.0;
  }
  // construct Omega_Target_frame basis vectors
  if (!Omega_Target_frame.compute_frame( omega_planet,
                                         target_point)) {
    CMLMessage::warn( __FILE__,__LINE__,
      "Target position vector and planetary rotation vector are nearly aligned."
      "\nThis is unusual but manageable.  Check configuration.\n");
    sin_phi_omega = 0.0;
    cos_phi_omega = 1.0;
    // set flag to block computation of the rotation of the target about the
    // omega vector.
    omega_target_aligned = true;
  }
  // compute sine and cosine of phi_omega, the angle between the omega vector
  // and the target vector.
  else {
    cos_phi_omega = jeod::Vector3::dot( Omega_Target_frame.u_hat,
                                  Target_Reference_frame.u_hat);
    sin_phi_omega = std::sqrt(1 - cos_phi_omega * cos_phi_omega);
  }
  flag_initialized = true;
}

/*****************************************************************************
compute_params_from_vectors
Purpose:
  (Compute the set of target relative parameters from the position vector
  w.r.t the planet center and expressed in the Planet-Center Planet-Fixed
  coordinate system and  the velocity vector w.r.t. the inertial frame
  and expressed in the Planet-Centered Planet-Fixed coordinate system.
  This is used for dispersing the current state:
  - the state is passed into this method to generate a parameter set,
  - the correlated dispersions are applied to this parameter set
  - the modified parameter set is used to generate the modified state via
    compute_*_from_parameters())
*****************************************************************************/
void
TargetRelative_StateParameter_Alt::compute_params_from_vectors(
  const double   (&R)[3],
  const double   (&V_wrt_PCI)[3],
  TR_ParameterSet & param_set)
{
  jeod::Vector3::copy( R, position);
  jeod::Vector3::copy( V_wrt_PCI, velocity);
  TargetRelative_StateParameter_Alt::compute_params_from_vectors();
  param_set = *this;
}
/****************************************************************************/
void
TargetRelative_StateParameter_Alt::compute_params_from_vectors()
{
  if ( TargetRelative_StateParam::compute_params_from_vectors()) {

    // computation of altitude depends on formulation, so it is split off here:
    altitude = jeod::Vector3::vmag(position) - target_r_mag;
  }
}

/*****************************************************************************
compute_position_from_params
Purpose:
 (Compute the position vector w.r.t. the planet center and expressed in
  the Planet-Centered Planet-Fixed coordinate system from the geodetic
  altitude, range angle, and either rotation angle or cross angle.)
*****************************************************************************/
void
TargetRelative_StateParameter_Alt::compute_position_from_params(
  double        relative_altitude,
  double        theta_Rng_,
  double        theta_Rot_OR_phi,
  InputPosAngle input_theta_type,
  double        (&R)[3])
{
  altitude = relative_altitude;
  theta_Rng = theta_Rng_;
  if (input_theta_type == INPUT_THETA_ROT) {
    phi_Cross = theta_Rot_OR_phi;
  } else {
    theta_Rot = theta_Rot_OR_phi;
  }
  TargetRelative_StateParameter_Alt::compute_position_from_params(
                                                            input_theta_type);

  jeod::Vector3::copy( position, R);
}
/****************************************************************************/
void
TargetRelative_StateParameter_Alt::compute_position_from_params(
  InputPosAngle input_theta_type)
{
  if (TargetRelative_StateParam::compute_position_from_params(
                                                           input_theta_type)) {
    //----------------------------------------------------------------------------
    // Compute position vector with magnitude that gives desired relative
    // altitude and direction as just computed
    //----------------------------------------------------------------------------
    jeod::Vector3::scale( uhat_R,
                    altitude + target_r_mag,
                    position);
  }
}
