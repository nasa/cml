/*******************************************************************************
Purpose:
  (Define methods for the mass body detach impulsive class.)

Library dependencies:
  ((dynamics/body_action/src/body_action.cc)
   (dynamics/body_action/src/body_action_messages.cc)
   (dynamics/mass/src/mass_point_state.cc)
   (cml/models/utilities/cml_message/src/cml_message.cc))

Programmers:
  (((Gary Turner) (OSR) (December 2011) (TS21 contribution) (Initial version)))
  (((Jeff Semrau) (Hon) (July 2017) (Updates per code review)))

*******************************************************************************/

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"
#include "jeod/models/dynamics/body_action/include/body_action.hh"
#include "jeod/models/dynamics/dyn_manager/include/class_declarations.hh"
#include "jeod/models/dynamics/mass/include/mass.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "jeod/models/dynamics/mass/include/mass_point.hh"
#include "jeod/models/dynamics/body_action/include/body_action_messages.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "jeod/models/utils/math/include/vector3.hh"

#include "../include/mass_body_detach_impulsive.hh"

#include <string>

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
  if (mass_subject == nullptr && dyn_subject == nullptr) {
    CMLMessage::error ( __FILE__,__LINE__,
      "Subject body has not been specified for impulsive detach action ",
      action_identifier,
      "\nCannot detach nothing, action-application aborted.\n");

    if (terminate_on_error) {
      CMLMessage::fail ( __FILE__, __LINE__,
        "\nBody-detach error is terminal.\n");
    }
    return;
  }

  // Identify the specified MassBody; this could be null if dyn_subject has
  // been specified. or DynBody as a MassBody.
  if (!mass_subject) {mass_subject = &dyn_subject->mass;}

  /* Identify the parent before it gets detached.
     Have to cast away the const-ness because we will be accessing dyn_parent
     from mass_parent and, if it exxists, applying an impulse to it.*/
  auto * mass_parent =
    const_cast<jeod::MassBody *>(mass_subject->get_parent_body());
  if (mass_parent == nullptr) {
    CMLMessage::error ( __FILE__, __LINE__,
       action_identifier, " failed to detach ", mass_subject->name.get_name(),
       "\nbecause no parent body was found to which ",
       mass_subject->name.get_name(), " is currently attached.");

    if (terminate_on_error) {
      CMLMessage::fail ( __FILE__, __LINE__,
        "\nBody-detach error is terminal.\n");
    }
    return;
  }

  // Detach the bodies.
  if (!mass_subject->detach()) {
    // Detachment failed: Terminate the sim if terminate_on_error is set.
    CMLMessage::error ( __FILE__, __LINE__,
      action_identifier, " failed to detach ", mass_subject->name.get_name(),
      ".\n");

    if (terminate_on_error) {
      CMLMessage::fail (
        __FILE__, __LINE__, jeod::BodyActionMessages::fatal_error,
        "Body-detach error is terminal.\n");
    }
    // Close out this body-action, it is done
    jeod::BodyAction::apply (dyn_manager);
    return;
  }

  // Successfully detached.
  // NOTE - The detach process includes recalculation of the mass properties of
  // what was root all the way down to parent. Mass properties of child on
  // down are unaffected.
  CMLMessage::debug ( __FILE__, __LINE__,
     action_identifier, ": ", mass_subject->name.get_name(), " detached.\n");

  /* Identify the parent dyn-body element to apply the impulse to.
     parent is currently identified only as a MassBody, so find its DynBody
     counterpart.*/
  jeod::DynBody  * dyn_parent   = mass_parent->dyn_owner;
  if ( dyn_parent == nullptr) {
    CMLMessage::warn ( __FILE__, __LINE__,
      "The parent mass-body object specified in ", action_identifier,
      " is not a dyn-body object.\n"
      "This implies that both bodies are basic MassBody items and, as such\n"
      "have no state so applying an impulse would have no effect."
      "Bodies smoothly detached instead without impulse applied.");
    jeod::BodyAction::apply (dyn_manager);
    return;
  }

  if (sep_speed_spec_by_user) {
    const double mass_of_subject =
                   mass_subject->get_root_body()->composite_properties.mass;
    const double mass_of_parent =
                     mass_parent->get_root_body()->composite_properties.mass;
    const double combined_mass = mass_of_subject + mass_of_parent;

    impulse_magnitude = (combined_mass <= 0.0) ?
                          0.0 :
                          ((mass_of_subject * mass_of_parent) / (combined_mass))
                                                             * separation_speed;
  }

  if (!compute_impulse( *dyn_parent)) {return;}

  /* At this point, linear_impulse_inrtl is the impulse being applied to the
   * parent body.*/
  apply_impulse( *dyn_parent,
                  parent_point_name);


  /* Now apply the opposite impulse to the child body:
     JEOD::BodyAction provides get_subject_dyn_body() to return either the
     dyn_subject if subject is a DynBody, mass_subject's dyn_owner if
     subject is incorrectly identified as only a MassBody but really is
     a DynBody, or null if subject is just a MassBody.
     If it is just a MassBody, there is no state to modify with the
     application of an impulse, so leave.
     If there is a DynBody there, flip the impulse vector and apply it to the
     subject / child.*/
  jeod::DynBody * dyn_subject_ = get_subject_dyn_body();
  if (dyn_subject_ != nullptr) {
    jeod::Vector3::negate(linear_impulse_inrtl);
    apply_impulse( *dyn_subject_,
                    subject_point_name);
  }

  // Forward the action up the chain. Does nothing.
  jeod::BodyAction::apply (dyn_manager);
}



