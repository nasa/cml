/***************************************************************************************************
PURPOSE:
 (Provide a manager class to apply perturbation and/or maneuvers to a vehicle body.
  The perturbations are calculated in the PerturbModel class. The maneuvers are calculated
  in the ManeuverModel.)

ASSUMPTIONS:
 ((This model does not compute exact dynamics. It applies prescribed kinematics onto a
   vehicle body. The body angular velocity and attitude does not represent an actual behavior of 
   a vehicle but are a sufficient approximation for the purpose of this model.
  (The attitude and angular velocity of the vehicle are calculated with respect to the inertial 
   frame.))

PROGRAMMERS:
 ((Alexandre Masset) (OSR) (10/25)     (Initial Implementation))
***************************************************************************************************/

//CML headers
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/attitude_manager.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
AttitudeManager::AttitudeManager( const double      & time_in, 
                                  jeod::DynBody & body_in)
    :
    dynamicTime( time_in),
    body( body_in),
    quatBodyWrtRef(),
    dynamicTimePrevious( 0.0),
    timeStep( 0.0),
    RefWrtInertialFrameState(nullptr),
    w_BodyWrtRef_Body{ 0.0, 0.0, 0.0},
    w_RefWrtInertial_Body{ 0.0, 0.0, 0.0},
    config( ATT_MNGR_CONF::NONE),
    rotational_dynamics_prev( false),
    perturbation( 
        timeStep, 
        body, 
        quatBodyWrtRef,
        w_BodyWrtRef_Body),
    maneuver(
        timeStep,
        body,
        quatBodyWrtRef,
        w_BodyWrtRef_Body)
{}

/*****************************************************************************
activate
Purpose:
  Activation method. Sets the current attitude and rates wrt to Ref frame and
  activate submodels that have been subscribed.
*****************************************************************************/
void
AttitudeManager::activate()
{
    if (!initialized) { return;}
    active = true;
    // Store previous value of rotational_dynamics, resumed after model deactivation
    rotational_dynamics_prev = body.rotational_dynamics;
    body.rotational_dynamics = false;

    // Compute at activation current attitude wrt Ref
    if (RefWrtInertialFrameState) {
        // q_BR = q_BI * q_RI^(-1)
        body.composite_body.state.rot.Q_parent_this.multiply_conjugate( 
            RefWrtInertialFrameState->rot.Q_parent_this, 
            quatBodyWrtRef);
        
        quatBodyWrtRef.left_quat_transform( 
            RefWrtInertialFrameState->rot.ang_vel_this, // w_RefWrtInertial_Ref
            w_RefWrtInertial_Body);
    } else {
        // q_BR = q_BI
        quatBodyWrtRef = body.composite_body.state.rot.Q_parent_this;
        jeod::Vector3::initialize( w_RefWrtInertial_Body);
    }

    // Compute at activation current attitude rate wrt Ref
    // w_BR_B = w_BI_B - [BR]*w_RI_R 
    jeod::Vector3::diff(body.composite_body.state.rot.ang_vel_this,
        w_RefWrtInertial_Body,
        w_BodyWrtRef_Body);

    /* The attitude manager works as a state-machine that has four possible states.
        - NONE: no action taken
        - PERT: perturbations are prescribed (no duration specified)
        - MNVR: maneuver is prescribed (for a limited time)
        - MNVR_PERT: a maneuver is prescribed while perturbations are hold during that
            time.
    While perturbations and maneuver can be subscribed at the same time, they
    cannot be active simultaneously. Maneuver will take precedence over perturbation
    if that's the case. In summary:
        - when maneuvering, we do not apply perturbations
        - when not maneuvering, we apply perturbations if subscribed
        - when not maneuvering and perturbations not subscribed, we don't do anything.
    */

    switch (config) {
        case ATT_MNGR_CONF::PERT:
            perturbation.activate();
            break;
        case ATT_MNGR_CONF::MNVR:
            maneuver.activate();
            break;
        case ATT_MNGR_CONF::MNVR_PERT:
            // Reset perturbed angles to guarantee the attitude remains continuous
            // if transitioning between a maneuver and perturbations.
            perturbation.deactivate();
            maneuver.activate();
            break;
        case NONE:
            break;
    }
}

/*****************************************************************************
initialize
Purpose:
  Turns off the rotational dynamics. The model prescribes rates and attitude.
  Initializes the maneuver and perturbation models if they have been subscribed.
*****************************************************************************/
void
AttitudeManager::initialize()
{
    if (!enabled) { return;}

    perturbation.initialize();
    maneuver.initialize();

    SubscriptionBase::initialize();
}


