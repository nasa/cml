/***************************************************************************************************
PURPOSE:
 (Class providing methods for vehicle maneuvers)

ASSUMPTIONS:
 ((The vehicle slews between two attitude quaternions using Euler axis rotation vector and angle
   between these two. The maximum maneuver rate and acceleration (in magnitude) are provided as inputs. 
   The maneuver duration is determined by these given maximum maneuver rate and acceleration.))

PROGRAMMERS:
 ((Alexandre Masset) (OSR) (10/25)     (Initial Implementation))
***************************************************************************************************/
#include <cmath> // std::abs()
#include <random> // std::mt19937

// JEOD headers
#include "jeod/models/utils/math/include/vector3.hh"

// CML headers
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/maneuver_model.hh"

ManeuverModel::ManeuverModel( const double & timestep_in,
                            jeod::DynBody & body_in,
                            jeod::Quaternion & quatBodyWrtRef_out,
                            double (& w_BodyToRef_Body_out)[3])
    :
    body( body_in),
    initialized( false),
    active( false),
    timeStep( timestep_in),
    mnvrRate( w_BodyToRef_Body_out),
    mnvrRateMax( 0.0),
    mnvrAccel( 0.0),
    mnvrRateUnit{ 0.0, 0.0, 0.0},
    eulerTarget( 0.0),
    currentEuler( 0.0),
    ang_vel_mag{ 0.0},
    quatTargetNominalWrtRef(),
    maneuvering( false),
    switchThreshold( 0.0),
    quatBodyWrtRef( quatBodyWrtRef_out),
    smoothing( false),
    targetRate( 0.0),
    mnvrRateMaxInput( 0.0),
    mnvrAccelInput( 0.0),
    randSeed( 0),
    rateTolerance( 0.0),
    eulerTolerance( 0.0)
    {}

/***************************************************************************************************
Method: initialize
Purpose: 
    Initialize the inputs and check if smoothing capability can be used if required.
***************************************************************************************************/
void
ManeuverModel::initialize() {
    if (initialized) {return;}

    mnvrRateMax = std::abs(mnvrRateMaxInput);
    mnvrAccel = std::abs(mnvrAccelInput);

    initialized = true;
}

/***************************************************************************************************
Method: activate
Purpose: 
    Activate the model on call. Only works if the model has been initialized
NOTE:
    - This method is called by the higher level subscribe_maneuver() method in AttitudeManager class
***************************************************************************************************/
void
ManeuverModel::activate() {
    if (!initialized) {return;}
    // Check smoothing conditions before active flag is set to true.
    set_smoothing(smoothing);

    active = true;
    ang_vel_mag = jeod::Vector3::vmag(mnvrRate);

    if (smoothing) {
        // If smoothing, check if the maneuver is ready to start, ie. the drift rates are close to zero.
        is_maneuver_ready();
    } else {
        // If not smoothing, the vehicle can directly maneuver to desired attitude
        maneuvering = true;
        compute_eigen_rot();
    }
}

/***************************************************************************************************
Method: set_smoothing
Purpose: Setter method to change smoothing flag.
NOTE:
    This method cannot be called when a maneuver is actively performed. Smoothing implies driving
    the rates to zero first, and then accelerating to mnvrRateMax. Command will be ignored.
***************************************************************************************************/
void 
ManeuverModel::set_smoothing(bool flag_in)
{
    if (!initialized) { smoothing = flag_in; return;}

    // Cannot change flag when the maneuver is actively performed
    if (active && MathUtils::has_changed_from(smoothing, flag_in)) {
        CMLMessage::warn(
            __FILE__,__LINE__,"Method call error\n",
            "Smoothing maneuver requires driving the rates to zero first.\n"
            "Cannot set smoothing flag while maneuver is active.\n"
            "Command ignored.\n");
        return;
    }

    if (!flag_in) { smoothing = false; return;}
    smoothing = true;
    // Protect against zero deadband accelerations if deadband smoothing is to be utilized
    if (!MathUtils::has_changed_from(mnvrAccel, 0.0)) {
        CMLMessage::warn(
            __FILE__,__LINE__,"Configuration error\n",
            "Maneuver acceleration cannot be set to zero if smoothing.\n"
            "Smoothing is turned off.\n");
        smoothing = false;
    }
}

/***************************************************************************************************
Method: update
Purpose: 
    Update call
NOTE:
    - If the vehicle has reached the desired attitude (Euler angle), stop the maneuver
***************************************************************************************************/
void
ManeuverModel::update() {
    if (!active) { return;}
    // Update current Euler angle only if maneuvering to desired target
    if (maneuvering) {currentEuler += ang_vel_mag * timeStep;}
    if (smoothing) {
        // If the vehicle is not in a maneuvering state, the angular velocity needs to be driven to zero
        if (maneuvering) {compute_smoothed_rate();}
        // If angular velocity has not been reset yet, drive it to zero
        else {drive_to_zero();}
    } else {
        // If not smoothing, set rate to its maximum magnitude
        if (ang_vel_mag != mnvrRateMax) {ang_vel_mag = mnvrRateMax;}
        if (currentEuler >= eulerTarget) {
            ang_vel_mag = targetRate;
            deactivate();}
    }
    // Apply angular velocity update for maneuver
    jeod::Vector3::scale(mnvrRateUnit, ang_vel_mag, mnvrRate);
}

