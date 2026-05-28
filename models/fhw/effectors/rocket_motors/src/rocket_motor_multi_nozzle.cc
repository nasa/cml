/*******************************************************************************
PURPOSE:
  (Provide an enhanced solid-motor model with multiple nozzles and
     optional flex effects..)

PROGRAMMERS:
  (((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares)
         (initial, providing features dictated by requirements for
         CEV-LAS motors))
   ((Daniel Ghan) (OSR) (Jun 2020) (Antares) (Simplified nozzles, changed flex
          rotation calculation)))

*******************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/quaternion/include/quat.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/rocket_motor_multi_nozzle.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
RocketMotor_MultiNozzle::RocketMotor_MultiNozzle(
    DynamicMassGroup                   & mass_group,
    DynamicMassBody                    * mass_body,
    DynamicMassString                  * mass_string,
    DynamicMassBodyPropertiesInterface & mass_properties,
    const double                       & time,
    const double                       * veh_cm,
    bool                                 use_mass_string,
    const double                       & atm_press)
  :
  RocketMotor_TableThrust( mass_group,
                           mass_body,
                           mass_string,
                           mass_properties,
                           time,
                           veh_cm,
                           use_mass_string),
  atmos_pressure(atm_press),
  motor_lin_flex(nullptr),
  motor_rot_flex(nullptr),
  table_is_net_thrust(false),
  compute_cosine_losses(false),
  atm_press_adjust(false),
  cosine_loss_scale_factor(1.0),
  net_roll_torq(0.0),
  thrust_vac{0.0, 0.0, 0.0},
  thrust_vac_mag(0.0),
  using_flex(false),
  flex_threshold(1.0E-12),
  num_flex_elements(0),
  num_noz(0)
{
  nozzles_ptr_vec.clear();

  // thrust_unit_motor is not used for a multi-nozzle configuration because the
  // direction is determined by the nozzle directions.
  // So set it to an arbitrary unit vector so its absence does not trigger any
  // fake errors in lower-level classes.
  jeod::Vector3::unit(0, thrust_unit_motor);
}
/****************************************************************************/
RocketMotor_MultiNozzle::RocketMotor_MultiNozzle(
    DynamicMassBody & mass,
    const double    & time,
    const double    & atm_press,
    const double    * veh_cm)
  :
  RocketMotor_MultiNozzle( mass_group_internal,
                           &mass,
                           nullptr,
                           mass.dynamic_properties,
                           time,
                           veh_cm,
                           false,
                           atm_press)
{}
/****************************************************************************/
RocketMotor_MultiNozzle::RocketMotor_MultiNozzle(
    DynamicMassGroup & mass_group,
    DynamicMassBody  & mass,
    const double     & time,
    const double     & atm_press,
    const double     * veh_cm)
  :
  RocketMotor_MultiNozzle( mass_group,
                           &mass,
                           nullptr,
                           mass.dynamic_properties,
                           time,
                           veh_cm,
                           false,
                           atm_press)
{}
/****************************************************************************/
RocketMotor_MultiNozzle::RocketMotor_MultiNozzle(
    DynamicMassString & string,
    const double      & time,
    const double      & atm_press,
    const double      * veh_cm)
  :
  RocketMotor_MultiNozzle( mass_group_internal,
                           nullptr,
                           &string,
                           string,
                           time,
                           veh_cm,
                           true,
                           atm_press)
{}
/****************************************************************************/
RocketMotor_MultiNozzle::RocketMotor_MultiNozzle(
    DynamicMassGroup  & mass_group,
    DynamicMassString & string,
    const double      & time,
    const double      & atm_press,
    const double      * veh_cm)
  :
  RocketMotor_MultiNozzle( mass_group,
                           nullptr,
                           &string,
                           string,
                           time,
                           veh_cm,
                           true,
                           atm_press)
{}

