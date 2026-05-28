/*******************************************************************************

   PURPOSE:
   (Provides the capability to spontaneously create a dummy vehicle mid-sim.
    This is useful for creating copies of a vehicle to test different
    environment responses, or for applying a current vehicle state to another
    body with a different mass.)

ASSUMPTIONS AND LIMITATIONS:
    (Requires two sets of data inputs, one for the mass and one for the state.
     These data may come from a single vehicle, or from 2 different vehicles,
     but the source of those data must be available somewhere in the
     existing simulation.)

   PROGRAMMERS:
    (((Gary Turner) (OSR) (August 2014) (New))
     ((Gary Turner) (OSR) (March 2015) (Modification to make more generic))
    )
*******************************************************************************/
#include <cstring> // NULL
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "jeod/models/utils/math/include/vector3.hh"


#include "../include/dummy_vehicle_launcher.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
DummyVehicleLauncher::DummyVehicleLauncher(
                jeod::DynBody& body_in,
                const jeod::DynBody& real_body_in)
    :
    copy_rot_state(false),
    active(false),
    add_to_integ_group_at_launch(false),
    integ_group_specified(false),
    body(body_in),
    real_mass_body(real_body_in.mass),
    real_state_body(real_body_in),
    intended_integ_body(NULL),
    intended_integ_group(NULL)
{
  body.translational_dynamics = false;
  body.rotational_dynamics = false;
}
/****************************************************************************/
DummyVehicleLauncher::DummyVehicleLauncher(
                jeod::DynBody& body_in,
                const jeod::MassBody& real_mass_body_in,
                const jeod::DynBody& real_state_body_in)
    :
    copy_rot_state(false),
    active(false),
    add_to_integ_group_at_launch(false),
    integ_group_specified(false),
    body(body_in),
    real_mass_body(real_mass_body_in),
    real_state_body(real_state_body_in),
    intended_integ_body(NULL),
    intended_integ_group(NULL)
{
  body.translational_dynamics = false;
  body.rotational_dynamics = false;
}

/*****************************************************************************
initialize_integ_group_actions
Purpose:(Removes the dummy vehicle from the integration group and stores the
         integration group for later use.)
Limitations:(If utilizing the capabilities of integration-loop switching,
             running this will likely result in errors.
             By switching integ-loops, all registered bodies are assumed to
             remain with their loop's integration group.  Pulling bodies in
             and out of integration groups confuses the integration-loop
             management.
             If this method is not run, the body will just stay with the
             integration group associated with the sim-object's
             integration-loop.)
*****************************************************************************/
void
DummyVehicleLauncher::initialize_integ_group_actions()
{
  if (active) { // already "launched".  This shouldn't happen.
    return;
  }
  // During its period of inactivity, the body should not be integrated.
  // However, for consistency, it gets added to the jeod::DynManager at
  // sim-initialization, so also gets added to an integration group.
  // That is not necessary until after it gets launched.
  // Grab the current integration group and store it off so that the body can
  // be added back at launch.  Then remove the body from the group.
  jeod::DynamicsIntegrationGroup * current_integ_group =
                     body.get_dynamics_integration_group();

  if (current_integ_group != NULL) {
    current_integ_group->delete_dyn_body(body);
  }

  // Store this integration group for later resurrection UNLESS the user has
  // already specified a particular target for the future integration group.
  if (intended_integ_group == NULL && intended_integ_body == NULL) {
    if (current_integ_group == NULL) {
      CMLMessage::warn (
        __FILE__, __LINE__, "VehicleLauncher::InitializationError\n",
        "The initialization of the dummy-vehicle-launcher for vehicle ", body.name, ""
        "\ndoes not have a specified integ group. If you're not setting "
        "\nthe integ group at a later time, it will try to use the same "
        "\ninteg group as ", real_state_body.name, ".\n");
    }
    intended_integ_group = current_integ_group;
  }
  add_to_integ_group_at_launch = true;
}

