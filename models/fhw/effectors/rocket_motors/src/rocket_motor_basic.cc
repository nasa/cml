/*******************************************************************************
PURPOSE:
   (Methods for the basic solid-rocket motor)

PROGRAMMERS:
   (((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares) (initial)))
******************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/rocket_motor_basic.hh"


/*****************************************************************************
constructor
*****************************************************************************/
RocketMotor_Basic::RocketMotor_Basic(
        DynamicMassGroup                   & mass_group_in,
        DynamicMassBody                    * mass_body_in,
        DynamicMassString                  * mass_string_in,
        DynamicMassBodyPropertiesInterface & mass_properties_in,
        const double                       & time_in,
        const double                       * veh_cm_in,
        bool                                 use_mass_string_in)
  :
  mass_group(mass_group_in),
  prop_mass_body(mass_body_in),
  prop_mass_string(mass_string_in),
  dyn_mass_properties(mass_properties_in),
  time_now(time_in),
  veh_cm(veh_cm_in),
  dispersions(),
  commanded(false),
  force_mass_update(true),
  motor_can_be_shutdown(false),
  thrust_magnitude(0.0),
  thrust_unit_motor{0.0, 0.0, 0.0},
  position{0.0, 0.0, 0.0},
  mass_flow_rate(0.0),
  T_struc_to_motor_frame{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
  thrust{0.0, 0.0, 0.0},
  moment{0.0, 0.0, 0.0},
  status(Inactive),
  mass_group_internal(),
  use_mass_string(use_mass_string_in),
  dt(0.0),
  time_last(0.0),
  command_time(0.0),
  burnout_time(0.0),
  thrust_unit_struc{0.0, 0.0, 0.0}
{
  if (!use_mass_string) {
    mass_group_internal.add_mass_to_group(prop_mass_body);
  }
}
/****************************************************************************/
RocketMotor_Basic::RocketMotor_Basic (
        DynamicMassBody & mass_body_in,
        const double    & time_in,
        const double    * veh_cm_in)
  :
  RocketMotor_Basic( mass_group_internal,
                     &mass_body_in,
                     nullptr,
                     mass_body_in.dynamic_properties,
                     time_in,
                     veh_cm_in,
                     false)
{}
/****************************************************************************/
RocketMotor_Basic::RocketMotor_Basic (
        DynamicMassGroup & mass_group_in,
        DynamicMassBody & mass_body_in,
        const double    & time_in,
        const double    * veh_cm_in)
  :
  RocketMotor_Basic( mass_group_in,
                     &mass_body_in,
                     nullptr,
                     mass_body_in.dynamic_properties,
                     time_in,
                     veh_cm_in,
                     false)
{}
/****************************************************************************/
RocketMotor_Basic::RocketMotor_Basic (
        DynamicMassString & mass_string_in,
        const double      & time_in,
        const double      * veh_cm_in)
  :
  RocketMotor_Basic( mass_group_internal,
                     nullptr,
                     &mass_string_in,
                     mass_string_in,
                     time_in,
                     veh_cm_in,
                     true)
{}
/****************************************************************************/
RocketMotor_Basic::RocketMotor_Basic (
        DynamicMassGroup  & mass_group_in,
        DynamicMassString & mass_string_in,
        const double      & time_in,
        const double      * veh_cm_in)
  :
  RocketMotor_Basic( mass_group_in,
                     nullptr,
                     &mass_string_in,
                     mass_string_in,
                     time_in,
                     veh_cm_in,
                     true)
{}

/*****************************************************************************
initialize
Purpose:(Used to initialize the model, the mass properties of the model,
         and to initialize the dispersions.)
*****************************************************************************/
void
RocketMotor_Basic::initialize()
{
  if (veh_cm == nullptr) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid construction\n",
      "The vehicle-CM pointer has not been set, and is required for\n"
      "generation of the moment.\n");
  }

  // Initialize the propellant mass, and ensure that it has some
  // consumable-mass available.
  if (use_mass_string) {
    // Add the string to the group -- but make sure it is not empty or bad
    // things happen; also make sure it has not already been added or there
    // will be an error message sent.
    if (prop_mass_string == nullptr) {
      // If using a mass-string, prop_mass_string should have been set in the
      // constructor.  This case should be impossible to hit.
      CMLMessage::fail(
        __FILE__,__LINE__,"Unknown configuration\n",
        "The prop_mass_string pointer has not been set, but the "
        "use_mass_string\nswitch has been set to True\n"
        "This configuration is not supported.\n");
    }
    if (prop_mass_string->get_body_collection_size() == 0) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid configuration\n",
        "The specified prop_mass_string has no bodies associated with it.\n"
        "This configuration is not supported.\n");
    }
    if (!mass_group.is_string_in_group( prop_mass_string)) {
      mass_group.add_string_to_group(prop_mass_string);
    }
  }
  else {
    if (prop_mass_body == nullptr) {
      // If not using a mass-string, prop_mass_body should have been set in the
      // constructor.  This case should be impossible to hit.
      CMLMessage::fail(
        __FILE__,__LINE__,"Unknown configuration\n",
        "The prop_mass_body pointer has not been set, but the use_mass_string\n"
        "switch has been set to False, indicating that the model is using a\n"
        "dynamic-mass-body directly.\nThis use-case is not supported.\n");
    }
    if (!mass_group.is_body_in_group( prop_mass_body)) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid configuration\n",
        "The specified prop_mass_body is not managed by the specified "
        "mass-group.\nThis configuration is not supported.\n");
    }
  }

  mass_group.initialize();
  mass_group.subscribe();

  if (dyn_mass_properties.consumable_mass < 0) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid Initialization\n",
      "The consumable mass cannot be < 0.\n"
      "Verify the specification of residual mass and core-mass.\n");
  }

  dispersions.apply_dispersions(position, T_struc_to_motor_frame);

  // Calculate thrust direction in structural frame
  double unit_vec_mag_sq = jeod::Vector3::vmagsq( thrust_unit_motor);
  if (MathUtils::is_near_equal( unit_vec_mag_sq, 0.0)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid unit vector specification\n",
      "The required directional unit-vector is still at the default [0,0,0]\n"
      "Setting the unit-vector to [1,0,0] instead.\n");
    jeod::Vector3::unit(0, thrust_unit_motor);
  }
  else if (!MathUtils::is_near_equal( unit_vec_mag_sq, 1.0)) {
    double scratch[3];
    jeod::Vector3::normalize( thrust_unit_motor, scratch);
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid unit vector specification\n",
      "The specified unit-vector  [", thrust_unit_motor[0], ", ", thrust_unit_motor[1], ", ", thrust_unit_motor[2], "] is not a unit vector.\n"
      "Normalizing this vector to [", scratch[0], ", ", scratch[1], ", ", scratch[2], "]\n");
    jeod::Vector3::copy(scratch, thrust_unit_motor);
  }
  jeod::Vector3::transform_transpose( T_struc_to_motor_frame,
                                thrust_unit_motor,
                                thrust_unit_struc);

  SubscriptionBase::initialize();
}