/*****************************************************************************
Name: compute_impulse
Purpose:
  Computes the linear impulse applied between the two separating bodies,
  expressing the result in the inertial frame.
*****************************************************************************/
bool
MassBodyDetachImpulsive::compute_impulse(
  jeod::DynBody & dyn_parent)
{
  const jeod::MassPoint * mass_point =
                            dyn_parent.mass.find_mass_point(parent_point_name);
  if (mass_point == nullptr) {
    CMLMessage::error ( __FILE__, __LINE__,
      "\n",action_identifier, " could not find mass point named ",
      parent_point_name,
      "\non parent body ", dyn_parent.name.get_name(),
      " to define the impulse vector.\n"
      "Impulse cannot be applied without direction.\n"
      "Vehicles separate cleanly without impulse.\n");
    if (terminate_on_error) {
      CMLMessage::fail ( __FILE__, __LINE__,
        "Body-detach error is terminal.\n");
    }
    return false;
  }

  /* If the angles have been set, use those to set the impulse-unit-vector
   * The impulse_unit_vector can also be set manually, but having the
   * angles set will take precendence.*/
  if (   MathUtils::has_changed_from( az_rot, 0.0)
      || MathUtils::has_changed_from( el_rot, 0.0)) {
    /* Azimuth/Elevation are defined relative to the the mass-point frame.
     * They utilize the y-z plane ofsaid frame as the base plane.
     * Azimuth is measured as a rotation about the x-axis from the
     *   negative y-axis towards the negative z-axis
     * Elevation is the angle below the y-z plane
     *   (i.e. towards the negative x-axis)
     * Note -- it is typicaly for a mass-point frame to define its x-axis
     * pointing outward away from the body so zero angles will result in an
     * impulse being applied inward, i.e. a "push".*/
    const double se = std::sin(el_rot);
    impulse_unit_vector[0] = -std::cos(el_rot);
    impulse_unit_vector[1] = -std::cos(az_rot) * se;
    impulse_unit_vector[2] = -std::sin(az_rot) * se;
  }

  /* Compute the linear-impulse, first in the frame associated with the
   * mass point and immediately transform into the dyn_body structure-frame
   * and inertial frame.*/
  jeod::Vector3::scale( impulse_unit_vector,
                        impulse_magnitude,
                        linear_impulse_inrtl);
  jeod::Vector3::transform_transpose( mass_point->T_parent_this,
                                      linear_impulse_inrtl);
  jeod::Vector3::transform_transpose(
                                 dyn_parent.structure.state.rot.T_parent_this,
                                 linear_impulse_inrtl);
  return true;
}

