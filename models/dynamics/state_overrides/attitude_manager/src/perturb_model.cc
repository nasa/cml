/***************************************************************************************************
Purpose:
 (Force small perturbations model)

Assumptions:
 ((Small prescribed angles are added to a vehicle body in terms of Euler angle 'deltas')
  (This is used to create an apparent 'perturbation' motion for simulation purposes only)
  (Does not compute exact dynamics)
  (Enforces lateral velocity at docking port requirements if desired))

Programmers:
 ((Daniele Tancredi) (OSR) (08/18)     (Initial Implementation))
 ((Daniele Tancredi) (OSR) (03/20)     (Added motion generation on the fly))
 ((Daniele Tancredi) (OSR) (12/20)     (S-LVLH prescribed attitude))
 ((Jeb Stefan)       (OSR) (01/21)     (Prescribed vehicle maneuvers))
 ((Jeb Stefan)       (OSR) (05/21)     (Updated model functionality to meet ICD requirements))
 ((Jeb Stefan)       (OSR) (07/21)     (Initial merge into Artemis-II integration))
 ((Jeb Stefan)       (EG6) (11/23)     (Addition of Target Vehicle Accels and secondary att updates))
 ((Jeb Stefan)       (EG6) (06/25)     (Removing Target Vehicle Accels capability))
 ((Alexandre Masset) (OSR) (10/25)   
    (Complete refactor, separating maneuvers and deadbanding into submodels handled by an attitude 
     manager model))
***************************************************************************************************/
/* NOTE:
    This model inherits from the previous implementations of the perturbation model. It models
    a vehicle deadband (free-drift) limit, ie. the minimum free drift rate the vehicle can have while 
    remaining in a minimum attitude deadband area.
*/
// JEOD headers
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"

// CML headers
#include "cml/models/utilities/math_utils/include/math_utils.hh"

// Local headers
#include "../include/perturb_model.hh"

/**************************************************************************************************/
PerturbModel::PerturbModel( const double & timestep_in,
                            jeod::DynBody & body_in,
                            jeod::Quaternion & quatBodyWrtRef_out,
                            double (& w_BodyToRef_Body_out)[3]):
    body( body_in),
    initialized( false),
    randomRates( false),
    portName(),
    driftDeadbandInput{ 1.0, 1.0, 1.0},
    randSeed( 1),
    initPhase{ 0.0, 0.0, 0.0},
    maxDeltaLatVelDPInput{ 1.0, 1.0, 1.0},
    driftRateMaxInput{ 1.0, 1.0, 1.0},
    deadbandAccelInput{ 0.01, 0.01, 0.01},
    quatBodyWrtRef( quatBodyWrtRef_out),
    prescribedAng{ 0.0, 0.0, 0.0},
    driftRate( w_BodyToRef_Body_out),
    momentArmDP{0.0, 0.0, 0.0},
    T_DP_BODY(),
    deltaLatVelDP{ 0.0, 0.0, 0.0},
    driftRateIn{ 0.0, 0.0, 0.0},
    driftRateOut{ 0.0, 0.0, 0.0},
    driftDeadband{ 0.0, 0.0, 0.0},
    maxDeltaLatVelDP{ 0.0, 0.0, 0.0},
    driftRateMax{ 0.0, 0.0, 0.0},
    smoothing( false),
    enforceLatVel( false),
    deadbandAccel{ 0.0, 0.0, 0.0},
    accelRegion{ 0.0, 0.0, 0.0},
    accelInput{ 0.0, 0.0, 0.0},
    generator(),
    timeStep( timestep_in),
    driftRateInSign{0,0,0},
    random_non_negative_number(0.0, 1.0) {
}


