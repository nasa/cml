/********************************* TRICK HEADER *******************************
PURPOSE:
  (Provide the capability to transform between position/velocity
   vectors and a set of target relative parameters.)

PROGRAMMERS:
  (
   ((Jeremy Rea) (NASA) (May 2017) (Initial implementation))
  )

*******************************************************************************/

#include <cmath>  /* abs */
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils::*

#include "../include/TR_state_parameters.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
TargetRelative_StateParameter::TargetRelative_StateParameter()
  :
  TargetRelative_StateParam(),
  target_point_altitude(0.0),
  target_point_latitude(0.0),
  target_point_longitude(0.0),
  tolerance(1e-08),
  iter_limit(1000),
  pfix_position()
{}

/*****************************************************************************
initialize
Purpose:(Initialize uhat_Ref_PCPF given a reference geodetic altitude,
         reference geodetic latitude, and reference longitude.
         Call initialize_internal() to initialize all other parameters.)
*****************************************************************************/
void
TargetRelative_StateParameter::initialize(
  double ref_geodetic_altitude,
  double ref_geodetic_latitude,
  double ref_longitude)
{
  if(!initialize_check_config()) { return;}

  //------------------------------------------------------------------
  // Compute target position vector for passing to initialiae_internal
  //------------------------------------------------------------------

  // Assign inputs to the planet-fixed-position instance.
  pfix_position.ellip_coords.altitude  = ref_geodetic_altitude;
  pfix_position.ellip_coords.latitude  = ref_geodetic_latitude;
  pfix_position.ellip_coords.longitude = ref_longitude;
  // Compute reference position vector in planet fixed coordinates
  pfix_position.update_from_ellip(pfix_position.ellip_coords);

  //-Pass a copy of cartesian coordinates through to initialize_internal
  double cart_coords[3];
  jeod::Vector3::copy(pfix_position.cart_coords,
                cart_coords);
  initialize_internal(cart_coords);
}

/*****************************************************************************
initialize
Purpose:(Initialize uhat_Ref_PCPF given a position reference vector.
         Call initialize_internal(...) to initialize all other parameters.)
*****************************************************************************/
void
TargetRelative_StateParameter::initialize(
     const double R_Ref_PCPF[3] )
{
  if(!initialize_check_config()) { return;}

  // Pass through to initialize_internal
  initialize_internal(R_Ref_PCPF);
}

/*****************************************************************************
initialize_check_config
Purpose: Internal method to check config, used by both initialize methods.
*****************************************************************************/
bool
TargetRelative_StateParameter::initialize_check_config()
{
  //-----------------------------------------------------
  // Exit if already initialized
  //-----------------------------------------------------
   if ( flag_initialized ) {return false;}

  //-----------------------------------------------------
  // pfix-position must have been configured prior to
  // initialization with the set_planet method.
  //-----------------------------------------------------
  if (pfix_position.planet == NULL) {
    CMLMessage::fail( __FILE__,__LINE__,
    "Planet has not been assigned to the PlanetFixedPosition instance\n"
    "(pfix_position) in TargetRelative_StateParameters.\n"
    "This instance is necessary for correct operation.\n"
    "If not constrained to the pfix frame, use "
    "TargetRelative_StateParameter_Alt instead.\n");
  }

  // passed all checks:
  return true;
}