/*******************************************************************************
Purpose:(Applies an impulse at a MassPoint for parent and subject bodies)
*******************************************************************************/
void
MassBodyDetachImpulsive::apply_impulse( jeod::DynBody     & dyn_body,
                                        const std::string & mass_point_name)
{
  const jeod::MassPoint * mass_point =
                               dyn_body.mass.find_mass_point(mass_point_name);
  if (mass_point == nullptr) {
    CMLMessage::error ( __FILE__, __LINE__,
      action_identifier, " could not find mass point named ", mass_point_name,
      "\non body ", dyn_body.name.get_name(), " to apply impulse to."
      "\nNo impulse applied to this body.");

    if (terminate_on_error) {
      CMLMessage::fail ( __FILE__, __LINE__,
        "Body-detach error is terminal.\n");
    }
    return;
  }

  /* IMPORTANT NOTE - the detach has already been performed!
   * The subject body should be root, but (what was) the parent may be
   * attached to something else.
   * Create a root-body pointer, and assign it to the current dyn-body by
   * default. If the dyn-body is not root, it will be changed accordingly.*/
  jeod::DynBody * root_body = &dyn_body;

  /* Generate the angular impulse; the algorithm forks here depending on
   * whether the dyn-body is root or not. We need the angular impulse
   * expressed in the root-body's body-frame.*/
  double ang_impulse_root_body[3] = {0,0,0};
  if( dyn_body.is_root_body() ) {
    /* combine moment arm and linear impulse to get angular impulse
     * moment arm from CM to mass point in dyn_body structure.  :*/
    double moment_arm[3];
    jeod::Vector3::diff( mass_point->position, // in dyn_body struc
                         dyn_body.mass.composite_properties.position,
                         moment_arm); // wrt dyn_body body, in root-body struc
    double lin_impulse_struc[3];
    jeod::Vector3::transform( dyn_body.structure.state.rot.T_parent_this,
                              linear_impulse_inrtl,
                              lin_impulse_struc);
    jeod::Vector3::cross( moment_arm,              // in root-body struc
                          lin_impulse_struc,       // in root-body struc
                          ang_impulse_root_body);  // in root-body struc
    jeod::Vector3::transform( dyn_body.mass.composite_properties.T_parent_this,
                              ang_impulse_root_body);  // in dyn-body body-frame
  }
  else {
    /* Identify the root body
       cast away the const-ness because we have to apply the impulse to the
       root body.*/
    root_body = const_cast<jeod::DynBody *>(dyn_body.get_root_body());
    // sanity check -- root body pointer must be non-NULL to continue.
    if (root_body == nullptr) {
      CMLMessage::error ( __FILE__, __LINE__,
         action_identifier, " could not find root body for body ",
         dyn_body.name.get_name(),
        "\nImpulse must be applied to root body."
        "\nNo impulse applied to this body.");
      if (terminate_on_error) {
        CMLMessage::fail ( __FILE__, __LINE__,
          "Body-detach error is terminal.\n");
      }
      return;
    }
    // Generate the relative state between the root-body body-frame and the
    // dyn-body structural frame.
    jeod::RefFrameState dyn_struc_wrt_root_body;
    dyn_body.structure.compute_relative_state( root_body->composite_body,
                                               dyn_struc_wrt_root_body);

    /* transform vector from dyn-body structure-point to location of
       impulse-point so that it is expressed in the root-body body-frame.
       Temporarily assign to moment_arm (although this is not yet the
       representation of the moment-arm). */
    double moment_arm[3];
    jeod::Vector3::transform_transpose(
                                  dyn_struc_wrt_root_body.rot.T_parent_this,
                                  mass_point->position, // in and wrt dyn-struc
                                  moment_arm);

    /* Then add to that the position of the dyn-body structure-point relative
     * to the root-body body-frame to get the location of the impulse-point
     * relative to the root-body CoM, expressed in the root-body body-frame.
     * This is the moment-arm about the CoM of the root body.*/
    jeod::Vector3::incr( dyn_struc_wrt_root_body.trans.position,
                         moment_arm);

    /* Transform the linear-impulse from the dyn-body struc-frame to the
       root-body body-frame. */
    double lin_impulse_root_body[3];
    jeod::Vector3::transform( root_body->composite_body.state.rot.T_parent_this,
                              linear_impulse_inrtl,
                              lin_impulse_root_body);

    /* Take the cross product (r x I) to get the angular impulse.*/
    jeod::Vector3::cross( moment_arm,
                          lin_impulse_root_body,
                          ang_impulse_root_body);
  }

  // The rest of the algorithm is agnostic of whether the dyn-body is the
  // root-body, it just uses root-body.

  // Apply the linear impulse to get a delta-velocity
  if ( root_body->mass.composite_properties.mass <= 0.0) {
    CMLMessage::error ( __FILE__, __LINE__,
       action_identifier, " involves a root body ",root_body->name.get_name(),
       "with invalid mass ", root_body->mass.composite_properties.mass,
       "\n Cannot apply an impulse to this mass.\n"
       "Aborting application of impulse.\n");
    if (terminate_on_error) {
      CMLMessage::fail ( __FILE__, __LINE__,
        "Body-detach error is terminal.\n");
    }
    return;
  }
  const double inv_mass = 1.0 / root_body->mass.composite_properties.mass;
  double inv_inertia[3][3];
  jeod::Matrix3x3::invert( root_body->mass.composite_properties.inertia,
                           inv_inertia);

  double scratch[3];
  jeod::Vector3::scale( linear_impulse_inrtl,
                        inv_mass,
                        scratch);  // delta velocity, inertial
  jeod::Vector3::incr( root_body->composite_body.state.trans.velocity,
                       scratch); // new velocity, inertial

  root_body->set_velocity( scratch,
                           root_body->composite_body);

  // Apply the angular impulse to get a delta-angular-rate
  jeod::Vector3::transform( inv_inertia,
                            ang_impulse_root_body,
                            scratch); // delta rate, body frame
  jeod::Vector3::incr( root_body->composite_body.state.rot.ang_vel_this,
                       scratch);      // new angular rate, body frame.
  root_body->set_attitude_rate( scratch,
                                root_body->composite_body);
  root_body->propagate_state();
}