/*****************************************************************************
add_nozzle
Purpose:(Adds a nozzle to the nozzles_ptr_vec vector)
*****************************************************************************/
void
RocketMotor_MultiNozzle::add_nozzle(
    RocketMotorNozzle &nozzle)
{
  for (size_t ii = 0; ii < nozzles_ptr_vec.size(); ++ii) {
    if (&nozzle == nozzles_ptr_vec[ii]) {
      CMLMessage::error(
        __FILE__,__LINE__,"Configuration error\n",
        "Instruction received to add a nozzle that is already in the vector\n"
        "of nozzles.\nInstruction ignored.\n");
      return;
    }
  }
  nozzles_ptr_vec.push_back( &nozzle);
  num_noz = nozzles_ptr_vec.size();
}

/*****************************************************************************
initialize
Purpose:(Initializes the model)
Reference: was init_las_solid_motor2
*****************************************************************************/
void
RocketMotor_MultiNozzle::initialize(
          size_t         num_flex_elements_in,
          const double * motor_lin_flex_in,
          const double * motor_rot_flex_in)
{
  if (initialized) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid Initialization\n",
      "This is a solid rocket motor and shouldn't be re-initialized, \n"
      "if for some reason you need to initialize the model again you should \n"
      "use the force_initialize() method.\n");
    return;
  }

  num_flex_elements = num_flex_elements_in;
  if (using_flex) {
    if (motor_lin_flex_in == nullptr) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid construction\n",
        "The motor linear displacement flex pointer has not been set, \n"
        "and is required for generation of the flex moment. \n");
    }

    if (motor_rot_flex_in == nullptr) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid initialization\n",
        "The motor rotational displacement flex pointer has not been set, \n"
        "and is required for generation of the motor's force and moment.\n");
    }

    if (num_flex_elements != 3* num_noz) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid construction \n",
        "For a system with ", num_noz, " nozzles, the flex structure must be sized with\n",
         num_noz * 3 , " elements.  It has ", num_flex_elements, " elements.\n");
    }
  }
  motor_lin_flex = motor_lin_flex_in;
  motor_rot_flex = motor_rot_flex_in;

  if (num_noz == 0) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid construction \n",
     "The number of nozzles on a rocket motor cannot be = 0, ensure the \n"
     "number of nozzles for the rocket motor was set to a value > 0. \n");
  }
  // Call parent class initialize method.  This set the initialized flag; reset
  // the flag if there are any problems later in initialization with the
  // nozzles.
  RocketMotor_TableThrust::initialize();

  initialize_nozzles();
}

/*****************************************************************************
force_initialize
Purpose:(Allow for the ability to re-initialize the rocket motors if needed)
*****************************************************************************/
void
RocketMotor_MultiNozzle::force_initialize(size_t num_flex_elements,
                                          const double * motor_lin_flex_in,
                                          const double * motor_rot_flex_in)
{
  initialized = false;
  initialize(num_flex_elements,
             motor_lin_flex_in,
             motor_rot_flex_in);
}