/***************************************************************************************************
Method: activate
Purpose: Activates submodel for use in AttitudeManager
    This applies the last prescribedAng and body angular rate values stored to the attitude. 
NOTE:
    - When smoothing, following a maneuver, the body angular rate will be close to zero. 
    Consequently, the vehicle might take some time to reach a deadband limit. This is a desired behavior.
    - When not smoothing, following a maneuver, the last value stored is the maximum maneuver rate,
    taking here as the initial rate for deadbanding
***************************************************************************************************/
void PerturbModel::activate() {
    if (!initialized) {return;}
    active = true;
    // Check current enforceLatVel flag and rescale driftRateMax if enforced.
    set_enforce_lat_vel(enforceLatVel);

    // Determine initial acceleration based on driftRateIn sign if |driftRate| < |driftRateIn|
    for (size_t ii=0; ii<3; ++ii) {
        // Initialize driftRate variables
        // Provides a random value between 0 and 1 if we are using random-rates and a fixed value of 1 otherwise.
        double rate_coef = 1 + (random_non_negative_number(generator) - 1) * randomRates;

        driftRateInSign[ii] = MathUtils::sign(driftRate[ii]);
        // Using last drift rate value
        driftRateIn[ii] = driftRate[ii];
        driftRateOut[ii] = -driftRateInSign[ii] * rate_coef * driftRateMax[ii];
        // Set acceleration to zero as the initial phase
        accelInput[ii] = 0.0;
    }
    // Checks conditions for smoothing and compute acceleration region if met
    // If not smoothing, ignore that part 
    set_smoothing(smoothing);
    // Update the initial vehicle attitude to prescribed attitude
    jeod::Quaternion delta_quat;
    jeod::Orientation::compute_quaternion_from_euler_angles( jeod::Orientation::RollPitchYaw,
                                                       prescribedAng,
                                                       delta_quat);
    quatBodyWrtRef.multiply_left(delta_quat);

}

/***************************************************************************************************
Method: deactivate
Purpose: Deactivates submodel
    Resets prescribed angle value to zero to guarantee continuous attitude if subscribed again later.
***************************************************************************************************/
void PerturbModel::deactivate() {
    active = false;
    jeod::Vector3::initialize(prescribedAng);
}

/***************************************************************************************************
Method: initialize
Purpose: Converts inputs and initializes the perturbed state.
***************************************************************************************************/
void PerturbModel::initialize() {
    if (initialized) {return;}

    generator.seed( randSeed );
    // Assign the initial rotational elements of the prescribed deadbanding[ii] attitude
    for (unsigned int ii = 0; ii < 3; ++ii) {
        // Protect against negative user inputs
        driftRateMax[ii] = std::abs(driftRateMaxInput[ii]);
        driftDeadband[ii] = std::abs(driftDeadbandInput[ii]);
        maxDeltaLatVelDP[ii] = std::abs(maxDeltaLatVelDPInput[ii]);
        deadbandAccel[ii] = std::abs(deadbandAccelInput[ii]);
        prescribedAng[ii] = initPhase[ii];
    }
    initialized = true;
}

/***************************************************************************************************
Method: update
Purpose: Updates and applies the rotation of the body.
NOTE:
    - In this model, driftRate corresponds to the Euler angles rates. The prescribed angles are updated
    independently using driftRate, which is a valid assumption for small angles. Indeed, in this case,
    the Euler axes in the transformation sequence can be considered independent.
***************************************************************************************************/
void PerturbModel::update() {
    if (!active) {return;}
    if (smoothing) {
        compute_acceleration();
    } else {
        compute_drift_rate();
    }
    // Update for logging purposes
    jeod::Vector3::scale_incr(driftRate, timeStep, prescribedAng);

    // Update drift rate
    jeod::Vector3::scale_incr(accelInput, timeStep, driftRate);
}