/*****************************************************************************
launch
Purpose:(Copies the necessary data over)
*****************************************************************************/
void
DummyVehicleLauncher::launch()
{
  if (integ_group_specified && !add_to_integ_group_at_launch) {
    process_inconsistent_setup();
  }
  if (add_to_integ_group_at_launch) {
    add_to_integ_group();
  }

  body.translational_dynamics = true;

  // mass properties
  body.mass.core_properties.mass = real_mass_body.composite_properties.mass;
  body.mass.composite_properties.mass = real_mass_body.composite_properties.mass;

  jeod::Vector3::copy( real_mass_body.composite_properties.position,
                       body.mass.core_properties.position);
  jeod::Vector3::copy( real_mass_body.composite_properties.position,
                       body.mass.composite_properties.position);

  jeod::Matrix3x3::copy( real_mass_body.composite_properties.inertia,
                         body.mass.core_properties.inertia);
  jeod::Matrix3x3::copy( real_mass_body.composite_properties.inertia,
                         body.mass.composite_properties.inertia);

  jeod::Matrix3x3::copy( real_mass_body.composite_properties.T_parent_this,
                         body.mass.core_properties.T_parent_this);
  jeod::Matrix3x3::copy( real_mass_body.composite_properties.T_parent_this,
                         body.mass.composite_properties.T_parent_this);

  body.mass.set_update_flag();
  body.mass.update_mass_properties();

  // Dynamic state

  jeod::Vector3::copy( real_state_body.composite_body.state.trans.position,
                       body.core_body.state.trans.position);
  jeod::Vector3::copy( real_state_body.composite_body.state.trans.position,
                       body.composite_body.state.trans.position);

  jeod::Vector3::copy( real_state_body.composite_body.state.trans.velocity,
                       body.core_body.state.trans.velocity);
  jeod::Vector3::copy( real_state_body.composite_body.state.trans.velocity,
                       body.composite_body.state.trans.velocity);

  jeod::Vector3::copy( real_state_body.derivs.trans_accel,
                       body.derivs.trans_accel);
  jeod::Vector3::copy( real_state_body.derivs.non_grav_accel,
                       body.derivs.non_grav_accel);

  if (copy_rot_state) {
    jeod::Vector3::copy( real_state_body.composite_body.state.rot.ang_vel_this,
                         body.core_body.state.rot.ang_vel_this);
    jeod::Vector3::copy( real_state_body.composite_body.state.rot.ang_vel_this,
                         body.composite_body.state.rot.ang_vel_this);

    jeod::Vector3::copy(real_state_body.composite_body.state.rot.Q_parent_this.vector,
                        body.core_body.state.rot.Q_parent_this.vector);
    jeod::Vector3::copy(real_state_body.composite_body.state.rot.Q_parent_this.vector,
                        body.composite_body.state.rot.Q_parent_this.vector);
    body.core_body.state.rot.Q_parent_this.scalar =
    body.composite_body.state.rot.Q_parent_this.scalar =
                  real_state_body.composite_body.state.rot.Q_parent_this.scalar;

    jeod::Matrix3x3::copy( real_state_body.composite_body.state.rot.T_parent_this,
                           body.core_body.state.rot.T_parent_this);
    jeod::Matrix3x3::copy( real_state_body.composite_body.state.rot.T_parent_this,
                           body.composite_body.state.rot.T_parent_this);

    jeod::Vector3::copy( real_state_body.derivs.rot_accel,
                         body.derivs.rot_accel);
  }

  active = true;
}

/*****************************************************************************
set_intended_integ_group
Purpose:(Sets the desired integ-group to which the body will be added at launch)
*****************************************************************************/
void
DummyVehicleLauncher::set_intended_integ_group(
      jeod::DynamicsIntegrationGroup * tgt)
{
  if (tgt == NULL) {
    CMLMessage::warn (
      __FILE__, __LINE__, "VehicleLauncher::StartupError\n",
      "The intended integration group was not specified, default behavior \n"
      "will be executed. This behavior may not be what the user intends, \n"
      "please check your setup.\n");
    integ_group_specified = false;
  }
  else {
    integ_group_specified = true;
  }
  intended_integ_group = tgt;
  intended_integ_body = NULL;
}

