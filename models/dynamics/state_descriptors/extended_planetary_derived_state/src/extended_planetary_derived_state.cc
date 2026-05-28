/*******************************************************************************
PURPOSE:
   (The ExtendedPlanetaryDerivedState extends the PlanetaryDerivedState to
    provide vehicle state with respect to various frames, including
    Topocentric/Topodetic NED, Boost Reference, Plumbline,
    Launch Range, Landing Range, EI Range, Range Safety,
    Pad SEU and left-handed pad frames,
    Hang and Roll angles )

PROGRAMMERS:
   (((Gary Turner) (OSR) (June 2014)
                      (New implementation of PlanetaryDerivedState for Antares))
    ((Bingquan Wang) (OSR) (April 2017) (Fixed the compilation warning of float-point
                       number equality comparison))
    ((Dan Jordan) (Jacobs/JETS) (July 2017)
                   (EM1 V&V cleanup)))
*******************************************************************************/

// NOTES:
// Considered utlizing more JEOD functionality, for examples:
//  - the topocentric and topodetic values are NED frames.  But all that is
//    needed is the transformation matrix; NED gives pfix->NED but we still
//    need inertial ->NED.
//  - Relative velocity could be computed from compute_relative_state(...)
//    without having to go through the (w x r) term, but that also computes the
//    position (already have) and rotational relative state (may not be
//    needed), so it adds to the computation load.  - Gary Turner ~ 2015

#include <cmath>  // acos
#include "jeod/models/environment/planet/include/planet.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "jeod/models/utils/orientation/include/orientation.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/extended_planetary_derived_state.hh"

