/*******************************************************************************
PURPOSE: (Allows the specification of a known state at a time other than the
          desired simulation-initialization-time.  This state can be propagated
          to the desired simulation-start-time and then used to initialize the
          simulation.)

ASSUMPTIONS AND LIMITATIONS:
   (( Initial propagation assumes that the primary gravity-body rotates on
      its z-axis, and that 3rd-bodies are static for the duration of the
      "make-up" integration.  Once the sim starts in full, the integration
      will be conducted using normal procedures.
     )
   ( Where possible, the simulation should just be run normally for the
     desired propagation time; any sim may be run in reverse if so desired by
     setting the time-model's scale-factor to a negative value.
     This model is provided for more simulations that are complicated with the
     presence of models that have one or more of the following problems:
     - implicit assumptions about start-time being zero,
     - implicit assumptions about the direction of time,
     - are cued off the simulation-counter rather than the dynamic-time
     - an initialization method that depends on initial state and no method
       to re-initialize when the initial state is truly known
     - an initialization method that relies on some pre-defined configuration,
       defined at the intended simulation start time but which is inconsistent
       with the configuration at the known-state's-time; this is only an issue
       in cases where the wrong configuration will interfere with the
       integration from the known-state's-time to the
       intended-simulation-start-time
     - any sort of sensor-effector feedback mechanism that will be invalidated
       by the integration; this is usually only applicable where it is
       necessary to integrate backward in time.
   )

LIBRARY DEPENDENCIES:
   ((../src/state_initialize_with_propagation.cc)
   )

PROGRAMMERS:
   (((Gary Turner) (OSR) (October 2016) (New))
 ******************************************************************************/

#ifndef STATE_INITIALIZE_INITIAL_PROPAGATION_HH
#define STATE_INITIALIZE_INITIAL_PROPAGATION_HH

#include "jeod/models/dynamics/dyn_manager/include/dyn_manager.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "jeod/models/environment/gravity/include/gravity_manager.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "state_initialize.hh"

class StateInitializeWithPropagation : public StateInitialize
{
 protected:
  jeod::GravityManager & gravity_manager; /* (--)
       Reference to the gravity-model used during the propagation. */
  jeod::DynBody * body; /* (--)
       Internally-set pointer to the jeod::DynBody identified by the
       inherited variable "subject" (inherited from jeod::BodyAction). */

  double T_initial_to_current[3][3];      /* (--)
       Direct transform from initial pfix to intermediate RK pfix frame. */

  double T_inrtl_to_initial_pfix[3][3];   /* (--)
       Copy of the initial (spec-time) inertial->pfix transformation matrix */
  double omega;            /* (rad/s) Angular velocity of planet on z-axis */

 public:
  double propagation_time; /* (s)
      The time over which the state will be propagated between the initial
      spec. state and the start of the integration.*/
  double time_step; /* (s)
     The approximate time-step used to complete the propagation.  This value
     may be adjusted internally such that propagation_time is an integer
     multiple of this value.*/
  bool   apply_dispersions_before_propagation;/* (--)
     Flag to indicate that state dispersions are to be applied prior to
     propagation.  Default true.*/

  explicit StateInitializeWithPropagation( jeod::GravityManager & gravity_manager);
  virtual ~StateInitializeWithPropagation(){};

  virtual void initialize(  jeod::DynManager & dyn_manager ) override;
  virtual void apply(  jeod::DynManager & dyn_manager ) override;

 protected:
  void propagate_state();
  void rk4_integration();
  void compute_planet_orientation (double time);

 private:
  StateInitializeWithPropagation (const StateInitializeWithPropagation&);
  StateInitializeWithPropagation & operator =
                                 (const StateInitializeWithPropagation&);
};
#endif