/*****************************************************************************
update
Purpose:(Updates the rocket effects. Top level executable.)
*****************************************************************************/
void
RocketMotor_Basic::update()
{
  if (!active) {
    return;
  }

  if (!update_status()) {
    return;
  }

  update_mass_consumption();
  generate_torque();
}

/*****************************************************************************
hold_motor
Purpose:(Temporarily shuts down the motor, switching to Inactive status.)
*****************************************************************************/
void
RocketMotor_Basic::hold_motor()
{
  status = Inactive;
  jeod::Vector3::initialize(thrust);
  jeod::Vector3::initialize(moment);
}

/*****************************************************************************
shutdown_motor
Purpose:(Shuts down the motor, setting the status to Finished which will
         deactivate it.  This is a more permanent shutdown than hold_motor())
*****************************************************************************/
void
RocketMotor_Basic::shutdown_motor()
{
  status = Finished;
  burnout_time = time_now;
  mass_flow_rate = 0.0;
  jeod::Vector3::initialize(thrust);
  jeod::Vector3::initialize(moment);
  thrust_magnitude = 0.0;
}

/*****************************************************************************
generate_torque
Purpose:( Generates torque)
Limitation: (Only usable for a rocket motor with a single nozzle.)
*****************************************************************************/
void
RocketMotor_Basic::generate_torque()
{
  double motor_wrt_com[3];
  // Got to do this every cycle because the position of the CoM can be moving
  // around.
  jeod::Vector3::diff(  position,
                  veh_cm,
                  motor_wrt_com);
  jeod::Vector3::cross( motor_wrt_com,
                  thrust,
                  moment);
}

/*****************************************************************************
update_status
Purpose:(Checks for changes in operating status.)
*****************************************************************************/
bool
RocketMotor_Basic::update_status()
{
  bool ret_val = false;
  switch (status) {
  case Finished:
    // If motor is finished (i.e. depleted), deactivate the model.
    // Note -- an inactive model will never get to this point, so this happens only once
    deactivate();
    break;
  case Inactive:
    // check the commanded flag.  If it has been set, start the motor,
    // otherwise return with no effect.
    if (commanded) {
      start_motor();
      ret_val = true;
    }
    break;
  case Firing:
    // Check that time has elapsed since the last call.
    // If not, nothing to do here
    if (!MathUtils::is_near_equal(time_now, time_last)) {
      dt = time_now - time_last;
      time_last = time_now;
      ret_val = true;
    }
    if (motor_can_be_shutdown && !commanded) {
      hold_motor();
      ret_val = false;
    }
    break;
  default:
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid motor status\n",
      "Motor status has been set to an invalid setting.\n"
      "Don't know how to handle this setting, ", status, ".\n");
    break;
  }
  return ret_val;
}

/*****************************************************************************
update_mass_consumption
Purpose:(Updates the mass consumed by this motor; checks for available mass
         to continue.)
*****************************************************************************/
void
RocketMotor_Basic::update_mass_consumption()
{
  // Update mass.
  // compute the mass consumed for this tank
  dyn_mass_properties.mass_consumed_step += dt  * mass_flow_rate;
  if (dyn_mass_properties.consumable_mass <
      dyn_mass_properties.mass_consumed_step) {
    shutdown_motor();
  }
  if (force_mass_update) {
    mass_group.update_group_mass();
  }
}

/*****************************************************************************
start_motor
Purpose:(Starts the motor running)
*****************************************************************************/
void
RocketMotor_Basic::start_motor()
{
  status = Firing;
  command_time = time_now;
  time_last = time_now;
  dt = 0.0;
  jeod::Vector3::scale( thrust_unit_struc, thrust_magnitude, thrust);
}

/*****************************************************************************
activate
Purpose:(activates the model from a subscription call
*****************************************************************************/
void
RocketMotor_Basic::activate()
{
  // if status is Finished, then the model will immediately deactivate
  // again.  So set a Finished status to Inactive -- which means wait for a
  // command.
  if (status == Finished) {
    status = Inactive;
  }
  SubscriptionBase::activate();
}