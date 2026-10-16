/***************************************************************************************************
PURPOSE:
 (Provide a manager class to apply perturbation and/or maneuvers to a vehicle body.
  The perturbations are calculated in the PerturbModel class. The maneuvers are calculated
  in the ManeuverModel.)

ASSUMPTIONS:
 ((This model does not compute exact dynamics. It applies prescribed kinematics onto a
   vehicle body. The body angular velocity and attitude does not represent an actual behavior of 
   a vehicle but are a sufficient approximation for the purpose of this model.)
  (The attitude and angular velocity of the vehicle are calculated with respect to the inertial 
   frame.))

LIBRARY DEPENDENCY:
 ((../src/attitude_manager.cc))

PROGRAMMERS:
 ((Alexandre Masset) (OSR) (10/25)     (Initial Implementation))
***************************************************************************************************/

#ifndef CML_ATTITUDE_MANAGER_HH
#define CML_ATTITUDE_MANAGER_HH

// JEOD headers
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh" // jeod::DynBody
#include "jeod/models/utils/quaternion/include/quat.hh" // Quaternion
#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh" // RefFrameState

//CML headers
#include "cml/models/utilities/subscriptions/include/subscriptions.hh" // SubscriptionBase
#include "cml/models/utilities/cml_message/include/cml_message.hh" // Publishing messages

#include "maneuver_model.hh"
#include "perturb_model.hh"

/***************************************************************************************************
AttitudeManager: This class is instantiated to apply perturbations and/or maneuvers onto a
vehicle body. It comprises angular velocities and attitudes provided by perturbation and maneuver
model instances.
***************************************************************************************************/
class AttitudeManager: public SubscriptionBase {
public:
    enum ATT_MNGR_CONF
    {
        NONE  = 0,     // No actions
        PERT = 1,      // Perturbation subscribed
        MNVR = 2,      // Maneuver subscribed
        MNVR_PERT = 3  /* Perturbation and Maneuver subscribed, with maneuver taking precedence and 
                    perturbations on hold */
    };
/***************************************************************************************************
Attributes
***************************************************************************************************/
protected:
    const double & dynamicTime; /* (s) 
        Current simulation time */
    jeod::DynBody & body; /* (--)  
        Perturbed vehicle body reference */
    jeod::Quaternion quatBodyWrtRef; /* (--)
        Attitude quaternion of a provided Reference Frame to the body frame.*/
    double dynamicTimePrevious; /* (s) 
        Previous simulation time. Used to calculate the simulation time step of this model. */
    double timeStep; /* (s) 
        Computed delta-time since the previous call, accessed by perturbation and maneuver submodels */
    jeod::RefFrameState * RefWrtInertialFrameState; /* (--) 
        Pointer to a reference frame state. Used if the vehicle is fixed to a specific reference 
        frame (ex. S-LVLH). If NULL, default to inertial */
    double w_BodyWrtRef_Body[3]; /* (rad/s) 
        Angular velocity of body frame wrt a given reference frame expressed in the body frame.*/
    double w_RefWrtInertial_Body[3]; /* (rad/s) 
        Angular velocity of a given reference frame wrt the Inertial frame expressed in the
        the Body frame.*/
    ATT_MNGR_CONF config; /* (--) 
        Enumeration to track the attitude manager subscribed submodels. */
    bool rotational_dynamics_prev; /* (--) 
        Flag to store previous value of body.rotational_dynamics before this model was activated.
        Resumes previous state when the model is deactivated.*/

public:
    PerturbModel  perturbation; /* (--) 
        Pertubation model instance */
    ManeuverModel maneuver; /* (--)
        Maneuver model instance */


/***************************************************************************************************
Methods
***************************************************************************************************/
public:
    // Constructor
    AttitudeManager( const double & time_in,
                  jeod::DynBody & body_in);

    void initialize() override;
    void update();
    
    void subscribe_perturbation();
    void unsubscribe_perturbation();      
    void subscribe_maneuver();
    void unsubscribe_maneuver();          
    void set_ref_frame_state( jeod::RefFrameState * ref_frame_state);

    //Destructor
    ~AttitudeManager() override {}

protected:
    void update_attitude();
    void update_ang_vel();
    void activate() override;
    void deactivate() override;

private:
    // Not implemented:
    AttitudeManager(const AttitudeManager& rhs);
    AttitudeManager & operator =(const AttitudeManager& rhs);
};
#endif