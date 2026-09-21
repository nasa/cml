/***************************************************************************************************
PURPOSE:
 (Forced motion / fixed perturbectory model)

ASSUMPTIONS:
 ((Small prescribed angles are added to a vehicle body in terms of Euler angle 'deltas')
  (This is used to create an apparent 'perturbation' motion for simulation purposes only)
  (Does not compute exact dynamics)
  (Enforces lateral velocity at docking port requirements if desired))

Library dependency:
 ((../src/perturb_model.cc))

PROGRAMMERS:
 ((Daniele Tancredi) (OSR) (08/18)     (Initial Implementation))
 ((Daniele Tancredi) (OSR) (03/20)     (Added motion generation on the fly))
 ((Daniele Tancredi) (OSR) (12/20)     (S-LVLH prescribed attitude))
 ((Jeb Stefan)       (OSR) (01/21)     (Prescribed vehicle maneuvers))
 ((Jeb Stefan)       (OSR) (04/21)     (Updated model functionality to meet ICD requirements))
 ((Jeb Stefan)       (OSR) (07/21)     (Initial merge into Artemis-II integration))
 ((Jeb Stefan)       (EG6) (11/23)     (Addition of Target Vehicle Accels and secondary att updates))
 ((Jeb Stefan)       (EG6) (06/25)     (Removing Target Vehicle Accels capability))
 ((Alexandre Masset) (OSR) (10/25)     (Refactor, moving maneuvers to another class))
***************************************************************************************************/

#ifndef CML_PERTURB_MODEL_HH
#define CML_PERTURB_MODEL_HH

#include<string>
#include<random>

// CML headers
#include "cml/models/utilities/cml_message/include/cml_message.hh"

// JEOD headers
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh" // jeod::DynBody
#include "jeod/models/dynamics/mass/include/mass_point_state.hh" // MassPointState
#include "jeod/models/utils/quaternion/include/quat.hh" // Quaternion

/***************************************************************************************************
PerturbModel: Model which applies perturbations to the current state of a simulation vehicle; has
              ability to mimic deadbanding and/or hold a vehicle centric LVLH frame
***************************************************************************************************/
class PerturbModel {
/***************************************************************************************************
External models references (required):
***************************************************************************************************/
protected:
    jeod::DynBody & body; /* (--) Perturbing vehicle body sim object */
    bool initialized; /* (--) Flag when model has been initialized */

