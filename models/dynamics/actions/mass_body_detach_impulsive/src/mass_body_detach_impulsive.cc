/*******************************************************************************
Purpose:
  (Define methods for the mass body detach impulsive class.)

Library dependencies:
  ((dynamics/body_action/src/body_action.cc)
   (dynamics/body_action/src/body_action_messages.cc)
   (dynamics/mass/src/mass_point_state.cc))

Programmers:
  (((Gary Turner) (OSR) (December 2011) (TS21 contribution) (Initial version)))
  (((Jeff Semrau) (Hon) (July 2017) (Updates per code review)))

*******************************************************************************/

// System includes
#include <cstddef>

#include "jeod/models/dynamics/mass/include/mass.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "jeod/models/dynamics/mass/include/mass_point.hh"
#include "jeod/models/dynamics/body_action/include/body_action_messages.hh"

#include "../include/mass_body_detach_impulsive.hh"

/*******************************************************************************
Function: MassBodyDetachImpulsive::MassBodyDetachImpulsive
Purpose: (Construct a MassBodyDetachImpulsive.)
*******************************************************************************/
MassBodyDetachImpulsive::MassBodyDetachImpulsive()
  :
  impulse_magnitude(0.0),
  subject_point_name(),
  parent_point_name(),
  separation_speed(0.0),
  sep_speed_spec_by_user(false)
{}

/*******************************************************************************
Function: MassBodyDetachImpulsive::initialize
Purpose: (Initialize a MassBodyDetach)
*******************************************************************************/
void
MassBodyDetachImpulsive::initialize( jeod::DynManager & dyn_manager)
{
  // Forward the request up the chain.
  jeod::BodyAction::initialize (dyn_manager);
}

/*******************************************************************************
Function: MassBodyDetachImpulsive::apply
Purpose: (Detach the mass bodies in question.)
*******************************************************************************/
void
MassBodyDetachImpulsive::apply( jeod::DynManager & dyn_manager)
{
  jeod::MassBody * subject = nullptr;
  if (is_subject_dyn_body()) {
    subject = &get_subject_dyn_body()->mass;
  } else {
    subject = mass_subject;
  }

  if (subject == nullptr) {
    CMLMessage::error (
      __FILE__,__LINE__, jeod::BodyActionMessages::not_performed,
      "\nSubject body is identified as NULL for impulsive detach action ", action_identifier, ".\n"
      "You forgot to specify a subject body.\n");

    if (terminate_on_error) {
      CMLMessage::fail (
        __FILE__, __LINE__, jeod::BodyActionMessages::fatal_error,
        "\nBody-detach error is terminal.\n");
    }
    return;
  }


  // Identify the parent before it gets detached.
  // Have to cast away the const-ness because we will be applying an impulse
  // to the parent.
  jeod::MassBody * parent = const_cast<jeod::MassBody *>(subject->get_parent_body());
  if (parent == NULL) {
    CMLMessage::error (
      __FILE__, __LINE__, jeod::BodyActionMessages::not_performed,
       "\n", action_identifier, " failed to detach ", subject->name, " because no parent body was found to\n"
      "which ", subject->name, " is currently attached.");

    if (terminate_on_error) {
      CMLMessage::fail (
        __FILE__, __LINE__, jeod::BodyActionMessages::fatal_error,
        "\nBody-detach error is terminal.\n");
    }
    return;
  }

  // Detach the bodies.
  if (!subject->detach()) {
    // Detachment failed: Terminate the sim if terminate_on_error is set.
    CMLMessage::error (
      __FILE__, __LINE__, jeod::BodyActionMessages::not_performed,
      "\n", action_identifier, " failed to detach ", subject->name, ".");


    if (terminate_on_error) {
      CMLMessage::fail (
        __FILE__, __LINE__, jeod::BodyActionMessages::fatal_error,
        "\nBody-detach error is terminal.\n");
    }
    jeod::BodyAction::apply (dyn_manager);
    return;
  }

  // Successfully detached.
  // NOTE - The detach process includes recalculation of the mass properties of
  // what was root all the way down to parent. Mass properties of child on
  // down are unaffected.
  CMLMessage::debug (
    __FILE__, __LINE__, jeod::BodyActionMessages::trace,
     "\n", action_identifier, ": ", subject->name, " detached.");

  jeod::DynBody  * parent_body  = parent->dyn_owner;
  jeod::DynBody  * subject_body = get_subject_dyn_body();

  // This test could be abbreviated to if (parent_body == NULL) because if the
  // parent-body is not a jeod::DynBody, then the subject-body cannot be a jeod::DynBody
  // (cannot attach a Dynbody to a jeod::MassBody). However, for completeness:
  if ( parent_body == NULL && subject_body == NULL) {
    CMLMessage::warn (
      __FILE__, __LINE__, jeod::BodyActionMessages::not_performed,
      "\nNeither of the two mass-body objects specified in ", action_identifier, " are dyn-body "
      "objects.\nBodies smoothly detached instead without impulse applied.");
    jeod::BodyAction::apply (dyn_manager);
    return;
  }

  if (sep_speed_spec_by_user) {
    const double mass_of_subject = subject->get_root_body()->composite_properties.mass;
    const double mass_of_parent  = parent->get_root_body()->composite_properties.mass;
    impulse_magnitude   = ((mass_of_subject * mass_of_parent) / (mass_of_subject + mass_of_parent)) * separation_speed;
  }

  // This if-test is redundant, put here purely for comprehensive safety
  // requirements. Do not try to hit a parent-body == NULL in code-coverage
  // verification, it can't be done. If parent_body == NULL, then
  // subject_body also == NULL, and method would have already exited.
  if (parent_body != NULL) {
    apply_impulse( *parent_body,
                    parent_point_name);
  }

  // Conversely, subject-body can be NULL (case of a jeod::MassBody attached to a
  // jeod::DynBody) Don't bother trying to apply impulse to a jeod::MassBody because it
  // has no state.
  if (is_subject_dyn_body()) {
    apply_impulse( *subject_body,
                    subject_point_name);
  }

  // Forward the action up the chain. Does nothing.
  jeod::BodyAction::apply (dyn_manager);
}