/*****************************************************************************
set_intended_integ_body
Purpose:(Sets the desired integ-group to be associated with a particular body
*****************************************************************************/
void
DummyVehicleLauncher::set_intended_integ_body(
      jeod::DynBody * tgt)
{
  if (tgt == NULL) {
    CMLMessage::warn (
      __FILE__, __LINE__, "VehicleLauncher::StartupError\n",
      "The intended integration body was not specified, default behavior \n"
      "will be executed. This behavior may not be what the user intends, \n"
      "please check your setup.\n");
  }
  integ_group_specified = true;
  intended_integ_body = tgt;
  intended_integ_group = NULL;
}

/*****************************************************************************
set_intended_integ_body_state_body
Purpose:(Sets the desired integ-group to be associated with the body that is
         used to set the state.)
Note - Because real_state_body is protected, users may not have direct access
       to it.  In that sense, this is a getter/setter combined method.
*****************************************************************************/
void
DummyVehicleLauncher::set_intended_integ_body_state_body()
{
  integ_group_specified = true;
  intended_integ_body = const_cast<jeod::DynBody *>(&real_state_body);
  if (intended_integ_body == NULL) {
    CMLMessage::warn (
      __FILE__, __LINE__, "VehicleLauncher::StartupError\n",
      "The intended integration body was not specified, default behavior \n"
      "will be executed. This behavior may not be what the user intends, \n"
      "please check your setup.\n");
  }
  intended_integ_group = NULL;
}

/*****************************************************************************
add_to_integ_group
Purpose:(adds the jeod::DynBody to a new integration group)
Limitations: (Can only be run if the body was previously removed from its
              default integration group by initialize_integ_group_actions.)
*****************************************************************************/
void
DummyVehicleLauncher::add_to_integ_group()
{
  //  If intended group specified, it was already processed from launch().
  //  Otherwise, try the intended integ-body's group
  if (intended_integ_body != NULL) {
    intended_integ_group =
                    intended_integ_body->get_dynamics_integration_group();
  }
  // Last resort: if neither is specified, pick the real-state-body's group
  else if ( intended_integ_group == NULL) {
    // temporarily cast away the const-ness to allow access to the
    // real-state-body's integ group.
    intended_integ_group =
       const_cast<jeod::DynBody &>(real_state_body).get_dynamics_integration_group();
    // Last resort failed:
    if ( intended_integ_group == NULL) {
      CMLMessage::error (
        __FILE__, __LINE__, "VehicleLauncher::StartupError\n",
        "The intended integration group was not specified.\n"
        "Unable to add vehicle ", body.name, " to any integration group.\n"
        "It will not be integrated.\n");
        return;
    }
    // Last resort succeeded.  Send a warning, shouldn't rely on this.
    else {
      CMLMessage::warn (
        __FILE__, __LINE__, "VehicleLauncher::StartupError\n",
        "The intended integration group was not specified.\n"
        "Adding the vehicle to the same integration group as the body from\n"
        "which it obtains its state.\n");
    }
  }
  intended_integ_group->add_dyn_body( body);
}

/*****************************************************************************
process_inconsistent_setup
Purpose:(Called when intended integration group has been set, but the
         initialize_integ_group_actions was not called. This is a user-error.)
*****************************************************************************/
void
DummyVehicleLauncher::process_inconsistent_setup()
{
  jeod::DynamicsIntegrationGroup * current_integ_group =
                   body.get_dynamics_integration_group();
  if (current_integ_group == NULL) {
    CMLMessage::warn (
      __FILE__, __LINE__, "VehicleLauncher::StartupError\n",
      "An intended integration group was specified, but the jeod::DynBody ", body.name, "\n"
      "cannot be removed from its existing integration group.\n"
      "Ignoring specified values.\n");
    return;
  }
  // else

  CMLMessage::warn (
    __FILE__, __LINE__, "VehicleLauncher::StartupError\n",
    "The automated integration group switching was not activated,\n"
    "but an intended integration group was specified.\n"
    "Removing the jeod::DynBody ", body.name, " from its current integration group\n"
    "and moving it to the specified integration group.\n");
  current_integ_group->delete_dyn_body( body );
  add_to_integ_group_at_launch = true;
}