    /***************************************************************************************************
Input file settings:
    NOTE: All rotational inputs are RPY and translational inputs are XYZ
***************************************************************************************************/
public:
    bool randomRates; /* (--) 
        Choice to randomize the new angular rate after each deadband transition */
    std::string portName; /* (--) 
        Name of vehicle mass point driving the lateral velocity requirements */
    double driftDeadbandInput[3]; /* (rad) 
        Deadband within which the Body frame will oscillate */
    int randSeed; /* (--) 
        Seed value for srand() used in all randomizations; if using same seed and initial conditions
        between two randomized runs, the resulting prescribed motion will be the same; varying seed 
        to be utilized by Monte Carlo capability to achieve varying prescribed rotations */
    double initPhase[3]; /* (rad) 
        Initial perturbed angular phase */
    double maxDeltaLatVelDPInput[3]; /* (m/s)
        Maximum lateral velocity change of docking port for deadband transition; difference in 
        docking port lateral velocity approaching and leaving deadband limits are enforced to be 
        less than this value; limits expressed in the Docking Port frame; not utilized if not enforceLatVel */
    double driftRateMaxInput[3]; /* (rad/s) 
        Limit on Body frame rotation rate when inside deadbands */
    double deadbandAccelInput[3]; /* (rad/s2) 
        Constant angular acceleration when smoothing deadband transitions; not utilized if not smoothing */

/***************************************************************************************************
Model specific variables:
***************************************************************************************************/
protected:
    bool active; /* (--) 
        Flag; if submodel is active */
    jeod::Quaternion & quatBodyWrtRef; /* (--)
        Reference to attitude quaternion from Ref to Body frame updated for use in the 
        attitude manager model*/
    double prescribedAng[3];/* (rad)
        Current presribed vehicle attitude angles (RPY) with respect to current simulation vehicle attitude */
    double (& driftRate)[3]; /* (rad/s)
        Current Euler angle rotation rate which is utilized to propagate the prescribed angle in the 
        current model call; updates to vehicle rotation rates occur after prescribed angles are applied
        so they do not result in additional vehicle rotations as vehicle state is propagated.  */
    double momentArmDP[3]; /* (m)
        Moment arm between Body frame and Docking Port frame specified in Docking Port frame */
    double T_DP_BODY[3][3]; /* (--)
        Simulation provided transformation matrix from Docking Port to Body frame */
    double deltaLatVelDP[3]; /* (m/s)
        Difference in lateral velocity of the Docking Port frame at the next deadband transition */
    double driftRateIn[3]; /* (rad/s)
        Free drift rate that takes the vehicle to the incoming deadband limit. 
        It corresponds to the desired free drift rate during a deadbanding phase. */
    double driftRateOut[3]; /* (rad/s)
        Free drift rate that will take the vehicle outward from the current deadband limit. 
        It corresponds to the next desired free drift rate for the next deadbanding phase. */
    jeod::MassPointState DP_wrt_STR; /* (--)
        Docking Port frame information with respect to the Stuctural frame */
    double driftDeadband[3]; /* (rad)
        Protected version of driftDeadbandInput ensuring it is positive */
    double maxDeltaLatVelDP[3]; /* (m/s)
        Protected version of maxDeltaLatVelDPInput ensuring it is positive */
    double driftRateMax[3]; /* (rad/s)
        Protected version of driftRateMaxInput ensuring it is positive */
    bool smoothing; /* (--) 
        Protected; Choice to apply smoothing during deadband */
    bool enforceLatVel; /* (--)
        Protected; Choice to enforce the lateral velocity velocity requirements of latVelDPMax */
    double deadbandAccel[3]; /* (rad/s2)
        Protected version of deadbandAccelInput ensuring it is positive */
    double accelRegion[3]; /* (rad)
        Angular switch when the vehicle should accelerate/decelerate */
    double accelInput[3]; /* (rad/s2)
        Acceleration input to update driftRate */
    std::mt19937 generator; /* (--)
        Random Engine Generator using Mersenne-Twister */
    const double & timeStep; /* (s)
        Time step of the model. Provived by the attitude manager. */
    /* TODO A.Masset 26/03: 
        The flags below could be changed to an enumeration if one wants to add more drift modes */
    int driftRateInSign[3]; /* (--)
        Array of driftRateIn components signs */

private:
#ifndef SWIG
    std::uniform_real_distribution<double> random_non_negative_number; /* (--)
        Random number used to generate random rates between 0 and maxDriftRate when randomRates is used*/ 
#endif // end ifndef SWIG

/***************************************************************************************************
Constructor/Destructor
***************************************************************************************************/
public:
    // Constructor
    PerturbModel( const double & timestep_in,
                  jeod::DynBody & body,
                  jeod::Quaternion & quatBodyWrtRef_out,
                  double (& w_BodyToRef_Body_out)[3]);
    //Destructor
    ~PerturbModel(void) {}
    void initialize();
    void update();

    void activate();
    void deactivate();
    void set_enforce_lat_vel(bool flag_in);
    void set_smoothing(bool flag_in);

/***************************************************************************************************
Model Functions
***************************************************************************************************/
protected:
    void compute_acceleration();
    void compute_drift_rate();
    void lat_vel_check();
    void generate_moment_arms();

private:
    // Not implemented:
    PerturbModel(const PerturbModel& rhs);
    PerturbModel & operator =(const PerturbModel& rhs);
};
#endif