/*****************************************************************************
initialize_nozzles
Purpose:(Initializes the rocket motor nozzles)
*****************************************************************************/
void
RocketMotor_MultiNozzle::initialize_nozzles()
{
  // Initialize the nozzles and sum the nominal and dispersed scale factors
  double total_scale_factor = 0.0, total_dispersed_scale_factor = 0.0;
  for (unsigned int ii = 0; ii < num_noz; ii++) {
    nozzles_ptr_vec[ii]->initialize( position,
                                     T_struc_to_motor_frame);
    total_scale_factor += nozzles_ptr_vec[ii]->sf;
    total_dispersed_scale_factor += nozzles_ptr_vec[ii]->sf_true;
  }

  if (table_is_net_thrust) {
    // Tabulated thrust_magnitude includes cosine losses so it represents the
    // magnitude of the vector sum of the nominal nozzle thrusts.  We need
    // the sum of the magnitudes of the nominal nozzle thrusts to evaluate
    // mass-flow, isp, and thrust-per-scale-factor.
    // The scaling necessary for converting from net-thrust values to raw-thrust
    // values is either provided (cosine_loss_scale_factor) or can be computed
    // from the orientation and scale-factor of the combined nozzles.
    if (compute_cosine_losses) {
      // Protect against invalid total_scale_factor:
      if (total_scale_factor <= 0.0) {
        CMLMessage::error(
         __FILE__,__LINE__,"Invalid scale-factor values.\n",
         "The provided scale-factors sum to something not greater than 0.0.\n"
         "Cannot compute cosine losses with this configuration.\n"
         "Continuing with user-provided cosine_loss_scale_factor.\n");
      }
      else {
        // The values specified in the nozzle are the nominal values, so execute
        // the computation of the scaled thrust for each nozzle.

        double sf_thrust_dir[3], accum_sf_thrust_dir[3] = {0.0, 0.0, 0.0};
        // Accumulate the vector-sum and the scalar-sum
        for (unsigned int ii = 0; ii < num_noz; ii++) {
          jeod::Vector3::scale( nozzles_ptr_vec[ii]->nominal_thrust_dir,
                          nozzles_ptr_vec[ii]->sf,
                          sf_thrust_dir);
          jeod::Vector3::incr( sf_thrust_dir, accum_sf_thrust_dir);
        }

        // cosine-loss is the ratio between the magnitude of the vector sum and
        // the sum of the magnitudes.
        // Set failed-divide to 1.1 to generate later error.  However, this
        // should be impossible because total_scale_factor==0.0 has already
        // been blocked.
        cosine_loss_scale_factor = MathUtils::divide_protected(
                                             jeod::Vector3::vmag(accum_sf_thrust_dir),
                                             total_scale_factor,
                                             1.1);
      }
    }
    if (cosine_loss_scale_factor > 1.0) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid cosine-loss value.\n",
        "The cosine-loss scale factor, ", cosine_loss_scale_factor, " > 1.0, which suggests that cosine\n"
        "losses would become cosine gains.\n"
        "Cannot convert from net-thrust values to raw-thrust values.\n"
        "Resetting the table_is_net_thrust flag to False; table values will\n"
        "be interpreted as being raw thrust.\n");
      table_is_net_thrust = false;
      cosine_loss_scale_factor = 1.0;
    }
    else if (cosine_loss_scale_factor <= 0.0) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid cosine-loss value.\n",
        "The cosine-loss scale factor, ", cosine_loss_scale_factor, " <= 0.0.\n"
        "It must be > 0 in order to convert net-thrust to raw-thrust.\n"
        "Resetting the table_is_net_thrust flag to False; table values will\n"
        "be interpreted as being raw thrust.\n");
      table_is_net_thrust = false;
      cosine_loss_scale_factor = 1.0;
    }
  }

  // Working under the assumption that total flow-rate through the nozzles is
  // somewhat fixed, recognize that where one nozzle's scale-factor increases
  // that must come at the expense of another nozzle's scale factor.  The sum
  // over all scale-factors is therefore constrained to be constant during the
  // application of dispersions and must average to 1.0.
  // Scale the scale-factors accordingly and recompute the scaled
  // thrust-direction vector.

  // Protect against invalid total_scale_factor:
  double sf_scale = 0.0;
  if (total_dispersed_scale_factor <= 0.0) {
    CMLMessage::error(
     __FILE__,__LINE__,"Invalid scale-factor values.\n",
     "The provided scale-factors sum to something not greater than 0.0.\n"
     "Zeroing the scale-factor on all nozzles.\n");
  }
  else {
    sf_scale = MathUtils::divide_protected( num_noz,
                                            total_dispersed_scale_factor,
                                            0.0);
  }
  for (unsigned int ii = 0; ii < num_noz; ii++) {
    nozzles_ptr_vec[ii]->sf_true *= sf_scale;
  }
  // The total scale-factor should now sum to be numerically equal to the number
  // of nozzles because the individual scale-factors have been so scaled.
}