/*****************************************************************************
initialize_internal
Purpose:(Initialize parameters:
          omega_mag
          Omega_Target_frame
          Target_Reference_frame
          sin_phi_omega
          cos_phi_omega)
*****************************************************************************/
void
TargetRelative_StateParameter::initialize_internal(
     const double R_Ref_PCPF[3] )
{
  //-----------------------------------------------------
  // Compute target position vector and
  // Target-Reference-frame basis vectors
  //-----------------------------------------------------

  // Assign inputs to the planet-fixed-position instance.
  pfix_position.ellip_coords.altitude  = target_point_altitude;
  pfix_position.ellip_coords.latitude  = target_point_latitude;
  pfix_position.ellip_coords.longitude = target_point_longitude;

  // Compute target position vector in planet fixed coordinates
  pfix_position.update_from_ellip(pfix_position.ellip_coords);
  /* Compute unit target position vector */
  if (!  Target_Reference_frame.compute_frame( pfix_position.cart_coords,
                                               R_Ref_PCPF)) {
    CMLMessage::fail( __FILE__,__LINE__,
     "Target and Reference position vectors are nearly aligned.\n",
     "This is an invalid configuration.\n"
     "Pick a different reference vector.\n");
  }

  //---------------------------------------------------------------------
  // Compute planetary rotation vector w.r.t. inertial frame, expressed in
  // planet-fixed, and
  // Omega_Target_frame basis vectors
  //---------------------------------------------------------------------
  double omega_pfix_wrt_inrtl__pfix[3];
  jeod::Vector3::copy( pfix_position.planet->pfix.state.rot.ang_vel_this,
                 omega_pfix_wrt_inrtl__pfix);

  omega_mag = jeod::Vector3::vmag(omega_pfix_wrt_inrtl__pfix);

  if ( MathUtils::is_near_equal( omega_mag, 0) ) {
    CMLMessage::warn( __FILE__,__LINE__,
      "Planetary rotation is zero.  Setting unit planetary rotation\n"
      "vector to [0,0,1].  Check configuration.\n");
    omega_pfix_wrt_inrtl__pfix[2] = 1.0;
  }
  // construct Omega_Target_frame basis vectors
  if (!Omega_Target_frame.compute_frame( omega_pfix_wrt_inrtl__pfix,
                                         pfix_position.cart_coords)) {
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
set_planet
Purpose:(Set pointer to planet)
*****************************************************************************/
void
TargetRelative_StateParameter::set_planet( const jeod::Planet * planet_IN )
{
  if (planet_IN == NULL) {
    CMLMessage::fail( __FILE__,__LINE__,
      "Planet passed in was NULL.\n"
      "Pfix-position has not been configured correctly or an invalid pointer\n"
      "was manually specified.\n"
      "Attempt to set planet pointer failed.\n");
  }
  // pfix-position has a non-const Planet pointer for some weird reason.
  // But this is going back to JEOD, and JEOD owns the Planet anyway, so as
  // long as we aren't touching it, that's probably ok.
  pfix_position.planet = const_cast<jeod::Planet*>(planet_IN);
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
Notes:
> For execution from code, passing by reference is available.
  For execution via SWIG, only pass-by-value is available for primitive data
  types (double in this case), so a pass-by-value version is made available.
*****************************************************************************/
void
TargetRelative_StateParameter::compute_params_from_vectors_SWIG(
  double R_PCPF[3],
  double V_wrt_PCI_PCPF[3],
  TR_ParameterSet & param_set)
{
  if (R_PCPF == nullptr || V_wrt_PCI_PCPF == nullptr) {
    CMLMessage::error(__FILE__,__LINE__,
      "Passed a null pointer in to compute_params_from_vectors_SWIG.\n"
      "Aborting.\n");
    return;
  }
  jeod::Vector3::copy( R_PCPF, position);
  jeod::Vector3::copy( V_wrt_PCI_PCPF, velocity);
  TargetRelative_StateParameter::compute_params_from_vectors();
  param_set = *this;
}
/****************************************************************************/
void
TargetRelative_StateParameter::compute_params_from_vectors(
  const double   (&R_PCPF)[3],
  const double   (&V_wrt_PCI_PCPF)[3],
  TR_ParameterSet & param_set)
{
  jeod::Vector3::copy( R_PCPF, position);
  jeod::Vector3::copy( V_wrt_PCI_PCPF, velocity);
  TargetRelative_StateParameter::compute_params_from_vectors();
  param_set = *this;
}
/****************************************************************************/
void
TargetRelative_StateParameter::compute_params_from_vectors()
{
  if ( TargetRelative_StateParam::compute_params_from_vectors()) {

    // computation of altitude depends on formulation, so it is split off here:
    pfix_position.update_from_cart( position);
    altitude = pfix_position.ellip_coords.altitude;
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
TargetRelative_StateParameter::compute_position_from_params_SWIG(
  double        geodetic_altitude,
  double        theta_Rng_,
  double        theta_Rot_OR_phi,
  InputPosAngle input_theta_type,
  double        R_PCPF[3])
{
  //-----------------------------------------------------
  // Check for improper inputs
  //-----------------------------------------------------
  if (R_PCPF == nullptr) {
    CMLMessage::error( __FILE__,__LINE__,
      "NULL vectors cannot be populated by target-reference parameters.\n"
      "Aborting calculation of position vector.\n");
    return;
  }
  altitude = geodetic_altitude;
  theta_Rng = theta_Rng_;
  if (input_theta_type == INPUT_THETA_ROT) {
    theta_Rot = theta_Rot_OR_phi;
  } else {
    phi_Cross = theta_Rot_OR_phi;
  }
  TargetRelative_StateParameter::compute_position_from_params(
                                                            input_theta_type);
  jeod::Vector3::copy( position, R_PCPF);
}
/****************************************************************************/
void
TargetRelative_StateParameter::compute_position_from_params(
  double        geodetic_altitude,
  double        theta_Rng_,
  double        theta_Rot_OR_phi,
  InputPosAngle input_theta_type,
  double        (&R_PCPF)[3])
{
  altitude = geodetic_altitude;
  theta_Rng = theta_Rng_;
  if (input_theta_type == INPUT_THETA_ROT) {
    theta_Rot = theta_Rot_OR_phi;
  } else {
    phi_Cross = theta_Rot_OR_phi;
  }
  TargetRelative_StateParameter::compute_position_from_params(
                                                            input_theta_type);
  jeod::Vector3::copy( position, R_PCPF);
}
/****************************************************************************/
void
TargetRelative_StateParameter::compute_position_from_params(
  InputPosAngle input_theta_type)
{
  if (TargetRelative_StateParam::compute_position_from_params(
                                                           input_theta_type)) {
    /* Compute position vector with magnitude that gives desired (geodetic)
     * altitude from TR-params and direction as just computed.*/
    jeod::Vector3::scale( uhat_R,
                    compute_position_magnitude(),
                    position);
  }
}


/*****************************************************************************
compute_position_magnitude
Purpose:
 (Compute the position magnitude that gives a desired geodetic altitude
 for a given unit position vector.)
*****************************************************************************/
double
TargetRelative_StateParameter::compute_position_magnitude()
{
  //-----------------------------------------------------
  // Iterate to find position magnitude that gives desired geodetic altitude
  //-----------------------------------------------------

  // Initial position magnitude guess for desired altitude
  double Rmag = pfix_position.planet->r_eq + altitude;

  /* Iterate */
  int   iter_count = 0;
  while ( iter_count < iter_limit ) {

    /* Increment iteration counter */
    iter_count++;

    /* Compute the position vector */
    jeod::Vector3::scale( uhat_R, Rmag, pfix_position.cart_coords );

    /* Compute the geodetic altitude */
    pfix_position.update_from_cart(pfix_position.cart_coords);

    /* Check for convergence */
    double deltaR = altitude - pfix_position.ellip_coords.altitude;
    if ( std::abs(deltaR) < tolerance ) {
      break;
    } else {
      Rmag += deltaR;
    }
    if ( iter_count >= iter_limit ) {
      CMLMessage::warn( __FILE__,__LINE__,
        " Geodetic altitude max iteration exceeded!!!\n"
        "Altitude iteration failed to converge with:\n"
        "  target altitude = ",altitude,"\n"
        "  tolerance       = ",tolerance,"\n"
        "  Final deltaR    = ",deltaR,"\n."
        "Continuing with altitude = ",pfix_position.ellip_coords.altitude,"\n"
      );
    }

  }  /* while ( iter_count < iter_limit ) { */

  //-----------------------------------------------------
  // Return position magnitude that gives desired geodetic altitude
  //-----------------------------------------------------
  return(Rmag);
}
