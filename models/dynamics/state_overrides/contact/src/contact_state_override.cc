/******************************** TRICK HEADER **********************************
PURPOSE:
   (The model provides a simple state override mechanism, avoiding the need to
   generate and integrate contact forces.  A nominal offset is provided between
   two bodies; if their relative sseparation is less than this nominal offset,
   the bodies are assumed to be in contact and their relative state is
   overridden such that it is equal to the nominal separation.)

PROGRAMMERS:
   (((Daniel Ghan) (OSR) (Mar 2020) (Antares) (Initial version)))

********************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"

#include "../include/contact_state_override.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
ContactStateOverride::ContactStateOverride(jeod::DynBody& reference_body,
                                           jeod::DynBody& override_body)
  :
  reference_body(reference_body),
  override_body(override_body),
  deactivation_threshold(0.0),
  contact_normal(),
  contact_pos_overridestruc_wrt_refstruc(),
  contact_pos_overridenomcore_wrt_refnomcore(),
  contact_quat_refstruc_to_overridestruc(),
  user_set_contact_normal(false),
  user_set_contact_position(false),
  user_set_contact_orientation(false),
  bodies_are_attached(false),
  self_disabled(false)
{
  jeod::Vector3::initialize(contact_normal);
  jeod::Vector3::initialize(contact_pos_overridestruc_wrt_refstruc);
  jeod::Vector3::initialize(contact_pos_overridenomcore_wrt_refnomcore);
  jeod::Vector3::initialize(nominal_reference_core_position);
  jeod::Vector3::initialize(nominal_override_core_position);
}

/*****************************************************************************
initialize is provided by inheritance from SubscriptionBase
*****************************************************************************/


/*****************************************************************************
update
Purpose:(
  Main executable.  Tests the current separation-state against the
  desired contact separation-state and the deactivation threshold.
  If the separation is less than the contact separation, the state of the
  override-body is overridden such that the
  separation matches the contact separation.  If the separation is greater
  than the deactivation-threshold, the model self-deactivates.)
*****************************************************************************/
void ContactStateOverride::update()
{
  if (!active) {
    return;
  }

  if (bodies_are_attached) {
    // Check again; if they are still attached, there is nothing to do.
    if (reference_body.get_root_body() == override_body.get_root_body()) {
      return;
    }
    bodies_are_attached = false;
  }

  // Separation is defined to be from the reference-body structural origin to
  // the override-body structural origin.
  // The structural frame is used -- rather than the body frame -- for the
  // separation state because the structure-origin is fixed wrt the vehicle
  // surface-structure, whereas the body-frame-origin (i.e. CCoM) is not.
  jeod::RefFrameState sep_state;
  override_body.structure.compute_relative_state(
                               reference_body.structure,
                               sep_state);

  // Current position minus contact position of override nominal position
  // relative to reference nominal position in reference structure frame

  /* Now compute the current relative positions of the points on the 2
  *  vehicles that respresent the nominal/reference CoM.
  *  For a post-separation analysis, these are typically the locations of where
  *  the CoMs *were* at vehicle separation.*/
  double scratch[3];
  /* 1. Transform the override-body's position from override-body structural
        frame to reference-body structural frame:
        scratch = r_{ OC | OS : RS} */
  jeod::Vector3::transform_transpose( sep_state.rot.T_parent_this,
                                nominal_override_core_position,
                                scratch);
  /* 2. Add that to the structure-to-structure position
        scratch = r_{ OC | RS : RS} */
  jeod::Vector3::incr( sep_state.trans.position,
                 scratch); // ref-struc to override-nominal-CoM-pt in ref-struc
  /* 3. Subtract off the reference-body structure-to-nominal-CoM position.
        scratch = r_{ OC | RC : RS},
           the ref-nominal-CoM-pt to override-nominal-CoM-pt in ref-struc*/
  jeod::Vector3::decr( nominal_reference_core_position,
                 scratch);

  /* 4. Subtract off the nominal CoM-to-CoM position vector to find out how
        much the current position vector between these two points differs from
        the nominal position vector between these two points.
        This is the difference between where the vehicle are (relative to one
        another) and where they should be if they were in contact*/
  jeod::Vector3::decr( contact_pos_overridenomcore_wrt_refnomcore,
                 scratch);

  // Component of the delta parallel to contact normal vector, or the distance
  // between the two bodies that is in excess of their usual contact
  // separation.
  double distance = jeod::Vector3::dot(scratch, contact_normal);

  // If <0, they are overlapping and the override-body must be placed at its
  // contact point.
  contact_detected = (distance < 0);
  if (contact_detected) {
    override_state();
  }

  // Otherwise, if the separation distance > 0 they are separated.  If the
  // distance exceeds some (user-defined) threshold, it is safe to assume the
  // vehicles are no longer at risk of contacting and the model can be
  // disabled.
  else if (distance > deactivation_threshold) {
    disable();
    self_disabled = true;
  }
}


