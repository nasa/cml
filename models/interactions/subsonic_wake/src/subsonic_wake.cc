/*******************************TRICK HEADER******************************
PURPOSE: (A generic model to simulate subsonic wake effects)

REFERENCE:
   ((Derived from Orion Subsonic wake model for Orion by Mark McPherson)
    (CM Wake Recirculation Zone CFD Report - EG-CAP-10-134 (11/3/2010))
    (Orion Sim Data Book (06/06/2011) - Reference drag area (Cd*S_ref) for FBC))

PROGRAMMERS:
  (((Mark McPherson) (NASA/DM42) (Apr 2012) (ANTARES) (Foundation))
   ((Gary Turner) (OSR) (Mar 2016) (Antares)
                (New adaptation to meet Antares coding standards))
   ((Daniel Ghan) (OSR) (Aug 2020) (Antares) (Clean-up and bug fixes)))

NOTES:
     Body A - The wake-producing body
     Body B - The object inside the wake
**********************************************************************/

#include "../include/subsonic_wake.hh"

#include "jeod/models/utils/math/include/vector3.hh"


/*****************************************************************************
constructor
*****************************************************************************/
SubsonicWake::SubsonicWake(
     const WakeParams & params_in,
     WakeGeneratingBody & objectA_in,
     WakeFollowingBody  & objectB_in,
     WakeEffectsOut & effects_in)
  :
  params(params_in),
  effects(effects_in),
  objectA(objectA_in),
  objectB_ref(objectB_in),
  prf_model( objectA_in,
             objectB_in,
             params_in),
  rev_flow(  objectA_in,
             objectB_in,
             params_in.rev_flow),
  use_prf_model(false),
  use_rev_flow(false),
  subd_prf(false),
  subd_revflow(false)
{
  subscribe_name = "SubsonicWake:";
}
/****************************************************************************/
SubsonicWakeNoForce::SubsonicWakeNoForce(
     const double & bodyB_area_in,
     const double * bodyB_inertial_pos_in,
     const double * bodyB_inertial_vel_in,
     const WakeParams & params_in,
     WakeGeneratingBody & objectA_in,
     WakeEffectsOut   & effects_in)
  :
  SubsonicWake(params_in,
               objectA_in,
               objectB,
               effects_in),
  objectB( bodyB_area_in,
           bodyB_inertial_pos_in,
           bodyB_inertial_vel_in,
           objectA_in,
           params_in.grid_origin)
{}
/****************************************************************************/
SubsonicWakeWithForce::SubsonicWakeWithForce(
     const double & bodyB_area_in,
     const double * bodyB_inertial_pos_in,
     const double * bodyB_inertial_vel_in,
     const double (& bodyB_T_inrtl_to_body_in)[3][3],
     const double (& bodyB_T_struc_to_body_in)[3][3],
     const WakeParams & params_in,
     WakeGeneratingBody & objectA_in,
     WakeEffectsOut & effects_in)
  :
  SubsonicWake(params_in,
               objectA_in,
               objectB,
               effects_in),
  objectB( bodyB_area_in,
           bodyB_inertial_pos_in,
           bodyB_inertial_vel_in,
           bodyB_T_inrtl_to_body_in,
           bodyB_T_struc_to_body_in,
           objectA_in,
           params_in.grid_origin)
{}

/*****************************************************************************
initialize()
Purpose:(initialize the data tables)
*****************************************************************************/
void
SubsonicWake::initialize()
{
  rev_flow.initialize();
  prf_model.initialize();
  SubscriptionBase::initialize();
}

/*****************************************************************************
update
Purpose:(updates the effect calculation)
Assumption: (The model gets subscribed when its object gets ejected into the
             wake, and unsubscribed when it is no longer needed.  If it does
             not get unsubscribed, the model will deactivate itself when the
             speed of the wake-producing-body falls below some threshold
             defined in params.mach_off.)
*****************************************************************************/
void
SubsonicWake::update()
{
  // If there is nothing in the wake, get out of here.
  if (!active) {
    return;
  }

  // If both reverse-flow and pressure-recovery-fraction are disabled, there
  // is no output from this model.  Unless the override flag has been set,
  // populate the standard outputs and leave.
  // Similarly, if there is no force to be computed, and the vehicle is too
  // slow, the same rule applies.
  bool fast_enough = (objectA.freestream_mach >=  params.mach_off);
  if (params.generate_distance_override ||
       ( fast_enough && (rev_flow.is_active() || prf_model.is_active()))) {
    objectB_ref.compute_relative_state();
  }

  // Do not evaluate rev-flow and prf if Mach number too low
  if ( fast_enough) {
    rev_flow.update();
    prf_model.update();
  }
  else {
    deactivate();
  }

  if (objectB_ref.get_generate_force()) {
    objectB_ref.compute_force(
                        prf_model.force_toward_A + rev_flow.force_toward_A);
  }

  // copy the effects into the out-structure for external models.
  effects.prf_model.PRF = prf_model.PRF;
  effects.prf_model.in_region = prf_model.get_in_region();
  effects.rev_flow.Qrev = rev_flow.Qrev;
}

/*****************************************************************************
activate
Purpose:(Activates the model and its submodels)
*****************************************************************************/
void
SubsonicWake::activate()
{
  if (use_prf_model) {
    prf_model.subscribe();
    subd_prf = true;
  }
  if (use_rev_flow) {
    rev_flow.subscribe();
    subd_revflow = true;
  }
  SubscriptionBase::activate();
  update();
}

/*****************************************************************************
deactivate
Purpose:(zero out the force on the body at deactivation)
*****************************************************************************/
void
SubsonicWake::deactivate()
{
  if (subd_prf) {
    prf_model.unsubscribe();
    subd_prf = false;
  }
  if (subd_revflow) {
    rev_flow.unsubscribe();
    subd_revflow = false;
  }

  SubscriptionBase::deactivate();
}
/****************************************************************************/
void
SubsonicWakeWithForce::deactivate()
{
  jeod::Vector3::initialize(objectB.force);
  SubsonicWake::deactivate();
}