/***************************************************************************************************
Method: compute_acceleration
Purpose: Computes acceleration sign based acceleration regions and current acceleration phase.
    For better understanding, use diagram in docs:
    The vehicle is a state-machine with three possible states:
        (1) Free drift
        (2) Decelerating
        (3) Accelerating
This method checks the conditions whether the vehicle needs to leave its current state or not.
    STATE (1-FREE):
        To (2-DECEL): only considered if target driftRate (driftRateIn) is positive
                    CONDITION: theta > theta_db - accel_region
        To (3-ACCEL): only considered if target driftRate (driftRateIn) is negative
                    CONDITION: theta < - (theta_db - accel_region)

    STATE (2-DECEL):
        To (3-ACCEL): 
                    CONDITION: theta < -(theta_db - accel_region)
        To (1-FREE): happens when the target driftRateIn is reached.
                This only makes sense for driftRateIn < 0, which was guaranteed when we started decelerating 
                    CONDITION: driftRate <= driftRateIn (effectively < 0)

    STATE (3-ACCEL):
        To (2-DECEL): 
                    CONDITION: theta > theta_db - accel_region
        To (1-FREE): happens when the target driftRateIn is reached.
                This only makes sense for driftRateIn > 0, which was guaranteed when we started accelerating 
                    CONDITION: driftRate >= driftRateIn (effectively > 0)

    Since (2-DECEL) and (3-ACCEL) conditions and properties are symmetric, we can merge them into one
    if/else statement.
***************************************************************************************************/
void
PerturbModel::compute_acceleration() 
{
    for (unsigned int ii = 0; ii < 3; ++ii) {
        if ((prescribedAng[ii] > driftDeadband[ii] - accelRegion[ii] && driftRateIn[ii] > 0) || // To (2-DECEL)
        (prescribedAng[ii] < -(driftDeadband[ii] - accelRegion[ii]) && driftRateIn[ii] < 0)){   // To (3-ACCEL)
            // The vehicle attitude approaches the current deadband limit.
            // Update free drift rate to move away from current deadband limit and towards the opposite one
            driftRateIn[ii] = driftRateOut[ii];
            driftRateInSign[ii] = MathUtils::sign(driftRateIn[ii]);
            // Accelerate to the new driftRateIn
            accelInput[ii] = driftRateInSign[ii] * deadbandAccel[ii];
            /* If randomizing rates (randomRates = true):
                Generate a random number in [0,1] and rescale the next driftRate 
                -> rate_coef = random_non_negative_number(generator) 
            Else (randomRates = false):
                -> rate_coef = 1.0 */
            double rate_coef = 1 + (random_non_negative_number(generator) - 1) * randomRates;
            // Compute desired drift rate to move away from the next deadband limit
            driftRateOut[ii] = -rate_coef * driftRateInSign[ii] * driftRateMax[ii];

            // Update acceleration region for the next upcoming deadband limit (effectively doing so when randomizing rates)
            accelRegion[ii] = MathUtils::divide_protected( 
                0.5*driftRateIn[ii]*driftRateIn[ii],
                            deadbandAccel[ii]);
           
        } else if (driftRateInSign[ii] * driftRate[ii] >= driftRateInSign[ii] * driftRateIn[ii]){ // To (1-FREE)
            // Set acceleration to zero
            accelInput[ii] = 0.0;
        }
    }
}

/***************************************************************************************************
Method: compute_drift_rate
Purpose: Computes drift rates whenever the deadband limit is reached when not smoothing
***************************************************************************************************/
void 
PerturbModel::compute_drift_rate() 
{
    for (unsigned int ii = 0; ii < 3; ++ii) {
        /* Check if current angle is outisde of deadband region 
        The sign of current driftRateIn determines which deadband limit is being considered:
        1) driftRateIn > 0
            Positive deadband limit
        2) driftRateIn < 0
            Negative deadband limit */
        if  ( driftRateInSign[ii]*(prescribedAng[ii]) > driftDeadband[ii]) {
            // Switch to the next deadbanding drift rate
            driftRateIn[ii] = driftRateOut[ii];
            driftRateInSign[ii] = MathUtils::sign(driftRateIn[ii]);
            driftRate[ii] = driftRateIn[ii];
            /* If randomizing rates (randomRates = true):
                Generate a random number in [0,1] and rescale the next driftRate 
                -> rate_coef = random_non_negative_number(generator) 
            Else (randomRates = false):
                -> rate_coef = 1.0 */
            double rate_coef = 1 + (random_non_negative_number(generator) - 1) * randomRates;
            /* Compute next driftRate after current phase: 
                w_out = (opposite sign to current driftRateIn) * driftRateMax */
            driftRateOut[ii] = -driftRateInSign[ii] * rate_coef * driftRateMax[ii];
        }
    }
}