/*****************************************************************************
override_state
Purpose:(Overrides the state of the integrated frame of the override-body)
*****************************************************************************/
void
ContactStateOverride::override_state()
{
  // NOTE - the integrated frame is always associated with the root body of
  //        the mass-tree.  If the body identified as the override-body is at
  //        the root of its own mass tree, things are a little easier because
  //        it owns the integration frame.
  if (override_body.is_root_body()) {
    override_state_root( override_body.composite_body,
                         override_body.mass.composite_properties.position,
                         override_body.mass.composite_properties.Q_parent_this);
    // push the new state through the entire mass-tree.
    override_body.propagate_state();
  }
  else {
    // Identify the root body and cast away the const-ness so we can move it
    jeod::DynBody* override_root = const_cast<jeod::DynBody*>(
                                             override_body.get_root_body());
    if (override_root == NULL) {
      CMLMessage::error(
        __FILE__,__LINE__,"Unidentified mass tree error\n"
        "The mass-tree returned a NULL pointer for the root of the tree\n"
        "containing the override-body (", override_body.name,"(.\n"
        "This should not happen.\n"
        "Aborting state override.\n");
      return;
    }
    jeod::RefFrameState rel_state;
    override_root->composite_body.compute_relative_state(
                                                    override_body.structure,
                                                    rel_state);
    override_state_root( override_root->composite_body,
                         rel_state.trans.position,
                         rel_state.rot.Q_parent_this);
    override_root->propagate_state();
  }
}

/*****************************************************************************
override_state_root
Purpose:(Overrides the state of the root-body's composite-body frame given the
         location of said frame relative to the override-body's structure-frame)
Assumptions: (The integration frame is the composite-body-frame)
Nomenclature: r_{A | B : C} is the position (r) of frame A relative to
                            frame B, expressed in frame C.
              oB - body frame of override-body
              oS - structure-frame of override-body
              RB - body-frame of the root-body of the tree containing
                   override-body
              rB - body frame of reference-body
              rS - structure-frame of reference-body
              I  - inertial frame
              T_A_to_B - transformation matrix from frame A to frame B.

*****************************************************************************/
void
ContactStateOverride::override_state_root(
    jeod::BodyRefFrame & integ_frame,
    double             pos_integ_frame_wrt_override_struc[3],
    const jeod::Quaternion & Q_override_struc_to_integ_frame)
{
  // Position override: Need the position of the override-body's integration
  // frame relative to, and expressed in, the inertial frame.  This method
  // assumes that the integration frames of both bodies are their respective
  // composite-body frames, and that the two bodies share a common inertial
  // frame.
  // r_{RB|I:I} = r_{RB|oS:I} + r{oS|rS:I} + r{rS|I:I}
  //            = [T_oS_to_I] [r_{RB|oS:oS}] +
  //              [T_rS_to_I] [r_{oS|rS:rS}] +
  //              r{rS|I:I}
  //            = [T_rS_to_I] [ [T_oS_to_rS] [r_{RB|oS:oS}] + r_{oS|rS:rS}] +
  //              r{rS|I:I}

  double scratch[3];
  // generate position of integ-frame wrt override-struc, expressed in
  // reference-body's structural frame.  Use the relative orientation between
  // the struc frames at contact
  jeod::Quaternion conjugate_;
  contact_quat_refstruc_to_overridestruc.conjugate( conjugate_);
  conjugate_.left_quat_transform( pos_integ_frame_wrt_override_struc,
                                  scratch);
  // add the offset between reference-body-structure -to-
  // override-body-structure at contact.
  jeod::Vector3::incr( contact_pos_overridestruc_wrt_refstruc,
                 scratch);
  // transform to inertial
  double pos_rootcomp_wrt_refstruc_inertial[3];
  jeod::Vector3::transform_transpose(
                      reference_body.structure.state.rot.T_parent_this,
                      scratch,
                      pos_rootcomp_wrt_refstruc_inertial);
  // Add the position of the reference-body's structure-frame.
  jeod::Vector3::sum( pos_rootcomp_wrt_refstruc_inertial,
                reference_body.structure.state.trans.position,
                integ_frame.state.trans.position);


  // Velocity override
  // NOTE - important assumption here that velocity components of the structural
  //        frame relative to the body frame (e.g. resulting from slosh) are
  //        negligible
  double ang_vel_inertial[3];
  jeod::Vector3::transform_transpose(reference_body.structure.state.rot.T_parent_this,
                               reference_body.structure.state.rot.ang_vel_this,
                               ang_vel_inertial);
  jeod::Vector3::cross( ang_vel_inertial,
                  pos_rootcomp_wrt_refstruc_inertial,
                  scratch); // w x r term
  jeod::Vector3::sum( reference_body.structure.state.trans.velocity,
                scratch,
                integ_frame.state.trans.velocity);

  // Orientation override
  jeod::Quaternion scratch_quat;
  Q_override_struc_to_integ_frame.multiply(
                     contact_quat_refstruc_to_overridestruc,
                     scratch_quat);
  scratch_quat.multiply( reference_body.structure.state.rot.Q_parent_this,
                         integ_frame.state.rot.Q_parent_this);

  /* The frame's quaternion is the fundamental attitude information, with the
   * transformation-mtrix being a derived representation from the quaternion.
   * Eventually, the transformation-matrix will be updated to match this
   * quaternion, but that might not happen until after some other model has
   * used the now-obsolete matrix. So just to be safe, compute the matrix as
   * well. */
  integ_frame.state.rot.Q_parent_this.left_quat_to_transformation(
                                         integ_frame.state.rot.T_parent_this);

  // Angular rate override
  integ_frame.state.rot.Q_parent_this.left_quat_transform(
                                            ang_vel_inertial,
                                            integ_frame.state.rot.ang_vel_this);
}