/*****************************************************************************
subscribed_perturbation
Purpose:
  Subscribes the perturbation model and change flags accordingly.
  NOTE:
    - perturbation.activate() is protected against uninitialized submodel
*****************************************************************************/
void
AttitudeManager::subscribe_perturbation()
{
    if (config == ATT_MNGR_CONF::PERT || config == ATT_MNGR_CONF::MNVR_PERT) {
        CMLMessage::warn(
            __FILE__,__LINE__,"Perturbation submodel is already subscribed.\n"
        "Command ignored.\n");
        return;
    }

    if (config == ATT_MNGR_CONF::MNVR) {
        config = ATT_MNGR_CONF::MNVR_PERT;
        // Maneuvering always takes precedence on perturbations. It is important
        // to reset perturbed angles to guarantee the attitude remains continuous
        // if transitioning between a maneuver and perturbations.
        perturbation.deactivate();
    } else if (config == ATT_MNGR_CONF::NONE) {
        config = ATT_MNGR_CONF::PERT;
    }

    /* Submodel can only be activated if the attitude manager is already active.
    More specifically for perturbations, the submodel can only be activated if
    there's no ongoing maneuver. Submodel activation acts as a model initialization
    computing the required value at the time it is activated. */
    if (active && config <= ATT_MNGR_CONF::PERT) {
        perturbation.activate();
    }
}


/*****************************************************************************
subscribe_maneuver
Purpose:
  Subscribes the maneuver model and change flags accordingly.
  NOTE:
    - If attitude_manager has not been initialized yet, maneuver_model has not
    been initialized either
    - maneuver.activate() is protected against uninitialized submodel
*****************************************************************************/
void
AttitudeManager::subscribe_maneuver()
{
    if (config == ATT_MNGR_CONF::MNVR || config == ATT_MNGR_CONF::MNVR_PERT) {
        CMLMessage::warn(
            __FILE__,__LINE__,"Maneuver submodel is already subscribed.\n"
        "Command ignored.\n");
        return;
    }
    if (config == ATT_MNGR_CONF::PERT) {
        config = ATT_MNGR_CONF::MNVR_PERT;
        // Maneuvering always takes precedence on perturbations. It is important
        // to reset perturbed angles to guarantee the attitude remains continuous
        // if transitioning between a maneuver and perturbations.
        perturbation.deactivate();
    } else if (config == ATT_MNGR_CONF::NONE) {
        config = ATT_MNGR_CONF::MNVR;
    }
    
    if (active) {maneuver.activate();}
}


/*****************************************************************************
update
Purpose:
  Update call. Computes the time step and update the maneuver and perturbation
  models. Updates the vehicle body attitude and angular velocity.
  NOTE:
    - Perturbation model is not updated when the maneuver is in progress.
*****************************************************************************/
void
AttitudeManager::update()
{
    if (!active) {
        return;
    }

    /* Update the simulation update rate for the submodels. */
    timeStep = dynamicTime - dynamicTimePrevious;
    dynamicTimePrevious = dynamicTime;

    /* The maneuver submodel is internally deactivated when the maneuver ends.
    Checks if the submodel is inactive and unsubscribe it is. */
    if ( (config == ATT_MNGR_CONF::MNVR || config == ATT_MNGR_CONF::MNVR_PERT) 
            && !maneuver.is_active()) {
        unsubscribe_maneuver();
    } 
    // NOTE: Maneuver and perturbation submodels are internally protected against 
    // update calls when deactivated. They also cannot be active at the same time.

    // Update the maneuver submodel
    maneuver.update();
    // Update the perturbation submodel
    perturbation.update();
    
    // Update rotational state
    update_attitude();
    update_ang_vel();
    body.propagate_state();
}


/*****************************************************************************
unsubscribe_perturbation
Purpose:
  Unsubscribes the perturbation model and change flags accordingly.
  Deactivates the model (no update calls).
  NOTE:
    - Internally resets the perturbed Euler angles in perturbation.deactivate()
    The attitude at unsubscription becomes the nominal attitude.
    Consequently, sequential subscription/unsubscription of perturb_model can
    gradually shift the nominal attitude (this is NOT a recommended use of 
    the subscription methods)
    - When combined with a maneuver, the resulting attitude becomes the new 
    nominal attitude.
    - Resets angular velocity Body-wrt-Ref if there's no ongoing maneuver
*****************************************************************************/
void
AttitudeManager::unsubscribe_perturbation()
{
    if (config == ATT_MNGR_CONF::MNVR_PERT) { config = ATT_MNGR_CONF::MNVR;}
    else if (config == ATT_MNGR_CONF::PERT) { 
        config = ATT_MNGR_CONF::NONE;
        MathUtils::zero_vector(w_BodyWrtRef_Body);
    }
    perturbation.deactivate();
}


/*****************************************************************************
unsubscribe_maneuver
Purpose:
  Unsubscribes the maneuver model and change flags accordingly.
  Deactivates the submodel (no update calls).
  NOTE:
    - Resumes any perturbation that was subscribed prior to/during the maneuver
    - Resets angular velocity Body-wrt-Ref if there's no perturbation
*****************************************************************************/
void
AttitudeManager::unsubscribe_maneuver()
{ 
    if (config == ATT_MNGR_CONF::MNVR_PERT) {
        config = ATT_MNGR_CONF::PERT;
        // Resume perturbations that were on hold
        perturbation.activate();
    } else if (config == ATT_MNGR_CONF::MNVR) { 
        config = ATT_MNGR_CONF::NONE;
    }
    maneuver.deactivate();
}