/*******************************************************************************
Purpose:(Applies an impulse at a MassPoint for parent and subject bodies)
*******************************************************************************/
void
MassBodyDetachImpulsive::apply_impulse( jeod::DynBody          & dyn_body,
                                        std::string  mass_point_name)
{
  const jeod::MassPoint * mass_point = dyn_body.mass.find_mass_point(mass_point_name);
  if (mass_point == NULL) {
    CMLMessage::error (
      __FILE__, __LINE__, jeod::BodyActionMessages::not_performed,
       "\n", action_identifier, " could not find mass point named ", mass_point_name, " on body ", dyn_body.name, " to apply impulse to."
      "\nNo impulse applied to this body.");

    if (terminate_on_error) {
      CMLMessage::fail (
        __FILE__, __LINE__, jeod::BodyActionMessages::fatal_error,
        "\nBody-detach error is terminal.\n");
    }
    return;
  }

  double lin_impulse[3];
  double lin_impulse_inrtl[3];
  double ang_impulse[3];
  double moment_arm[3];
  double new_vel[3];
  double new_rate[3];
  double inv_inertia[3][3];
  double impulse[3];

  jeod::Vector3::initialize(lin_impulse);
  jeod::Vector3::initialize(lin_impulse_inrtl);
  jeod::Vector3::initialize(ang_impulse);
  jeod::Vector3::initialize(impulse);

  impulse[0] = -impulse_magnitude;   // Impulse provided back along the x-axes

  // Transform input impulse from mass point structure to dyn_body structure
  jeod::Vector3::transform_transpose( mass_point->T_parent_this,
                                impulse,
                                lin_impulse); // dyn_body struc.

  // Transform linear impulse to inertial.
  jeod::Vector3::transform_transpose( dyn_body.structure.state.rot.T_parent_this,
                                lin_impulse,
                                lin_impulse_inrtl); // inertial

  // IMPORTANT NOTE - the detach has already been performed!
  // The subject body should be root, but (what was) the parent may be
  // attached to something else.
  // Create a root-body pointer, and assign it to the current dyn-body by
  // default. If the dyn-body is not root, it will be changed accordingly.
  jeod::DynBody * root_body = &dyn_body;

  // Generate the angular impulse; the algorithm forks here depending on
  // whether the dyn-body is root or not. We need the angular impulse
  // expressed in the root-body's body-frame.
  if( dyn_body.is_root_body() ) {
    // combine moment arm and linear impulse to get angular impulse
    // moment arm from CM to mass point in dyn_body structure.  :
    jeod::Vector3::diff( mass_point->position, // in dyn_body struc
                   dyn_body.mass.composite_properties.position, // in dyn_body struc
                   moment_arm); // wrt dyn_body body, in dyn_body struc
    jeod::Vector3::cross( moment_arm,   // in dyn_body struc
                    lin_impulse,  // in dyn_body struc
                    ang_impulse); // in dyn_body struc
    jeod::Vector3::transform( dyn_body.mass.composite_properties.T_parent_this,
                        ang_impulse);  // in dyn-body body-frame
  }
  else {
    // Identify the root body
    // cast away the const-ness because we have to apply the impulse to the
    // root body.
    root_body = const_cast<jeod::DynBody *>(dyn_body.get_root_body());
    // sanity check -- root body pointer must be non-NULL to continue.
    if (root_body == NULL) {
      CMLMessage::error (
        __FILE__, __LINE__, jeod::BodyActionMessages::not_performed,
         "\n", action_identifier, " could not find root body for body ", dyn_body.name, ".\n"
        "Impulse must be applied to root body.\n"
        "No impulse applied to this body.");
      if (terminate_on_error) {
        CMLMessage::fail (
          __FILE__, __LINE__, jeod::BodyActionMessages::fatal_error,
          "\nBody-detach error is terminal.\n");
      }
      return;
    }
    // Generate the relative state between the root-body body-frame and the
    // dyn-body structural frame.
    jeod::RefFrameState dyn_struc_wrt_root_body;
    dyn_body.structure.compute_relative_state( root_body->composite_body,
                                               dyn_struc_wrt_root_body);

    // transform location of impulse-point into root-body body-frame.
    jeod::Vector3::transform_transpose( dyn_struc_wrt_root_body.rot.T_parent_this,
                                  mass_point->position, // in and wrt dyn-struc
                                  moment_arm);  // position of application pt.
                                                // wrt dyn-body struc, expressed
                                                // in root-body body-frame
    jeod::Vector3::incr( dyn_struc_wrt_root_body.trans.position,
                   moment_arm); // wrt and in root-body body-frame
    jeod::Vector3::transform_transpose( dyn_struc_wrt_root_body.rot.T_parent_this,
                                  lin_impulse); // was in dyn-body struc-frame
                                                // now in root-body body-frame
    jeod::Vector3::cross( moment_arm,
                    lin_impulse,
                    ang_impulse); // in root-body body-frame
  }

  // The rest of the algorithm is agnostic of whether the dyn-body is the
  // root-body, it just uses root-body.

  // Apply the linear impulse to get a delta-velocity
  double inv_mass;
  inv_mass = 1.0 / root_body->mass.composite_properties.mass;
  jeod::Matrix3x3::invert( root_body->mass.composite_properties.inertia,inv_inertia);

  jeod::Vector3::scale( lin_impulse_inrtl,
                  inv_mass,
                  new_vel);  // delta velocity, inrtl
  jeod::Vector3::incr( root_body->composite_body.state.trans.velocity,
                 new_vel);

  root_body->set_velocity( new_vel,
                           root_body->composite_body);

  // Apply the angular impulse to get a delta-angular-rate
  jeod::Vector3::transform( inv_inertia,
                      ang_impulse,
                      new_rate); // delta rate, body frame
  jeod::Vector3::incr( root_body->composite_body.state.rot.ang_vel_this,
                 new_rate);      // new angular rate, body frame.
  root_body->set_attitude_rate( new_rate,
                                root_body->composite_body);
  root_body->propagate_state();
}
