/******************************** TRICK HEADER **********************************
PURPOSE:
   (Implements the methods in the SeparationState class)

PROGRAMMERS:
   (((Gary Turner) (OSR) (April 2014) (Antares) (Replaces SEPKIN))
    ((Gary Turner) (OSR) (December 2014) (Antares)
                           (Greater protection, added subscription concept))
   )

********************************************************************************/

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/math/include/vector3.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/separation_state.hh"

/********************************************************************************
Method: SeparationState
Purpose: (Constructor)
********************************************************************************/
SeparationState::SeparationState()
  :
  generate_euler_angles(true),
  rel_state(),
  rel_orientation(),
  separation_distance(0.0),
  separation_rate(0.0),
  frames_subscribed(false),
  source(NULL),
  subject(NULL)
{
  subscribe_name = "SeparationState:";

  rel_orientation.euler_sequence = jeod::Orientation::EulerZYX;
  jeod::Vector3::initialize(rel_orientation.euler_angles);
}
//*****************************************************************************
SeparationState::SeparationState( std::string name_)
  :
  SeparationState()
{
  subscribe_name += name_;
  subscribe_name += ":";
}


/********************************************************************************
Method: initialize
Purpose: (Initializes the class)
Notes
  - There are 4 different methods provided involving different combinations of
    known frames and known vehicle-point-names.
  - If an empty string is passed in for a point name, the model defaults to
    using the composite-body frame.
  - an empty string can be passed as a deliberately empty string ("") or by
    omitting the argument entirely.  However, note that ifor the 4-argument
    method with 2 optional strings, it isn't possible to omit source_name
    and then provide the subject_name; if one of the strings is missing, it
    will be processed as though the subject-name has been omitted.
  - in all cases, the source-body information is provided before the
    subject-body  information, and the frame names are provided last.
********************************************************************************/
void
SeparationState::initialize(
       jeod::RefFrame     & source_frame,
       jeod::BodyRefFrame & subject_frame)
{
  if (!enabled) {
    return;
  }
  source  = &source_frame;
  subject = &subject_frame;
  SubscriptionBase::initialize();
}
/*******************************************************************************/
void
SeparationState::initialize(
       jeod::DynBody & source_body,
       jeod::BodyRefFrame & subject_frame,
       std::string source_name)
{
  if (!enabled) {
    return;
  }
  subject = &subject_frame;
  // Check the source frame:
  // If an empty string is passed in, this defaults to the body's
  // composite-body frame.  Otherwise, the source body must have a vehicle
  // point named 'source_name'.  If it does, the source frame will be assigned
  // to the frame based on that vehicle point.  If no such point exists, the
  // model fails with a terminal fault.
  source = initialize_find_frame( source_body, source_name, "source");
  SubscriptionBase::initialize();
}
/*******************************************************************************/
void
SeparationState::initialize(
       jeod::RefFrame & source_frame,
       jeod::DynBody  & subject_body,
       std::string      subject_name)
{
  if (!enabled) {
    return;
  }
  source = &source_frame;
  // Check the subject frame:
  subject = initialize_find_frame( subject_body, subject_name, "subject");
  SubscriptionBase::initialize();
}
/*******************************************************************************/
void
SeparationState::initialize(
       jeod::DynBody & source_body,
       jeod::DynBody & subject_body,
       std::string source_name,
       std::string subject_name)
{
  if (!enabled) {
    return;
  }
  source = initialize_find_frame( source_body, source_name, "source");
  subject = initialize_find_frame( subject_body, subject_name, "subject");
  SubscriptionBase::initialize();
}
/*******************************************************************************/
jeod::BodyRefFrame *
SeparationState::initialize_find_frame(
       jeod::DynBody    & body,
       std::string  frame_name,
       std::string  body_type)
{
  jeod::BodyRefFrame * frame = NULL;
  if (frame_name.empty()) {
    frame = &body.composite_body;
  } else {
    frame = const_cast<jeod::BodyRefFrame *>(
                           body.find_vehicle_point(frame_name));
  }
  if (frame == NULL) {
    CMLMessage::fail (
     __FILE__, __LINE__, "separation_state INVALID NAME\n",
     "Could not find a vehicle point named ", frame_name, " on the ", body_type, " vehicle");
  }
  return frame;
}


/********************************************************************************
Method: update
Purpose: (Updates the state if necessary)
********************************************************************************/
void
SeparationState::update()
{
  if (!active) {
    return;
  }

  // Method is active and Frames are computed.
  // Compute the relative state between the source and subject.
  // Generate the euler angles from the relative state.
  subject->compute_relative_state(*source, rel_state);
  if (generate_euler_angles) {
    rel_orientation.set_transform(rel_state.rot.T_parent_this);
    rel_orientation.compute_euler_angles();
  }
  separation_distance = jeod::Vector3::vmag(rel_state.trans.position);
  if (separation_distance > 0.0) {
      separation_rate = MathUtils::divide_protected(
                              jeod::Vector3::dot (rel_state.trans.position,
                                                  rel_state.trans.velocity),
                              separation_distance);
  } else {
      separation_rate = 0.0;
  }
}

/*****************************************************************************
activate_internal
Purpose:(Switches the model on)
******************************************************************************/
void
SeparationState::activate()
{
  //  Ensure subscription to the BodyRefFrame reference frames (this makes
  //  the Dynmanager compute their state)
  if (!frames_subscribed) {
    source->subscribe();
    subject->subscribe();
    frames_subscribed = true;
  }

  active = true;
  update();
}

/*****************************************************************************
force_deactivate
Purpose:(Switches the model off)
******************************************************************************/
void SeparationState::deactivate()
{
  // unsubscribe to the BodyrefFrame reference frames. This stops thisi
  // method from requiring the  the Dynmanager to compute their state.
  if (frames_subscribed) {
    source->unsubscribe();
    subject->unsubscribe();
    frames_subscribed = false;
  }
  active = false;
}