/**************************************************************************
update
Purpose: (Main executable.
          Compute the force/torque resulting from firing the motor)
**************************************************************************/
void
RocketMotor_MultiNozzle::update()
{
  if (!active) {
    return;
  }
  if (!update_status()) {
    return;
  }
  update_table();
  // TODO  - Turner 2018/05
  //       Should the code return here if the update_table() method shuts the
  //       motor off?
  //       See rocket_motor_table_thrust.cc for discussion

  // If cosine loss is included in profile, need to obtain pre-loss profile
  // for computing mass flow rates.  This code can estimate cosine losses at
  // initialization, or can use a user-supplied value; either process produces
  // a value for cosine_loss_scale_factor, which is used as:
  //     net-thrust = cosine_loss_scale_factor * raw-thrust
  // cosine_loss_scale_factor has value in (0.0, 1.0], where 0.0 represents
  // total loss and 1.0 represents no loss.
  // If the profile represents the raw thrust, that can be passed through to the
  // mass-flow calculations without additional modification.
  if (table_is_net_thrust) {
    thrust_magnitude = MathUtils::divide_protected( thrust_magnitude,
                                                    cosine_loss_scale_factor,
                                                    thrust_magnitude);
  }
  // thrust_magnitude is now the scalar sum of the nozzle thrust magnitudes.

  // compute mass-flow-rate and specific-impulse before applying cosine-loss.
  compute_flow_rate_and_isp();

  // compute the mass consumed for this tank
  update_mass_consumption();
  // TODO  - Turner 2018/05
  //       Should the code return here if the update_mass_consumption()
  //       method shuts the motor off?
  //       See rocket_motor_table_thrust.cc for discussion
  // update_mass_consumption can trigger the status to Finished when its
  // propellant has expired.  So check the status flag before proceeding to the
  // thrust assignment.
  //if (status == Finished) {
  //  return;
  //}

  // compute the thrust on each nozzle; accumulate each nozzle to provide the
  // total system values.  The sum over all scale-factors has already been
  // checked and enforced to be equal to the number of nozzles.
  // NOTE - num_noz != 0 verified at initialization, resulting in termination if
  // it failed.
  double thrust_per_sf = thrust_magnitude / num_noz;
  // Start by initializing the system values to allow incremental accumulation
  jeod::Vector3::initialize(thrust);
  jeod::Vector3::initialize(thrust_vac);
  jeod::Vector3::initialize(moment);

  for (unsigned int ii = 0; ii<num_noz; ii++)  {
    nozzles_ptr_vec[ii]->compute_thrust_mag( thrust_per_sf);
    // Perform back-pressure adjustment
    // NOTE - The atmospheric pressure adjustment doesn't affect the mass
    //        flow rate.
    if (atm_press_adjust) {
      nozzles_ptr_vec[ii]->modify_thrust_mag_atmos( atmos_pressure );
    }

    // Generate the vector forces and moments:
    nozzles_ptr_vec[ii]->compute_thrust_vec();

    // Increment the system moment with the nozzle moment
    double pos_wrt_cm[3];
    jeod::Vector3::diff( nozzles_ptr_vec[ii]->position,
                   veh_cm,
                   pos_wrt_cm);

    // Add the nozzle thrust and moment to the system totals
    if (using_flex) { // Adjust thrust and moment
      double new_position[3]; // For calculating moment
      jeod::Vector3::sum(pos_wrt_cm, motor_lin_flex + 3*ii, new_position);

      double rotang = jeod::Vector3::vmag(motor_rot_flex + 3*ii);
      if (rotang > flex_threshold) {
        // Euler rotation: angle = vmag(motor_rot_flex_ii)
        //           unit vector = motor_rot_flex_ii / vmag(motor_rot_flex_ii)
        //     Where motor_rot_flex_ii is this nozzle's flex angle vector
        // Perform the rotation using a quaternion
        jeod::Quaternion Q_flex;
        Q_flex.scalar = std::cos(rotang / 2);
        jeod::Vector3::scale( motor_rot_flex + 3*ii,
                        std::sin(rotang / 2) / rotang,
                        Q_flex.vector);

        // Rotate nozzle thrust and thrust_vac and add them to the totals
        // NOTE -- the quaternion just constructed is a right-rotation
        //         quaternion. We can use JEOD left-transformation quaternion
        //         math because a right-rotation and a left-transformation
        //         quaternion are equivalent.
        double rotated_thrust_vec[3];
        Q_flex.left_quat_transform( nozzles_ptr_vec[ii]->thrust_vac,
                                    rotated_thrust_vec);
        jeod::Vector3::incr(rotated_thrust_vec, thrust_vac);

        Q_flex.left_quat_transform( nozzles_ptr_vec[ii]->thrust,
                                    rotated_thrust_vec);
        jeod::Vector3::incr(rotated_thrust_vec, thrust);
        // Increment moment based on new position and rotated thrust
        jeod::Vector3::cross_incr(new_position, rotated_thrust_vec, moment);
      }
      else {
        jeod::Vector3::incr(nozzles_ptr_vec[ii]->thrust_vac, thrust_vac);
        jeod::Vector3::incr(nozzles_ptr_vec[ii]->thrust, thrust);
        // Increment moment based on new position and non-rotated thrust
        jeod::Vector3::cross_incr(new_position, nozzles_ptr_vec[ii]->thrust, moment);
      }
    }
    else {
      jeod::Vector3::incr(nozzles_ptr_vec[ii]->thrust_vac, thrust_vac);
      jeod::Vector3::incr(nozzles_ptr_vec[ii]->thrust, thrust);
      jeod::Vector3::cross_incr(pos_wrt_cm, nozzles_ptr_vec[ii]->thrust, moment);
    }
  }
  thrust_magnitude = jeod::Vector3::vmag(thrust);
  thrust_vac_mag = jeod::Vector3::vmag(thrust_vac);
  net_roll_torq = moment[0];
}

