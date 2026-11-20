/*******************************TRICK HEADER******************************
PURPOSE: (Wrapper Header for defining and applying vent disturbances to candidate vehicle)

LIBRARY DEPENDENCY:
  (../src/two_level_vent.cc)
  (../src/two_level_vent_set.cc)

PROGRAMMERS:
  (((Suyog Benegalrao & Alexandre Masset) (JSC) (Jul 2025) (Antares) (initial version)))
 ************************************************************************/

#ifndef CML_TWO_LEVEL_VENT_HH
#define CML_TWO_LEVEL_VENT_HH

#include "cml/models/fhw/effectors/vent/include/simple_vent.hh"
#include "cml/models/vehicle_management/events_manager/include/event_action_base.hh"

#include <random> /* need for std::mt19937 */

// Forward Declaration
class TwoLevelVent;
class TwoLevelVentSet;


/***************************************************************************************************
Class TwoLevelVentReset:
    EventActionBase extension. It allows an event to reset the vent schedule, putting it in conditions
    to start a high mode firing (by setting it to low mode)
***************************************************************************************************/
class TwoLevelVentReset : public EventActionBase {
 protected:
  TwoLevelVent * two_level_vent;
 public:
  explicit TwoLevelVentReset(TwoLevelVent * two_level_vent_) :
  two_level_vent(two_level_vent_) {}
  bool specific_execution() override;
 private:
  TwoLevelVentReset (const TwoLevelVentReset& rhs);
  TwoLevelVentReset& operator = (const TwoLevelVentReset& rhs);
};


/***************************************************************************************************
Class TwoLevelVent:
    SimpleVent extension to include two venting modes to meet specific user-defined 
    requirements. The model has a set of parameters that can be specifically used for
    Monte Carlo purposes.
***************************************************************************************************/
class TwoLevelVent : public SimpleVent
{
  friend class TwoLevelVentReset; // To access protected method reset()
  friend class TwoLevelVentSet; // To access protected method TwoLevelVent parameters
    public:

    /* MC Parameters */
    double     wait_time_min;               /* (s) 
        parameter setting for minimum wait time between start of vent high mode firings */
    double     wait_time_max;               /* (s) 
        parameter setting for maximum wait time between start of vent high mode firings */
    double     high_duration_min;         /* (s) 
        parameter setting for minimum duration of each vent high mode firing */
    double     high_duration_max;         /* (s) 
        parameter setting for maximum duration of each vent high mode firing */
    bool       wait_time_randFlag;          /* (s) 
        parameter flag where TRUE sets high mode wait time to a random number */
    bool       high_duration_randFlag;    /* (--) 
        parameter flag where TRUE sets high mode duration time to a random number */

    /* Two-Level Vent Components */
    double     accel_mag_high;    /* (m/s2) 
        magnitude of high vent firing acceleration */
    double     accel_mag_low;   /* (m/s^2) 
        magnitude of low vent firing acceleration */
    double     high_duration;     /* (s) 
        parameter setting for high mode duration when dur_randFlag is set FALSE */
    double     high_wait_time;    /* (s) 
        parameter setting for high mode wait time when wait_time_randFlag is set FALSE */
    double     dv_max;              /* (m/s) 
        parameter for maximum DV allowed for each vent high mode firing */
    TwoLevelVentReset action_reset; /* (--) 
        Event-action to reset the vent configuration on call */

    protected:
    /* Logged Two-Level Vent Variables */
    const double & vehicle_mass; /* (kg) 
        Reference to vehicle mass */ 
    std::mt19937 & gen; /* (--)
        Reference to random number generator */
    double     dv_mag;            /* (m/s) 
        logged DV from each vent high mode firing */
    double     sched_onTime;      /* (s) 
        logged scheduled on-time for each vent high mode firing */
    bool       high_venting;    /* (--) 
        logged type of vent firing */
    double     start_time_high; /* (s) 
        time when the vent started high mode firing */

    public:
    /* Constructor */
    TwoLevelVent( const double& dyn_time_, TwoLevelVentSet& vent_set_);

    protected:
    bool check_configuration() override;
    void stop_venting() override;
    void update_force() override;
    void reset();
    void high_venting_mode();
    void low_venting_mode();

    private:
    /* Copy constructor, operator= declared private and not implemented; class is not copyable. */
    TwoLevelVent(const TwoLevelVent&);
    TwoLevelVent& operator = (const TwoLevelVent&);
};

#endif