/***************************************************************************************************
Method: compute_smoothed_rate
Purpose: 
    Compute the maneuver rate based on current Euler angle to the target attitude when smoothing
    the transitions.
***************************************************************************************************/
void 
ManeuverModel::compute_smoothed_rate()
{
    // If the threshold is reached, start decelerating.
    // The threshold is defined such that the resulting attitude is close to the desired target
    if (currentEuler > switchThreshold) {
        ang_vel_mag -= mnvrAccel*timeStep;
        if (ang_vel_mag < targetRate) {
            /* Maneuver ends when magnitude is negative. Overshoot is possible and desired in that case.
            Although a "negative" magnitude does not have any meaning, we use that negative sign to
            compute the vector in the opposite direction. Small overshooting is desired. If maneuvers
            are coupled with perturbations, those small rates will drive the vehicle to the deadband limit. */ 
            deactivate();
        }

    // If the threshold is NOT reached, keep accelerating.
    } else if (ang_vel_mag < mnvrRateMax) {ang_vel_mag += mnvrAccel*timeStep;}
}

/***************************************************************************************************
Method: drive_to_zero
Purpose: 
    Drives the rates to zero. Used if the initial drift rates are not zero when smoothing.
NOTE:
    - Only used when smoothing the transitions
TODO: (A.Masset 26/03)
    - This can be improved to be more representative of an actual maneuver.
    For instance, we compute and interpolate the angle between the initial drift rate vector and the 
    desired maneuver rate vector, making a spherical interpolation between the two vectors.
    We can also check if the two vectors are aligned or not.
***************************************************************************************************/
void
ManeuverModel::drive_to_zero() {
    // Compute current angular rate vector that shall be driven to zero
    jeod::Vector3::normalize( mnvrRate, mnvrRateUnit);
    // Decelerate to zero
    ang_vel_mag -= mnvrAccel*timeStep;
    // Check if maneuver ready (angular rate close to zero)
    is_maneuver_ready();
}

/***************************************************************************************************
Method: is_maneuver_ready
Purpose: 
    Checks if the rates are zero and update the target Euler angle and switching threshold at that time.
    Used only when smoothing is on. This is called at submodel activation and when driving the rates to
    zero before a maneuver. 
***************************************************************************************************/
bool
ManeuverModel::is_maneuver_ready()
{
    maneuvering = false;

    // This method is used when driving state to zero before a maneuver
    // If the maneuver rate has reached zero, maneuvering can start
    if (MathUtils::is_within_abs_tolerance(
            ang_vel_mag, 0.0, mnvrAccel * timeStep)) {
        // Switch to a maneuvering state
        maneuvering = true;
        compute_eigen_rot();
        
        /* There are two cases for computing the acceleration threshold since the acceleration
        is constant (in magnitude):
            1) the angular velocity at theta/2 is not larger than the maximum velocity:
                start decelerating at theta/2 to reach the desired attitude and a zero angular-rate
            2) the angular velocity at theta/2 is larger than the maximum velocity:
                the vehicle free drifts at the maximum velocity until it reaches the switching angle
                that will drive it to the desired attitude and a zero angular-rate
        The corresponding switching angle is the maximum value between those two cases.
        */
        
        switchThreshold = std::max(
            eulerTarget - MathUtils::divide_protected( 
                0.5*(mnvrRateMax*mnvrRateMax - targetRate*targetRate),
                                mnvrAccel),
            eulerTarget/2
        );
    }
    return maneuvering;
}
 
/***************************************************************************************************
Method: compute_eigen_rot
Purpose: 
    Computes the eigen rotation vector and angle. 
    - The target attitude (post-maneuver) is designated here as "nominal": q_NR or quatTargetNominalWrtRef
    - The current attitude (pre-maneuver) is designated as: q_BR or quatBodyWrtRef
    - The difference between those two attitudes, ie. the slew maneuver to apply is:
        q_NB or quatNominalWrtBody
    Then, the difference is computed as:
    q_NB = q_NR x q_BR^-1
    Here quaternions are already normalized and conjugate is equivalent to inverse quaternion
***************************************************************************************************/
void 
ManeuverModel::compute_eigen_rot()
{
    // Reset current Euler angle
    currentEuler = 0.0;
    // Compute quaternion difference
    jeod::Quaternion quatNominalWrtBody;
    quatTargetNominalWrtRef.multiply_conjugate(quatBodyWrtRef, quatNominalWrtBody);
    // Normalize for sanity
    quatNominalWrtBody.normalize();
    // Transform into an Eigen rotation
    quatNominalWrtBody.left_quat_to_eigen_rotation( &eulerTarget, mnvrRateUnit);

    // Generate randomized values about our desired target state to simulate uncertainty in sim truth
    std::mt19937 generator;
    generator.seed( randSeed );
    std::uniform_real_distribution<double> rand(-1.0, 1.0);
    // Add randomized noise to target Euler angle
    eulerTarget *= (1 + rand(generator) * eulerTolerance);

    // Compute noise on maneuver axis
    double scratch[3];
    for (size_t ii=0; ii<3; ii++) {
        // Generate a different value for each rate component
        scratch[ii] = rand(generator) * rateTolerance * mnvrRateMax;
    }
    // Generate a random noise eps added to the targeted angular rate magnitude at the end of the maneuver
    // The maximum maneuver rate is used as a reference to compute that error 
    targetRate = rand(generator) * rateTolerance * mnvrRateMax;
    // Rescale and normalize new maneuver axis
    jeod::Vector3::scale_incr(mnvrRateUnit, mnvrRateMax, scratch);
    jeod::Vector3::normalize(scratch, mnvrRateUnit);
}

/***************************************************************************************************
Method: set_target_attitude
Purpose: 
    Set the target attitude quaternion the vehicle will maneuver to and resets maneuvering state.
***************************************************************************************************/
void 
ManeuverModel::set_target_attitude( jeod::Quaternion q_target)
{
    q_target.normalize();
    quatTargetNominalWrtRef = q_target;

    // If the vehicle maneuver submodel is already active (either maneuvering or driving state to zero),
    // resets the maneuvering state and target Euler angle.
    if (active) {
        maneuvering = false;
        activate();
    }
}