/***************************************************************************************************
Method: set_smoothing
Purpose: Setter method to change smoothing flag.
This can be called during a run to turn on/off smoothing or before activation
***************************************************************************************************/
void 
PerturbModel::set_smoothing(bool flag_in)
{
    if (!active) { smoothing = flag_in; return;}
    if (!flag_in) { smoothing = false; return;}

    smoothing = true;
    for (size_t ii=0; ii<3; ii++) {
        // Protect against zero deadband accelerations if deadband smoothing is to be utilized
        if (deadbandAccel[ii] <= 0.0) {
            smoothing = false;
            CMLMessage::warn(__FILE__, __LINE__, "Improper deadbandAccel value.\n"
                "Deadband smoothing has been turned off.\n");
            return;
        } else if ( driftDeadband[ii] < MathUtils::divide_protected( 
                        0.5*driftRateMax[ii]*driftRateMax[ii],
                                            deadbandAccel[ii]) ) {
        // Compute the acceleration region for smoothed rates.
        /* Acceleration and deadband limit define the maximum drift rate the vehicle can
        have to stay within the deadband limits with smooth transitions. In a phase plane,
        the curves that drive theta to the deadband limits are:
            theta = theta_db - 1/(2a)*w^2
        The maximum driftRate allowed is such that theta = 0 = theta_db - 1/(2a)*w_lim^2
            w_lim = +/- sqrt(2a * theta_db)
        Otherwise, the vehicle is leaving the deadband region (see docs).
        If current driftRateMax makes the acceleration region too large, 
        overwrite it to maximum value allowed.*/
            CMLMessage::warn(__FILE__, __LINE__, "Invalid configuration.\n",
                "Provided acceleration, driftRateMax and deadband limit is unstable.\n"
                "Resetting driftRateMax to maximum value allowed");
            driftRateMax[ii] = MathUtils::sqrt_protected(2*driftDeadband[ii]*deadbandAccel[ii]);
        }
        // Set acceleration region if smoothing
        accelRegion[ii] = MathUtils::divide_protected( 0.5*driftRateIn[ii]*driftRateIn[ii],
                    deadbandAccel[ii]);
    }
}

/***************************************************************************************************
Method: set_enforce_lat_vel
Purpose: Setter method to change enforceLatVel flag.
If called during a run, this will use the latest values of docking port name and driftRateMax
to compute the moment-arm and rescale the angular rates.
NOTE:
    The velocity requirements are assumed to be fixed throughout the run.
***************************************************************************************************/
void 
PerturbModel::set_enforce_lat_vel(bool flag_in)
{
    // If submodel not active yet, only set the flag and leave the rest to submodel activation
    if (!active) { enforceLatVel = flag_in; return;}
    if (flag_in) { 
        enforceLatVel = true;
        lat_vel_check();
    } else {
        // Reset driftRateMax to initial (or updated) driftRateMax user-input
        enforceLatVel = false;
        for (size_t ii=0; ii<3; ii++) {
            driftRateMax[ii] = std::abs(driftRateMaxInput[ii]);
        }
    }
}