/*****************************************************************************
deactivate
Purpose:
  Unsubscribes every submodel and deactive attitude manager.
*****************************************************************************/
void
AttitudeManager::deactivate()
{
    unsubscribe_maneuver();
    unsubscribe_perturbation();
    // Need a last update call to reset body rates to zero
    update();
    active = false;
    // Restore previous rotational dynamics flag
    body.rotational_dynamics = rotational_dynamics_prev;
}

/*****************************************************************************
set_ref_frame
Purpose:
  Sets the reference frame state to which the vehicle is fixed (aside any
  perturbation or maneuver). Thiscan be a dynamic reference frame state that is
  updated in the sim. If NULL, it is considered that the vehicle is fixed with 
  respect to the Inertial frame.
*****************************************************************************/
void 
AttitudeManager::set_ref_frame_state( jeod::RefFrameState * ref_frame_state)
{
    if (active) {
        CMLMessage::warn(
            __FILE__,__LINE__,"Cannot set Ref reference frame if Attitude Manager "
            "is already active.\n",
            "Attempt failed. No action performed.\n");
            return;
    }
    if (ref_frame_state == nullptr) {
        CMLMessage::warn(
            __FILE__,__LINE__,"Configuration error\n",
            "Attempt to assign a reference frame state that is NULL.\n"
            "This is not a valid setting. NULL pointer still assigned, assuming"
            "the resulting frame is Inertial frame.\nAttempt failed.\n");
        // Reset angular rate since we consider Ref equivalent to Inertial in that case
        jeod::Vector3::initialize(w_RefWrtInertial_Body);
    }
    RefWrtInertialFrameState = ref_frame_state;
}


/*****************************************************************************
update_attitude
Purpose:
  Updates the vehicle body attitude with respect to inertial frame.
  This attitude can be described by the following quaternion operation:
  q_BI = q_BR * q_RI
  where I is the inertial frame
        R is the provided reference frame
        B is the perturbed body frame (with or without perturbation, depending
        on current mode)
  1) Update q_BR as: q_BR(t+dt) = dq * q_BR(t)    (valid for left-quaternions)
    with dq the "small-angle" attitude quaternion between the two time steps.
  2) Add q_RI if necessary
*****************************************************************************/
void
AttitudeManager::update_attitude()
{
    // Compute dq attitude quaternion with current body angular rate
    jeod::Quaternion delta_quat;
    double delta_angle = jeod::Vector3::vmag(w_BodyWrtRef_Body) * timeStep ;
    double rate_axis[3];
    jeod::Vector3::normalize(w_BodyWrtRef_Body, rate_axis);
    delta_quat.left_quat_from_eigen_rotation( delta_angle,
                                              rate_axis);
    // Apply q_BR(t+dt) = dq * q_BR(t)
    quatBodyWrtRef.multiply_left(delta_quat);

     /* Checks if a correct reference frame is provided. 
    The vehicle attitude is fixed with respect to this reference frame (aside
    perturbations or maneuvers).
    If not NULL: set q_BI = q_BR * q_RI.
    Else, assume q_BI = q_BR */
    if (RefWrtInertialFrameState) {
        quatBodyWrtRef.multiply( 
            RefWrtInertialFrameState->rot.Q_parent_this, 
            body.composite_body.state.rot.Q_parent_this);
    } else {
        body.composite_body.state.rot.Q_parent_this = quatBodyWrtRef;
    }
    body.composite_body.state.rot.Q_parent_this.left_quat_to_transformation(
        body.composite_body.state.rot.T_parent_this);
}


/*****************************************************************************
update_ang_vel
Purpose:
  Updates the vehicle body angular velocity with respect to inertial 
  frame, expressed in the perturbed body frame. This velocity can be described by 
  the following operation:
  w_IB_B = w_RB_B + [BR]*w_IR_R
  where I is the inertial frame
        R is the provided reference frame
        B is the perturbed body frame (with/without perturbation applied)
        w_RB_B is the angular velocity from B frame to R expressed in B
        w_IR_R is provided by the given reference frame state
*****************************************************************************/
void
AttitudeManager::update_ang_vel()
{
    /* Checks if a correct reference frame is provided. 
    The vehicle attitude is fixed with respect to this reference frame (aside
    perturbations or maneuvers).
    Set w_RI_R*/
    if (RefWrtInertialFrameState) {
        quatBodyWrtRef.left_quat_transform( 
            RefWrtInertialFrameState->rot.ang_vel_this, // w_RefWrtInertial_Ref
            w_RefWrtInertial_Body);
    }
    /* Assign angular velocity values to vehicle body */
    jeod::Vector3::sum( 
        w_BodyWrtRef_Body,
        w_RefWrtInertial_Body,
        body.composite_body.state.rot.ang_vel_this);
}