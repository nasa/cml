/***************************************************************************************************
PURPOSE:
 (Class providing methods for maneuvers)

ASSUMPTIONS:
 ((The target vehicle slews between two attitude quaternions using Euler axis rotation vector and angle
   between these two. The maximum maneuver rate and acceleration (in magnitude) are provided as inputs. 
   The maneuver duration is determined by these given maximum maneuver rate and acceleration.))

LIBRARY DEPENDENCY:
 ((../src/maneuver_model.cc))

PROGRAMMERS:
 ((Alexandre Masset) (OSR) (10/25)     (Initial Implementation))
***************************************************************************************************/

#ifndef CML_MANEUVER_MODEL_HH
#define CML_MANEUVER_MODEL_HH

// CML headers
#include "cml/models/utilities/cml_message/include/cml_message.hh"

// JEOD headers
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh" // jeod::DynBody
#include "jeod/models/utils/quaternion/include/quat.hh" // Quaternion


/***************************************************************************************************
ManeuverModel: This class provides methods to compute a slew maneuver of a vehicle between
two attitude quaternions. These attitude quaternions represent the attitude from the body frame of 
the vehicle to a given reference frame Ref.
***************************************************************************************************/
class ManeuverModel {
protected:
    jeod::DynBody & body; /* (--) 
        Perturbing vehicle body sim object */
    bool initialized; /* (--) 
        Flag; if the model has been initialized. */
    bool active; /* (--) 
        Flag; if the model is currently active (updated). */
    const double & timeStep; /* (s) 
        Time step of the model. Provided by the attitude manager. */
    double (&mnvrRate)[3]; /* (rad/s) 
        Maneuver rate. It corresponds to the angular velocity of the body frame with respect to 
        the given reference frame Ref, expressed in thebody reference frame. */
    double mnvrRateMax; /* (rad/s) 
        Protected version of the maximum maneuver rate (in magnitude). */
    double mnvrAccel; /* (rad/s2) 
        Protected version of the maneuver acceleration (in magnitude). */
    double mnvrRateUnit[3]; /* (--) 
        Protected version of the maneuver rate unit vector. */
    double eulerTarget; /* (rad) 
        Computed Euler angle between the initial attitude quaternion and the 
        desired attitude quaternion. */
    double currentEuler; /* (rad) 
        Current Euler angle between the current attitude quaternion and the 
        desired attitude quaternion. */
    double ang_vel_mag; /* (rad/s) 
        Protected angular rate magnitude used during maneuver. */
    jeod::Quaternion quatTargetNominalWrtRef; /* (--) 
        Desired attitude quaternion. It represents an attitude from the nominal body frame to 
        the given reference frame Ref. "Nominal" refers here to the body frame with no perturbation. */
    bool maneuvering; /* (--) 
        Flag; ready to maneuver to desired target. Used if smoothing to know the model needs to drive 
        the rates to zero before starting the maneuver to target. */
    double switchThreshold; /* (rad) 
        During maneuver, angle at which the vehicle shall start decelerating to reach desired attitude
        Euler angle. Only updated if smoothing. */
    jeod::Quaternion & quatBodyWrtRef; /* (--) 
        Computed attitude quaternion from the body frame to the reference frame Ref.
        Used in the attitude manager to update the vehicle attitude. */
    bool smoothing; /* (--) 
        Flag; If smoothing the maneuver rate. */
    double targetRate; /* (rad/s) 
        Target rate desired at the end of the maneuver. This is a randomized value generated based on
        the user-defined tolerance that will be added to the angular rate magnitude. */

/***************************************************************************************************
Input file settings:
***************************************************************************************************/
public:
    double mnvrRateMaxInput; /* (rad/s) 
        Input variable for the maximum maneuver rate (in magnitude) */
    double mnvrAccelInput; /* (rad/s) 
        Input variable for the maneuver acceleration (in magnitude) if smoothing */
    int randSeed; /* (--)
        Randomization seed to generate noise about final target state */
    double rateTolerance; /* (--) 
        Dimensionless rate error, added to maneuver axis and final target attitude rate.
        Example: 0.1 is equivalent to adding noise in a +/- 10% range of the maximum maneuver rate */
    double eulerTolerance; /* (--) 
        Dimensionless angle error, added to targeted Euler angle 
        Example: 0.1 is equivalent to adding noise in a +/- 10% range around the initial target */

/***************************************************************************************************
Constructor/Destructor
***************************************************************************************************/
public:
    // Constructor
    ManeuverModel( const double & timestep_in,
                  jeod::DynBody & body,
                  jeod::Quaternion & quatBodyWrtRef_out,
                  double (& w_BodyToRef_Body_out)[3]);
                  
    void initialize();
    void update();

    void activate();
    void set_target_attitude( jeod::Quaternion q_target);
    void compute_eigen_rot();

    void deactivate()            { active = false; maneuvering = false;}
    inline bool is_active()             { return active;}
    void set_smoothing(bool flag_in);
    
    //Destructor
    ~ManeuverModel() {}

protected:
    void compute_smoothed_rate();
    void drive_to_zero();
    bool is_maneuver_ready();

private:
    // Not implemented:
    ManeuverModel(const ManeuverModel& rhs);
    ManeuverModel & operator =(const ManeuverModel& rhs);
};
#endif