/***************************************************************************************************
Method: lat_vel_check
Purpose: Ensure the Body frame rates can only generate change in lateral velocity at docking port that 
meet predefined requirements defined in DP frame. If not, scale down the maximum drift rates accordingly.
***************************************************************************************************/
void PerturbModel::lat_vel_check() {
    const jeod::MassPoint * mass_point = body.mass.find_mass_point(portName);
    // Checks whether the provided docking port frame exists or not.
    if (mass_point) {
    mass_point->compute_relative_state(body.mass.structure_point,DP_wrt_STR);
        // Generate the transformation matrices between the vehicle Body and Docking Port frames
        jeod::Matrix3x3::product_right_transpose( body.mass.composite_properties.T_parent_this,
                                            DP_wrt_STR.T_parent_this,
                                            T_DP_BODY);
        generate_moment_arms();
    } else { // If not, turn off flag.
        enforceLatVel = false;
        CMLMessage::warn(__FILE__, __LINE__, "Incorrect Port Name\n"
            "Could not find port ", portName, ".\n",
            "Turning lateral velocity requirements off.\n");
        return;
    }

    /* TODO: (A.Masset 05/2026)
    Rescaling angular rate components in the docking port frame to meet some velocity requirements
    and mapping those maximum values back to the vehicle body frame is not a trivial mathematical
    problem. To largely simply this problem at the time of this implementation, we make three 
    important assumptions:
        1) The moment-arm is mainly about the docking port x-axis (Rx >> Ry,Rz).
        2) DP frame and Body frame share the same x-axis and are rotated wrt one another only in 90deg
        rotations about that axis. This assumption is not tested here.
        3) The moment-arm remains constant as long as this submodel is activated.
        Note that deactivating and reactivating the submodel recomputes the moment-arm
    Although very limiting, these assumptions allow lateral velocity requirements for short-term useful 
    cases. */
    // Rx > 100*Rz and 100*Ry is arbitrary but sufficient for current scenarios
    if (std::abs(momentArmDP[0]) < 100 * std::abs(momentArmDP[1]) || 
    std::abs(momentArmDP[0]) < 100 * std::abs(momentArmDP[2])) {
        CMLMessage::warn(__FILE__, __LINE__, "Moment-arm does not meet submodel assumptions.\n"
        "Cannot properly enforce lateral velocity rquirements.\n",
        "Turning lateral velocity requirements off.\n");
        enforceLatVel = false;
    }

    // Transform body drift rates to Docking Port frame
    double driftRateMaxDP[3];
    jeod::Vector3::transform_transpose( T_DP_BODY,
                                  driftRateMax,
                                  driftRateMaxDP);
    // w_max x r in the Docking Port frame
    jeod::Vector3::cross( driftRateMaxDP,
                    momentArmDP,
                    deltaLatVelDP);
    // v_max = 2 w_max x r
    jeod::Vector3::scale(2.0, deltaLatVelDP);

    // Check if y-axis velocity requirement is met
    if (std::abs(deltaLatVelDP[1]) > maxDeltaLatVelDP[1]) {
        driftRateMaxDP[2] = MathUtils::sign(deltaLatVelDP[1])*maxDeltaLatVelDP[1]/(2*momentArmDP[0]);
        deltaLatVelDP[1] = maxDeltaLatVelDP[1];
    }
    // Check if z-axis velocity requirement is met
    if (std::abs(deltaLatVelDP[2]) > maxDeltaLatVelDP[2]) {    
        driftRateMaxDP[1] = -MathUtils::sign(deltaLatVelDP[2])*maxDeltaLatVelDP[2]/(2*momentArmDP[0]);
        deltaLatVelDP[2] = maxDeltaLatVelDP[2];
    }
    jeod::Vector3::transform( T_DP_BODY,
                        driftRateMaxDP,
                        driftRateMax);
}


/***************************************************************************************************
Method: generate_moment_arms
Purpose: Generate the moment arms between the vehicle CoM and Docking Port frames
***************************************************************************************************/
void PerturbModel::generate_moment_arms() {
    double momentArmSTR[3];
    jeod::Vector3::diff( DP_wrt_STR.position,
                   body.mass.composite_properties.position,
                   momentArmSTR);
    jeod::Vector3::transform( DP_wrt_STR.T_parent_this,
                        momentArmSTR,
                        momentArmDP);
    if (!MathUtils::has_changed_from(momentArmDP[0], 0.0)) {
        CMLMessage::warn(__FILE__, __LINE__, "Selected port results in moment-arm with zero x-component.\n"
                "Deactivating lateral velocity requirements enforcement.\n");
        enforceLatVel = false;
    }
}