/*****************************************************************************
activate
Purpose:(Activates the state-override capability.  This is a protected method,
         called by the public subscribe() method which is inherited from
         SubscriptionBase)
*****************************************************************************/
void
ContactStateOverride::activate()
{
  //  Identify whether the bodies are still attached -- i.e. in the same
  //  mass-tree.
  //  NOTE -- this could be temporary in the case where a detach is pending
  //          application, so this value needs checking every cycle.
  bodies_are_attached =
     (reference_body.get_root_body() == override_body.get_root_body());

  // Compute the separation-state between the two structure frames at
  // activation to use as a reference point.
  jeod::RefFrameState sep_state;
  override_body.structure.compute_relative_state(
                               reference_body.structure,
                               sep_state);

  // Unless the user has specified a contact normal, we will use the vector
  // aligned with the vector between the core-body centres of mass of the
  // two bodies.
  if (!user_set_contact_normal) {
    double scratch[3];
    // Calculate position of override body's CM relative to reference body's CM,
    // inertial frame
    jeod::Vector3::diff(override_body.core_body.state.trans.position,
                  reference_body.core_body.state.trans.position,
                  scratch);
    // Transform to reference body's structure frame
    jeod::Vector3::transform( reference_body.structure.state.rot.T_parent_this,
                        scratch,
                        contact_normal);
    jeod::Vector3::normalize(contact_normal);
  }

  /* Unless the user has specified a contact orientation, we will use the
     relative orientation of the two structure-frames at activation as the
     target.
  */
  if (!user_set_contact_orientation) {
    contact_quat_refstruc_to_overridestruc.scalar =
                                             sep_state.rot.Q_parent_this.scalar;
    jeod::Vector3::copy ( sep_state.rot.Q_parent_this.vector,
                    contact_quat_refstruc_to_overridestruc.vector);
  }

  // Unless the user has specified a contact position, we will use the
  // relative positioning of the two bodies at activation as the target
  // override position. Relative position is measured using the core-body
  // centres of masses' positions in their respective structure frames at the
  // time of activation (because the structure origins could be far away from
  // the bodies).
  if (!user_set_contact_position) {
    jeod::Vector3::copy( sep_state.trans.position,
                   contact_pos_overridestruc_wrt_refstruc);
  }
  // Also record the following position-vectors for later use:
  //    reference-body-core-body wrt reference-body-structure
  //     override-body-core-body wrt override-body-structure
  //     override-body-core-body wrt reference-body-core-body, expressed in
  //       reference-body-structure
  jeod::Vector3::copy( reference_body.mass.core_properties.position,
                 nominal_reference_core_position);
  jeod::Vector3::copy( override_body.mass.core_properties.position,
                 nominal_override_core_position);
  double scratch[3];
  jeod::Quaternion conjugate_;
  contact_quat_refstruc_to_overridestruc.conjugate( conjugate_);
  conjugate_.left_quat_transform( nominal_override_core_position,
                                  scratch);
  /* scratch = position of nominal override-body core CoM relative to
               override-body struc-frame, expressed in reference-body
               struc-frame.*/
  jeod::Vector3::incr( contact_pos_overridestruc_wrt_refstruc,
                 scratch);
  /* scratch = position of nominal override-body core CoM relative to
               reference-body struc-frame, expressed in reference-body
               struc-frame.*/
  jeod::Vector3::diff( scratch,
                 nominal_reference_core_position,
                 contact_pos_overridenomcore_wrt_refnomcore);
  active = true;
}

