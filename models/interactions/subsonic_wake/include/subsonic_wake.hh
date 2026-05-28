/*******************************TRICK HEADER******************************
PURPOSE: (A generic model to simulate subsonic wake effects)

REFERENCE:
   ((Derived from Orion Subsonic wake model for Orion by Mark McPherson)
    (CM Wake Recirculation Zone CFD Report - EG-CAP-10-134 (11/3/2010))
    (Orion Sim Data Book (06/06/2011) - Reference drag area (Cd*S_ref) for FBC))

LIBRARY DEPENDENCY:
   ((../src/subsonic_wake.cc)
    (../src/reverse_flow.cc)
    (../src/prf_model.cc))

PROGRAMMERS:
  (((Mark McPherson) (NASA/DM42) (Apr 2012) (ANTARES) (Foundation))
   ((Gary Turner) (OSR) (Mar 2016) (Antares)
                (New adaptation to meet Antares coding standards))
   ((Bingquan Wang) (OSR) (Oct 2016) (Antares)
                (Library dependency warning fixing))
   ((Daniel Ghan) (OSR) (Aug 2020) (Antares) (Clean-up and bug fixes)))

NOTES:
     Body A - The wake-producing body
     Body B - The object inside the wake
**********************************************************************/
#ifndef SUBSONIC_WAKE_HH
#define SUBSONIC_WAKE_HH

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include "wake_bodies.hh"
#include "wake_params.hh"
#include "wake_effects.hh"


/*****************************************************************************
SubsonicWake
Purpose:(Replaces the old WAKE_EFFECTS struct and extends it to encapsulate
         the model functionality that was previously prototyped in
         wake_effects_exec.h.
         This is the top-level model-class.)
*****************************************************************************/
class SubsonicWake : public SubscriptionBase
{
 protected: // External references
  const WakeParams & params; /* (--) controlling parameters for the model */
  WakeEffectsOut & effects; /* (--) reverse-flow and PRF output */
  WakeGeneratingBody & objectA; /* (--) variables specific to generating body */
  WakeFollowingBody  & objectB_ref;  /* (--) variables specific to following body */

 public:
  WakePrfModel        prf_model; /* (--) the pressure-recovery-fraction model.*/
  WakeReverseFlow     rev_flow;  /* (--) the reverse-flow model. */

  bool use_prf_model; /* (--) whether to use the PRF model. */
  bool use_rev_flow;  /* (--) whether to use the rev-flow model. */

 protected:
  // These flags are used to determine whether to unsubscribe from the effect
  // models. The use_prf_model and use_rev_flow flags cannot be used for this
  // purpose because they are public and could be changed anytime.
  bool subd_prf;  /* (--) Whether subscribed to the PRF model */
  bool subd_revflow; /* (--) Whether subscribed to the rev-flow model*/

 public:
  SubsonicWake(const WakeParams & params_in,
               WakeGeneratingBody & objectA_in,
               WakeFollowingBody  & objectB_in,
               WakeEffectsOut & effects_output_in);
  virtual ~SubsonicWake(){};

  virtual void initialize();
  void update();

 protected:
  virtual void activate();
  virtual void deactivate();
  virtual void query_objB()=0; // just to make it abstract

 private: // and undefined:
  SubsonicWake (const SubsonicWake& rhs);
  void operator = (const SubsonicWake& rhs);
};

/*****************************************************************************
SubsonicWakeNoForce
Purpose:(SubsonicWake with no forces on the following body calculated)
*****************************************************************************/
class SubsonicWakeNoForce : public SubsonicWake
{
 public:
  WakeFollowingBodyNoForce objectB; /* (--) the following body. */

  SubsonicWakeNoForce(
     const double & bodyB_area_in,
     const double * bodyB_inertial_pos_in,
     const double * bodyB_inertial_vel_in,
     const WakeParams & params_in,
     WakeGeneratingBody & objectA_in,
     WakeEffectsOut & effects_in);

 protected:
  virtual void query_objB() override{}; // just to make it instantiable

 private: // and undefined:
  SubsonicWakeNoForce (const SubsonicWakeNoForce& rhs);
  void operator = (const SubsonicWakeNoForce& rhs);
};

/*****************************************************************************
SubsonicWakeWithForce
Purpose:(SubsonicWake with forces on the following body calculated)
*****************************************************************************/
class SubsonicWakeWithForce : public SubsonicWake
{
 public:
  WakeFollowingBodyWithForce objectB; /* (--) the following body.*/

  SubsonicWakeWithForce(
     const double & bodyB_area_in,
     const double * bodyB_inertial_pos_in,
     const double * bodyB_inertial_vel_in,
     const double (& bodyB_T_inrtl_to_body_in)[3][3],
     const double (& bodyB_T_struc_to_body_in)[3][3],
     const WakeParams & params_in,
     WakeGeneratingBody & objectA_in,
     WakeEffectsOut & effects_in);

  virtual void deactivate() override;

 protected:
  virtual void query_objB() override{}; // just to make it instantiable

 private: // and undefined:
  SubsonicWakeWithForce (const SubsonicWakeWithForce& rhs);
  void operator = (const SubsonicWakeWithForce& rhs);
};

#endif