/*****************************************************************************
shutdown_motor
Purpose:(Shuts the motor down)
*****************************************************************************/
void
RocketMotor_MultiNozzle::shutdown_motor()
{
  jeod::Vector3::initialize(thrust_vac);
  net_roll_torq  = 0.0;
  for (unsigned int ii = 0; ii<num_noz; ii++)  {
    nozzles_ptr_vec[ii]->shutdown_nozzle();
  }
  RocketMotor_Basic::shutdown_motor();
}

/*******************************************************************************
enable_flex
Purpose:(Activates using_flex flag)
*******************************************************************************/
void
RocketMotor_MultiNozzle::enable_flex()
{
  // If the model has already been initialized, check:
  //   the array-access for non-NULL and
  //   the number of elements to be correct for the number of nozzles
  // If not initialized, set using_flex = true; these will then be checked at
  // initialization.
  if ( !initialized ||
       (motor_lin_flex && motor_rot_flex && (num_flex_elements==3* num_noz))) {
    using_flex = true;
  }
  else {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid configuration\n",
      "Instruction received to enable flex but the flex arrays are NULL.\n"
      "Flex remains disabled.\n");
  }
}

/*****************************************************************************
set_flex_threshold
Purpose:(Sets the flex threshold, as long as the new value is valid.)
*****************************************************************************/
void
RocketMotor_MultiNozzle::set_flex_threshold(double new_threshold)
{
  if (new_threshold < 0.0 || MathUtils::is_near_equal(new_threshold, 0.0)) {
    CMLMessage::error(__FILE__, __LINE__, "Invalid flex threshold\n",
      "Attempted to set an invalid flex threshold of ", new_threshold, ".\n"
      "Threshold will remain at ", flex_threshold, ".\n");
  }
  else { flex_threshold = new_threshold; }
}