/*****************************************************************************
set_contact_normal
Purpose:(Allows the user to input the vector perpendicular to the contact
         plane, expressed in the reference-body's structural reference frame..
         The vector is normalized and a flag is set so it isn't overwritten
         during activation.)
*****************************************************************************/
void
ContactStateOverride::set_contact_normal(double contact_normal_in[3])
{
  if (active) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Modified an active contact model\n",
      "This contact model is currently active and the contact-normal is\n"
      "being manually reset.  This can have unexpected consequences for the\n"
      "computation of the separation distance.\n"
      "Proceed with caution.");
  }

  // Make sure the provided vector is a unit-vector:
  jeod::Vector3::normalize(contact_normal_in, contact_normal);
  // Block automatic override of this assigned value:
  user_set_contact_normal = true;
}

/*****************************************************************************
set_contact_position
Purpose:(Allows the user to input the desired position of the override-body's
         structural frame relative to the reference-body's structural frame,
         expressed in the reference-body's structural frame.
         A flag is set so this value is not overwritten with the value obtained
         automatically during activation.)
*****************************************************************************/
void
ContactStateOverride::set_contact_position(
    double contact_position[3])
{
  if (active)
  {
    CMLMessage::error(__FILE__, __LINE__,
      "Attempted to modify an active model\n",
      "This contact model is currently active and the contact position is\n"
      "already defined.  It is not permissible to change the "
      "contact-position\nat this time because that could result in the\n"
      "override body being instantaneously snapped to another position.\n"
      "Contact position will remain as it was before.\n");
      return;
  }

  jeod::Vector3::copy(contact_position,
                contact_pos_overridestruc_wrt_refstruc);
  user_set_contact_position = true;
}

/*****************************************************************************
set_contact_orientation
Purpose:(
  Allows the user to input a T-matrix or left-transformation quaternion
  representing the orientation of the override-body's structure frame wrt
  the reference-body's structure frame.
  A flag is set so this orientation is not overwritten with that
  obtained automatically during activation.)
Note:
  Overloading set_contact_orientation with the two arguments confuses SWIG,
  so the quaternion form is identified as *_Q.
*****************************************************************************/
void
ContactStateOverride::set_contact_orientation(
    double contact_orientation[3][3])
{
  if (active)
  {
    CMLMessage::error(__FILE__, __LINE__,
      "Attempted to modify an active model\n",
      "This contact model is currently active and the contact orientation is\n"
      "already defined.  It is not permissible to change the "
      "contact-orientation\nat this time because that could result in the\n"
      "override body being instantaneously snapped to another orientation.\n"
      "Contact orientation will remain as it was before.\n");
      return;
  }
  contact_quat_refstruc_to_overridestruc.left_quat_from_transformation(
                                                         contact_orientation);
  user_set_contact_orientation = true;
}
/****************************************************************************/
void
ContactStateOverride::set_contact_orientation_Q(
    const jeod::Quaternion & contact_orientation)
{
  if (active)
  {
    CMLMessage::error(__FILE__, __LINE__,
      "Attempted to modify an active model\n",
      "This contact model is currently active and the contact orientation is\n"
      "already defined.  It is not permissible to change the "
      "contact-orientation\nat this time because that could result in the\n"
      "override body being instantaneously snapped to another orientation.\n"
      "Contact orientation will remain as it was before.\n");
      return;
  }

  contact_quat_refstruc_to_overridestruc.scalar = contact_orientation.scalar;
  jeod::Vector3::copy ( contact_orientation.vector,
                  contact_quat_refstruc_to_overridestruc.vector);
  user_set_contact_orientation = true;
}

/*****************************************************************************
set_deactivation_threshold
Purpose:(sets the separation distance at which the model will self-deactivate)
*****************************************************************************/
void
ContactStateOverride::set_deactivation_threshold( double threshold)
{
  if (threshold < 0.0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Illegal value\n",
      "Attempt to set the deactivation_threshold to ",threshold," FAILED.\n"
      "The deactivation threshold must be greater than or equal to 0.0.\n"
      "A value less than zero implies that the model is no longer needed at\n"
      "the instant of its own activation."
      "deactivation_threshold remains at its current setting of ",
      deactivation_threshold);
  }
  else {
    deactivation_threshold = threshold;
  }
}
