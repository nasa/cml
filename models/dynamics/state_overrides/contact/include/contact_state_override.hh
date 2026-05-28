/******************************** TRICK HEADER **********************************
PURPOSE:
   (The model provides a simple state override mechanism, avoiding the need to
   generate and integrate contact forces.  A nominal offset is provided between
   two bodies; if their relative sseparation is less than this nominal offset,
   the bodies are assumed to be in contact and their relative state is
   overridden such that it is equal to the nominal separation.)

ASSUMPTIONS:
  ((The two bodies are not deformable.  If they contact, the contact force
   between them is unbounded, ensuring that their relative motion stops when
   the separation hits the nominal separation.)
  (The default normal to the plane separating the two bodies is generated as
   being aligned with the vector between the centers-of-mass of the two bodies
   (NOTE - using the core-body center-of-mass, not the composite-body)
   This normal can be manually overridden with the set_contact_normal(...)
   method prior to activation.))

LIMITATION:
  ((When used during separation of two bodies, the override mechanism
    uses the state of one body (the reference-body) to override the state of
    the other (the override-body).
    Any forces and torques applied to the reference-body will generate
    accelerations based on its mass-properties only.  The effects of forces
    and torques applied to the override-body will be lost when its state is
    overridden.)
   (When used during approach contact, similar limitations apply)
   (In neither case is the conservation of either momentum or angular
    momentum considered as a constraint; these laws are followed during the
    attach and detach mechanisms themselves, but not in the state-overrides
    that immediately precede or follow those actions.))

LIBRARY DEPENDENCIES:
   ((../src/contact_state_override.cc))

PROGRAMMERS:
   (((Daniel Ghan) (OSR) (Mar 2020) (Antares) (Initial version)))

********************************************************************************/
#ifndef CML_STATE_OVERRIDE_CONTACT_MODEL_HH
#define CML_STATE_OVERRIDE_CONTACT_MODEL_HH

#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "jeod/models/utils/quaternion/include/quat.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

class ContactStateOverride : public SubscriptionBase
{
 protected:
  // External references
  jeod::DynBody& reference_body; /* (--) Baseline body from which the
                                             relative state is measured. */
  jeod::DynBody& override_body; /* (--)
       The body whose state will be overridden to be at the fixed relative
       location as long as the body's position is computed to be at a
       physically impossible location.*/

  bool contact_detected; /* (--)
    Flag set to indicate that the contact plane has been broken.*/

  double deactivation_threshold; /* (m)
      When the objects are this far apart, this model will deactivate itself. */
  double contact_normal[3];  /* (--)
      Unit vector perpendicular to contact plane, expressed in the
      reference-body's structgure frame.*/
  double contact_pos_overridestruc_wrt_refstruc[3]; /* (m)
      Origin of override structure frame relative to origin of reference
      structure frame, coordinatized in the reference structure frame, when the
      bodies are in contact. Used for state override. */
  double contact_pos_overridenomcore_wrt_refnomcore[3]; /* (m)
      Nominal override core position with respect to nominal reference core
      position, coordinatized in the reference structure frame, when the bodies
      are in contact. Used for testing for contact. */
  jeod::Quaternion contact_quat_refstruc_to_overridestruc; /* (--)
      Left-transformation quaternion from the structural frame of
      reference_body to the structural frame of override_body
      when the bodies are in contact. */
  double nominal_reference_core_position[3]; /* (m)
      Location in the reference body used for testing for contact, coordinatized
      in the reference structure frame */
  double nominal_override_core_position[3]; /* (m)
      Location in the override body used for testing for contact, coordinatized
      in the override structure frame */
  bool user_set_contact_normal; /* (--)
      Set when a user sets the normal direction manually; prevents the user-set
      contact_normal from being overwritten with the automatically-generated
      default value during activation. Default: false */
  bool user_set_contact_position; /* (--)
      Set when a user sets the contact position manually; prevents the
      user-defined position from being overwritten with the
      automatically-generated default value during activation.
      Default: false */
  bool user_set_contact_orientation; /* (--)
      Set when a user sets the contact orientation manually; prevents the
      user-defined orientation from being overwritten with the
      automatically-generated default value during activation.
      Default: false */
  bool bodies_are_attached; /* (--)
		  True if the bodies are in the same mass tree. If so, the state will not be
		  overridden because the bodies are directly or indirectly attached. */

 public:
  bool self_disabled;  /* (--)
    Public-accessible flag that gets set when the model has self-deactivated.
    Needed because the model's active flag is protected by inheritance from
    SubscriptionBase.*/

  ContactStateOverride(jeod::DynBody& reference_body,
                       jeod::DynBody& override_body);

  void update();
  void set_contact_normal(double contact_normal_in[3]);
  void set_contact_position(double contact_position_in[3]);
  void set_contact_orientation(double contact_orientation[3][3]);
  void set_contact_orientation_Q(const jeod::Quaternion & contact_orientation);
  void set_deactivation_threshold(double threshold);

 protected:
  void override_state();
  void override_state_root( jeod::BodyRefFrame& integ_frame,
                            double pos_integ_frame_wrt_override_struc[3],
                            const jeod::Quaternion & override_struc_to_integ_frame);
  #ifndef SWIG // SWIG does not like the override keyword.
  void activate() override;
  #endif

 private:
  // Make class non-copyable
  ContactStateOverride(const ContactStateOverride&);
  ContactStateOverride& operator = (const ContactStateOverride&);
};
#endif