/*************************************************************************
Method: ExtendedPlanetaryDerivedState
Purpose: Constructor
*************************************************************************/
ExtendedPlanetaryDerivedState::ExtendedPlanetaryDerivedState()
  :
  PlanetaryDerivedState(),
  hang_angle_body_vec{0.0, 0.0, 0.0},
  roll_wrt_hdg_body_vec{0.0, 0.0, 0.0},
  br_ref_longitude(0.0),
  br_ref_geodetic_latitude(0.0),
  pl_ref_longitude(0.0),
  pl_ref_geodetic_latitude(0.0),
  pl_ref_azimuth(0.0),
  launch_range(state),
  landing_range(state, pfix_rel_vel),
  entry_range(state),
  topocentric_altitude(0.0),
  pt_to_pt(state),
  range_safety(),
  topodetic( state.ellip_coords,
             calc_rel_vel),
  topocentric( state.sphere_coords,
               calc_rel_vel),
  inrtl_vel_mag(0.0),
  relative_vel{0.0, 0.0, 0.0},
  relative_vel_mag(0.0),
  pfix_rel_vel{0.0, 0.0, 0.0},
  relative_accel{0.0, 0.0, 0.0},
  T_inrtl_br{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  T_inrtl_pl{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  E_br_body_YPR{0.0, 0.0, 0.0},
  E_pl_body_PYR{0.0, 0.0, 0.0},
  hang_angle(0.0),
  roll_wrt_heading(0.0),
  calc_rel_vel(0),
  calc_topocentric(0),
  calc_topocentric_altitude(0),
  calc_topodetic(0),
  calc_pt_to_pt(0),
  calc_boost_reference(0),
  calc_plumbline(0),
  calc_range_safety(0),
  calc_hang_roll(0),
  boost_ref_initialized(false),
  plumbline_initialized(false),
  range_safety_initialized(false),
  using_launch_range(false),
  using_landing_range(false),
  using_entry_range(false)
{
  subscribe_name = "ExtendedPlanetaryDerivedState:";
}



/*************************************************************************
Method: initialize
Purpose: Initializes the core class models and enabled sub-models.
    This design allows for "partial initialization" of sub-models during
    the call to initialize() and "incremental initialization" of sub-models
    during input and runtime. See the subscribe* methods for reference.
*************************************************************************/
void
ExtendedPlanetaryDerivedState::initialize(
     jeod::DynBody & subject_body,
     jeod::DynManager & dyn_manager)
{
  // finds the planet and initializes the geodetic and geocentric
  // states.
  PlanetaryDerivedState::initialize(subject_body, dyn_manager);

  // Unreachable code.  planet == NULL is already checked, and is a temination
  // fault, in JEOD as part of PlanetaryDerivedState::initialize(...)
  // However, that is an external dependency, and if that were to change in
  // a future release, this would be necessary.
  if (planet == NULL) {
    CMLMessage::fail(
    __FILE__,__LINE__, "Planetary Derived State initialization failure\n",
    "Model failed to find the specified planet.\n"
    "This error should have been caught in DerivedState::find_planet();\n"
    "this should never be seen.\n");
  }

  // These methods verify that all pointers used within the TopoContainer and
  // VelocitySet classes are non-NULL.
  // The methods will cause a general fault if they are not.
  // planet and subject are set in in PlanetaryDerivedState so cannot be NULL.
  topodetic.initialize(planet->pfix.state.rot,
         subject->composite_body.state.rot,
         subject->composite_body.state.trans.velocity,  // Velocity expressed in inrtl
         relative_vel);                                 // Planet-relative velocity (expressed in inrtl)
  topocentric.initialize(planet->pfix.state.rot,
         subject->composite_body.state.rot,
         subject->composite_body.state.trans.velocity, // Velocity expressed in inrtl
         relative_vel);                                // Planet-relative velocity (expressed in inrtl)

  pt_to_pt.initialize( subject_body,
                       planet->pfix.state.rot);

  if (calc_boost_reference > 0) {
    init_boost_reference();
  }

  if (calc_plumbline > 0) {
    init_plumbline();
  }

  if (using_launch_range || calc_range_safety>0) {
    launch_range.initialize(*planet);
  }
  if (using_landing_range) {
    landing_range.initialize(*planet);
  }
  if (using_entry_range) {
    entry_range.initialize(*planet);
  }
  if (calc_range_safety > 0) {
    init_range_safety();
  }


  SubscriptionBase::initialize();
  // If subscription to this model is pending initialization, update() will
  // be called automatically.
  // If subscription is not pending initialization, then model has not been
  // subscribed, and there is no need to call update().
  // Either way, do not call update() from here.
  return;
}

/*************************************************************************
Method: update
Purpose: This function takes state information [position and velocity] and
         computes planet relative trajectory parameters and coordinate
         transformations required by other environment models.)
REFERENCE:
    (((SES II) (planet_relative_state function) (position_att.c))
     ((SES) (Parameters of Motion function) (POM.f))
     ((SORT) (Where At function) (WHEREAT.f)))
PROGRAMMERS:
    (((Gavin Mendeck) (NASA) (May 2005) (ARES) (Initial implementation as pom_pos))
     ((Edgar Medina) (NASA) (June 2005) (ARES) (Changed name and updated function))
     ((Kurt McCall)  (NASA) (June 2006) (ARES) (NED-to-body Euler angles))
     ((Ryan Whitley) (NASA) (July 2006) (ARES) (Add range calculation to some reference))
     ((Robert Gay) (NASA-JSC) (Dec 2006) (ANTARES) (new dprange_ls_rad))
     ((Robert Gay) (NASA-JSC) (Mar 2007) (ANTARES) (fixed T_inrtl_tc & omega_cross_r - for full RNP))
     ((Robert Gay) (NASA-JSC) (May 2007) (ANTARES) (added pfix_rel_vel))
     ((Robert Gay) (NASA-JSC) (May 2007) (ANTARES) (added misc. angles))
     ((Scott Nemeth) (USA-FDD) (Aug 2007) (ANTARES) (RDLaa10040: Entry range calcs))
     ((James Thaxton) (USA) (April 2008) (ANTARES) (Added acceleration calculations and
                                                    modified relative velocity calc))
     ((Carl Merry) (USA-FDD) (July 2008) (ANTARES) (Geodetic calcs))
     ((Pat Galvin) (ESCG) (Dec 2008) (AGDL00001242) (add geocentric height calc for use in GRAM))
     ((Luke McNamara)  (NASA-JSC) (Mar 2009)  (ANTARES)  (AGDL00002451))
     ((Carl Merry) (USA-FDD) (2010) (ANTARES)  (Accurate geodetic calcs
                                                Acceleration calculations
                                                Range safety calculations
                                                Reorg to group like calculations))
     ((Jeremy Rea) (NASA) (Feb 2011) (ANTARES) (AGDL200000512: Add capability to output J2000
                                                               independent of sim inertial frame))
     ((Jeremy Rea) (NASA) (April 2011) (ANTARES) (AGDL200000557: Incorrect implementation of topocentric
                                                                 coordinate system))
     ((Jeremy Rea) (NASA) (September 2011) (ANTARES) (AGDL200000720: Range computation relative to target position and heading))
     ((Jeremy Rea) (NASA) (September 2011) (ANTARES) (AGDL200000727: Make planet relative parameter computation robust to divide by zero))
     ((Thomas Phung) (ESCG) (April 2012) (ANTARES) (AGDL200000826: Handle euler angles round off errors
                                                                                when aligning vehicle on launch pad))
    )
*******************************************************************************/

void
ExtendedPlanetaryDerivedState::update()
{

  if (!active) {
    return;
  }

  // Compute the cartesian (xyz), geocentric (sphere_coords), and geodetic
  // (ellip_coords) position state.
  PlanetaryDerivedState::update();
  inrtl_vel_mag = jeod::Vector3::vmag(subject->composite_body.state.trans.velocity);

  if (calc_pt_to_pt>0) {
    pt_to_pt.update();
  }

  if (calc_topocentric_altitude>0) {
    calculate_topocentric_altitude();
  }

  if (calc_rel_vel>0      || calc_hang_roll>0 ||
      calc_range_safety>0 || landing_range.is_active()) {
    calculate_relative_vel();
  }

  if (calc_topocentric>0) {
    topocentric.update();
  }

  if ( calc_topodetic>0 || calc_hang_roll>0) {
    topodetic.update();
  }

  if (calc_hang_roll>0) {
    if (!calc_rel_vel)
    {
      //if rel_vel has not been subscribed, then explicitly update it.
      topodetic.relative_vel.update();
    }
    hang_roll();
  }

  if (launch_range.is_active()) {
    launch_range.update();
  }

  if (landing_range.is_active()) {
    landing_range.update();
  }

  if (entry_range.is_active()) {
    entry_range.update();
  }

  if (calc_range_safety > 0) {
    analyze_range_safety();
  }


  /* TODO  Gary Turner Sept 2014 - Revised by Dan Jordan 2017
   *  It appears that T_br_body, E_br_body_YPR are not necessarily "needed"
   *  desired outputs.  They are not used by other models, and only data recorded.
   *  Compute yaw-pitch-roll Euler angles from boost reference to body
   */
  if (calc_boost_reference>0) {
      double T_br_body[3][3];
      jeod::Matrix3x3::product_right_transpose(
                       subject->composite_body.state.rot.T_parent_this,
                       T_inrtl_br,
                       T_br_body);
      jeod::Orientation::compute_euler_angles_from_matrix(
                       T_br_body,
                       jeod::Orientation::Yaw_Pitch_Roll,
                       E_br_body_YPR);
  }

  /* TODO  Gary Turner Sept 2014 - Revised by Dan Jordan 2017
   *  It appears that T_pl_body, E_pl_body_PYR are not necessarily "needed"
   *  desired outputs.  They are not used by other models, and only data recorded.
   *  Compute pitch-yaw-roll Euler angles from plumbline to body
   */
  if (calc_plumbline>0) {
      double T_pl_body[3][3];
      jeod::Matrix3x3::product_right_transpose(
                       subject->composite_body.state.rot.T_parent_this,
                       T_inrtl_pl,
                       T_pl_body);
      jeod::Orientation::compute_euler_angles_from_matrix(
                       T_pl_body,
                       jeod::Orientation::Pitch_Yaw_Roll,
                       E_pl_body_PYR);
  }
}

/*****************************************************************************
subscribe_*
Purpose: Adds the various aspects of the model into the execution path if the
    model has already been initialized.  If the model has not already been
    initialized, just increments the respective sub-model subscription count
    so that model-initialization will initialize the respective sub-models.
    Some sub-models do not require initialization, some submodels will check
    that input conditions are sufficient to turn on the model, rejecting
    the subscription if they are not met.
    This design allows for "partial initialization" of sub-models during
    the call to initialize() and "incremental initialization" of sub-models
    during input and runtime.
*****************************************************************************/
void ExtendedPlanetaryDerivedState::subscribe_hang_roll()
{
  bool subscribe = true;
  // Error  if hang / roll vectors are near zero in magnitude, they must be
  // defined to enable the sub-model
  if (jeod::Vector3::vmag(hang_angle_body_vec) < std::numeric_limits<double>::min() ) {
      CMLMessage::error (
        __FILE__, __LINE__, "hang_angle_body_vec magnitude near zero!\n",
        "A zero body vector will result in incorrect hang angle calculation.\n"
        "Refusing to subscribe to hang/roll, try setting hang_angle_body_vec.\n");
      subscribe = false;
  }
  if (jeod::Vector3::vmag(roll_wrt_hdg_body_vec) < std::numeric_limits<double>::min() ) {
      CMLMessage::error (
        __FILE__, __LINE__, "roll_wrt_hdg_body_vec magnitude near zero!\n",
        "A zero body vector will result in incorrect roll angle calculation.\n"
        "Refusing to subscribe to hang/roll, try setting roll_wrt_hdg_body_vec.\n");
      subscribe = false;
  }
  if (subscribe) {
      calc_hang_roll++;
  }
}
void
ExtendedPlanetaryDerivedState::subscribe_boost_reference()
{
  if (initialized && !boost_ref_initialized) {
    init_boost_reference();
  }
  calc_boost_reference++;
}

void
ExtendedPlanetaryDerivedState::subscribe_plumbline()
{
  if (initialized && !plumbline_initialized) {
    init_plumbline();
  }
  calc_plumbline++;
}

void
ExtendedPlanetaryDerivedState::subscribe_launch_range()
{
  if (initialized && !launch_range.is_initialized()) {
    // NOTE - initialized => planet != NULL
    launch_range.initialize(*planet);
  }
  using_launch_range = true;
  launch_range.subscribe();
}

void
ExtendedPlanetaryDerivedState::subscribe_landing_range()
{
  if (initialized && !landing_range.is_initialized()) {
    // NOTE - initialized => planet != NULL
    landing_range.initialize(*planet);
  }
  using_landing_range = true;
  landing_range.subscribe();
}

void
ExtendedPlanetaryDerivedState::subscribe_entry_range()
{
  if (initialized && !entry_range.is_initialized()) {
    // NOTE - initialized => planet != NULL
    entry_range.initialize(*planet);
  }
  using_entry_range = true;
  entry_range.subscribe();
}

void
ExtendedPlanetaryDerivedState::subscribe_range_safety()
{
  if (initialized) {
    // NOTE - initialized => planet != NULL
    if (!launch_range.is_initialized()) {
      launch_range.initialize(*planet);
    }
    if (!range_safety_initialized) {
      init_range_safety();
    }
  }
  calc_range_safety++;
}


/*****************************************************************************
init_boost_reference
Purpose: Compute the transformation from inertial to boost reference.
         This is computed in the init function because the boost frame
         is inertial, so the transformation is constant.
*****************************************************************************/
void
ExtendedPlanetaryDerivedState::init_boost_reference()
{
  if (boost_ref_initialized) {
    return;
  }

  double T_ef_br[3][3];
  double E_ef_br_YPR[3] = { br_ref_longitude,
                            -(br_ref_geodetic_latitude + M_PI_2),
                            0.0 };

  jeod::Orientation::compute_matrix_from_euler_angles(jeod::Orientation::Yaw_Pitch_Roll,
                                                      E_ef_br_YPR,
                                                      T_ef_br);
  // Note - The planet pfix state was updated at initialization with
  // priority P_ENV (P30) in the Earth sim object.
  // This method is run with at-best priority P_DYN (P50), so comes AFTER
  // the RNP update.  Hence, pfix state should be current.
  jeod::Matrix3x3::product( T_ef_br,
                      planet->pfix.state.rot.T_parent_this,
                      T_inrtl_br);
  boost_ref_initialized = true;
  return;
}

/*****************************************************************************
init_plumbline
Purpose: Compute the transformation from inertial to plumbline.
         This is computed in the init function because the plumbline frame
         is inertial, so the transformation is constant.
*****************************************************************************/
void
ExtendedPlanetaryDerivedState::init_plumbline()
{
  if (plumbline_initialized) {
    return;
  }

  double T_ef_pl[3][3];
  double E_ef_pl_YPR[3] = {  pl_ref_longitude,
                            -pl_ref_geodetic_latitude,
                            -pl_ref_azimuth
                          };

  jeod::Orientation::compute_matrix_from_euler_angles(jeod::Orientation::Yaw_Pitch_Roll,
                                                E_ef_pl_YPR,
                                                T_ef_pl);
  // Note - The planet pfix state was updated at initialization with
  // priority P_ENV (P30) in the Earth sim object.
  // This method is run with at-best priority P_DYN (P50), so comes AFTER
  // the RNP update.  Hence, pfix state should be current.
  jeod::Matrix3x3::product( T_ef_pl,
                      planet->pfix.state.rot.T_parent_this,
                      T_inrtl_pl);
  plumbline_initialized = true;
  return;
}

/*************************************************************************
Method: init_range_safety
Reference: CML method range_safety_calc.c
Purpose: Initializes values for the range-safety calculation.
*************************************************************************/
void
ExtendedPlanetaryDerivedState::init_range_safety()
{
/* ====================================================================
  Compute Planet Fixed to Pad transformation matrix for RSTAPE generation
  The Pad frame is a left handed frame centered at the launch pad,
  where the Z axis points zenith, the X axis points along the launch
  azimuth and the Y axis points in the azimuth plus 90 degrees direction.
  For vertical-plane representations we use a South-East-Up frame co-located
  with the pad frame.
 ====================================================================*/

  double temp_euler[3];
  // Create the pfix->SEU-frame transformation
  // TODO: Why not use ellip_coords.longitude as well here? We take data
  // "across containers" which might be confusing, they should be the same
  // - Dan Jordan, July 2017
  temp_euler[0] = launch_range.reference_data.sphere_coords.longitude;
  temp_euler[1] = M_PI_2 - launch_range.reference_data.ellip_coords.latitude;
  temp_euler[2] = 0.0;
  jeod::Orientation::compute_matrix_from_euler_angles(
                                  jeod::Orientation::EulerZYX,
                                  temp_euler,
                                  range_safety.T_pfix_pad_SEU);

  // Create the pfix->pad frame transformation.  Start with the SEU frame
  //   and add a rotation so that the y-axis lies along the direction vector
  //   (i.e. North + azimuth)
  temp_euler[0] = M_PI_2 - launch_range.reference_data.azimuth;
  temp_euler[1] = 0.0;
  temp_euler[2] = 0.0;
  double scratch[3][3];
  jeod::Orientation::compute_matrix_from_euler_angles(
                                  jeod::Orientation::EulerZYX,
                                  temp_euler,
                                  scratch);
  // scratch looks like [[a b 0][-b a 0][0 0 1]] because it is z-axis
  //   rotation only
  // desired frame, pad, is left-handed with x along azimuth, y at az+pi/2
  // Conceptualize a temporary frame, pad', which is right-handed with
  //   y along azimuth, x at az+pi/2
  // so pad' -> pad is [[0 1 0][1 0 0][0 0 1]]
  // pfix->pad' = scratch * T_pfix_pad_SEU
  // Thus,
  // pfix->pad  = [[0 1 0][1 0 0][0 0 1]] * scratch * T_pfix_pad_SEU
  //            = [[-b a 0][a b 0][0 0 1]] * T_pfix_pad_SEU
  scratch[0][0] = scratch[1][0];
  scratch[0][1] = scratch[1][1];
  scratch[1][0] = scratch[1][1];
  scratch[1][1] = -scratch[0][0];

  jeod::Matrix3x3::product(scratch,
                     range_safety.T_pfix_pad_SEU,
                     range_safety.T_pfix_pad);
  range_safety_initialized = true;
  return;
}


/*****************************************************************************
calculate_topocentric_altitude
Purpose:(calculates the radial disstance between the current position and the
         intersection of the radial line with the ellipsoid.  This is distinct
         from the spherical_coords.altitude - distance to the reference sphere -
         and the ellip_coords.altitude - distance between position and ellipsoid
         measured perpendicular to the ellipsoid.)
******************************************************************************/
void
ExtendedPlanetaryDerivedState::calculate_topocentric_altitude()
{
  double pos_mag_sq = jeod::Vector3::vmagsq(state.cart_coords);
  double pos_mag = std::sqrt(pos_mag_sq);
  double eq_pos_sq = state.cart_coords[0] * state.cart_coords[0] +
                     state.cart_coords[1] * state.cart_coords[1];
  topocentric_altitude = pos_mag * (1 - planet->r_pol /
                          std::sqrt (pos_mag_sq - planet->e_ellip_sq *eq_pos_sq));
  return;
}

/*************************************************************************
Method:  calculate_relative_vel
Purpose:  Computes the relative velocity
*************************************************************************/
void
ExtendedPlanetaryDerivedState::calculate_relative_vel()
{
  // Planet rotation vector:
  double pl_rot_vec_inrtl[3]; /* r/s Rotation of planet in inrtl frame */
  jeod::Vector3::transform_transpose(planet->pfix.state.rot.T_parent_this,
                               planet->pfix.state.rot.ang_vel_this,
                               pl_rot_vec_inrtl);

  // velocity vector of co-rotating point at this position
  double omega_cross_r[3];
  jeod::Vector3::cross( pl_rot_vec_inrtl,
                  subject->composite_body.state.trans.position,
                  omega_cross_r);

  // Vehicle velocity relative to co-rotating point (inertial)
  jeod::Vector3::diff( subject->composite_body.state.trans.velocity,
                 omega_cross_r,
                 relative_vel);

  relative_vel_mag = jeod::Vector3::vmag(relative_vel);

  // Vehicle velocity relative to co-rotating point (pfix)
  // NOTE - needed by range_safety_calculation and ground_resist model.
  jeod::Vector3::transform(planet->pfix.state.rot.T_parent_this,
                     relative_vel,
                     pfix_rel_vel);
  return;
}

/*************************************************************************
Method: hang_roll
Purpose: Computes the hang angle and the roll angle wrt heading
*************************************************************************/
void
ExtendedPlanetaryDerivedState::hang_roll()
{

  double hang_angle_body_vec_td[3];
  // Transform hang_angle_body_vector from the body frame to the topodetic NED frame
  jeod::Vector3::transform_transpose( topodetic.T_this_to_body,
                                hang_angle_body_vec,
                                hang_angle_body_vec_td);
  double  mag_hang_angle_vec = jeod::Vector3::vmag(hang_angle_body_vec_td);

  // The hang angle is the angle between the hang_angle_body_vector in the NED frame
  // and down, which is the 3rd index [2]
  hang_angle = MathUtils::acos_protected(
                         MathUtils::divide_protected(hang_angle_body_vec_td[2],
                                                     mag_hang_angle_vec,
                                                     1.0));

  if( topodetic.relative_vel.vel_xy > 1.0e-9 ){

    double scratch[3] = {topodetic.relative_vel.velocity[0],
                         topodetic.relative_vel.velocity[1],
                         0.0};
    double rel_td_vel_xy_unit_vec[3];
    jeod::Vector3::normalize(scratch, rel_td_vel_xy_unit_vec);

    jeod::Vector3::transform_transpose( topodetic.T_this_to_body,
                                  roll_wrt_hdg_body_vec,
                                  scratch);
    scratch[2] = 0.0;
    double roll_wrt_hdg_body_unit_vec_td[3];
    jeod::Vector3::normalize(scratch, roll_wrt_hdg_body_unit_vec_td);

    roll_wrt_heading =
               MathUtils::acos_protected(jeod::Vector3::dot(rel_td_vel_xy_unit_vec,
                                               roll_wrt_hdg_body_unit_vec_td));

    jeod::Vector3::cross(roll_wrt_hdg_body_unit_vec_td,
                   rel_td_vel_xy_unit_vec,
                   scratch);
    if( scratch[2] < 0.0 ){
        roll_wrt_heading *= -1.0;
    }
  }
  else {
    roll_wrt_heading = 0.0;
  }
  return;
}


/*************************************************************************
Method: analyze_range_safety
Reference: CML method range_safety_calc.c
Purpose: Makes the range-safety call.
*************************************************************************/
void
ExtendedPlanetaryDerivedState::analyze_range_safety()
{

  double relative_pos_pfix[3];

  // Compute the vector difference of planet-fixed vehicle position
  // and the planet-fixed launch range position
  jeod::Vector3::diff( state.cart_coords,
                 launch_range.reference_data.cart_coords,
                 relative_pos_pfix);
  // slant range (distance) is the magnitude of this difference vector
  range_safety.slant_range = jeod::Vector3::vmag(relative_pos_pfix);

  // Transform the relative position into the left-handed pfix pad frame
  jeod::Vector3::transform(range_safety.T_pfix_pad,
                     relative_pos_pfix,
                     range_safety.XPad);

  // Transform the relative position into the right-handed pfix pad SEU frame
  jeod::Vector3::transform(range_safety.T_pfix_pad_SEU,
                     relative_pos_pfix,
                     range_safety.XVP);

  // Transform the relative velocity into the pad frame (relative vel already computed)
  jeod::Vector3::transform(range_safety.T_pfix_pad,
                     pfix_rel_vel,
                     range_safety.VPad);

  // Repeat for acceleration:
  calculate_relative_accel();

  jeod::Vector3::transform(range_safety.T_pfix_pad,
                     relative_accel,
                     range_safety.NPad);


  //======================================================================
  // Compute vertical plane parameters
  //======================================================================

  // At this point range_safety.XVP is the vehicle position relative to launch_range.reference_data
  // expressed in the right-handed SEU pad frame

  // Take the dot product of a pure north vector and the projection of XVP on the horizontal
  // plane in order to get angle between them
  double scratch[3] = {range_safety.XVP[0],
                         range_safety.XVP[1],
                         0.0};
  double scratch_xy_norm[3] =  {0.0, 0.0, 0.0};

  jeod::Vector3::normalize(scratch, scratch_xy_norm);
  double scratch_north[3] = {-1.0,
                              0.0,
                              0.0};
  range_safety.pad_azimuth = MathUtils::acos_protected(jeod::Vector3::dot(scratch_xy_norm,
                                               scratch_north));
  // Determine "direction" of dot product to ensure range_safety.pad_azimuth is between
  // 0 and 2*PI measured from north
  jeod::Vector3::cross(scratch_xy_norm,
                 scratch_north,
                 scratch);
  if( scratch[2] < 0.0 ){
      range_safety.pad_azimuth = 2.0*M_PI - range_safety.pad_azimuth;
  }

  double inplane_mag = std::sqrt((range_safety.XVP[0] *
                                  range_safety.XVP[0]) +
                                 (range_safety.XVP[1] *
                                  range_safety.XVP[1]));

  // Compute the distance (components) along the XVRT, YVRT, ZVRT axes
  range_safety.XVRT = inplane_mag *
              std::cos(range_safety.XVRT_Azi - range_safety.pad_azimuth);
  range_safety.YVRT = inplane_mag *
               std::cos(range_safety.pad_azimuth - range_safety.YVRT_Azi);
  range_safety.ZVRT = range_safety.XVP[2];

  return;
}

/*************************************************************************
Method: calculate_relative_accel
Purpose: Computes the planet-relative acceleration
*************************************************************************/
void
ExtendedPlanetaryDerivedState::calculate_relative_accel()
{
  // Notation : a_{<wrt>:<expressed in>}
  // want accel relative to pfix expressed in pfix    a_{p:p}
  // know accel relative to inrtl expressed in inrtl  a_{I:I}
  // need intermediat accel relative to inrtl expressed in pfix   a_{I:p} :
  // a_{I:p} = a_{p:p} + (w_{p|I:p} x (w_{p|I:p} x r_{p})) +
  //                     2 w_{p|I:p} x v_{p:p}
  // a_{I:p} = T_{I->p} a_{I:I)

  // accel wrt inertial expressed in pfix: a_{I:p} = T_{I->p} a_{I:I)
  jeod::Vector3::transform( planet->pfix.state.rot.T_parent_this,
                      subject->derivs.trans_accel,
                      relative_accel);

  // (w_{p|I:p} x r_{p})
  double omega_cross_r[3];
  jeod::Vector3::cross(planet->pfix.state.rot.ang_vel_this,
                 state.cart_coords,
                 omega_cross_r);
  // (w_{p|I:p} x (w_{p|I:p} x r_{p}))
  double omega_cross_omega_cross_r[3];
  jeod::Vector3::cross(planet->pfix.state.rot.ang_vel_this,
                 omega_cross_r,
                 omega_cross_omega_cross_r);

  // 2 w_{p|I:p} x v_{p:p}
  double two_omega_cross_v[3];
  jeod::Vector3::cross(planet->pfix.state.rot.ang_vel_this,
                 pfix_rel_vel,
                 two_omega_cross_v);
  jeod::Vector3::scale(2.0, two_omega_cross_v);

  // a_{p:p} = relative_accel - omega_cross_omega_cross_r - two_omega_cross_v
  jeod::Vector3::decr(omega_cross_omega_cross_r, relative_accel);
  jeod::Vector3::decr(two_omega_cross_v, relative_accel);

  return;
}
