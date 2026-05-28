/*******************************************************************************
Purpose:
  (Define the class PointingRefFrame.
   The frame so defined is typically used to describe a vehicle state relative
   to a line joining two bodies such as planets.  A common application would be
   a Synodic Frame, such as the Earth-Moon rotating frame.

Library dependencies:
  ((../src/pointing_ref_frame.cc))

Programmers:
  ( ((Michael McCarthy, Gary Turner) (OSR) (June 2019) (Antares) (initial))
  )

*******************************************************************************/
#ifndef CML_POINTING_REF_FRAME_HH
#define CML_POINTING_REF_FRAME_HH

#include "jeod/models/utils/ref_frames/include/ref_frame.hh" // RefFrame
#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh" // jeod::RefFrameState
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
                                                           // SubscriptionBase


/*****************************************************************************
PointingRefFrame
Purpose:(
  Defines a reference frame (the pointing-frame) based on two other reference
  frames --
   - originating frame
   - target frame

  The pointing frame is then defined by:
    - origin at the origin of the originating frame
    - x axis - along the line from the origin of the originating frame
               to the origin of the target frame.
    - y-axis - completes
    - z-axis - aligned with the angular momentum vector resulting from the
               relative linear motion of the target frame with respect to
               the originating frame.
    - attitude-rate will always be on the local z-axis with value necessary for
      the x-axis to track the motion of the target frame.
   The frame is typically used to describe a vehicle state relative to a line
   joining two bodies such as planets.  A common application would be a
   Synodic Frame, such as the Earth-Moon rotating frame.

Assumptions:
  (Both the originating frame and target frame must be registered in the
  simulation's frame-manager to provide a mechanism for deriving the
  relative state between them.)

Design Considerations:
  (Originating Frame and Target Frame are pointers rather than references
   because references would impose the requirement that these frames exist
   prior to the construction of this class instance.  That is an unnecessary
   requirement.)
  (The RefFrame instance is a class member (has-a) rather than making this
   class a derivation of RefFrame (is-a).  This choice is driven by a desire to
   avoid conflict between the two competing subscription mechanisms found in
   CML and JEOD.  It was considered preferable to use the CML subscription
   pattern for the class implementation and to trigger the JEOD subscription
   process from the CML subscription process.)
*****************************************************************************/
class PointingRefFrame : public SubscriptionBase {
 protected:
  jeod::RefFrame * originating_frame; /* (--)
      pointer to the originating frame.  Note this cannot be const due to
      subscribe/unsubscribe operations, but the PointingRefFrame class should
      not influence any other aspect of this RefFrame instance.*/
  jeod::RefFrame * target_frame; /* (--)
      pointer to the target frame.  Note this cannot be const due to
      subscribe/unsubscribe operations, but the PointingRefFrame class should
      not influence any other aspect of this RefFrame instance.*/

 public:
  jeod::RefFrame pointing_frame; /* (--) The generated reference frame */
  jeod::RefFrameState target_wrt_originating_state; /* (--)
      The state of the target frame with respect to the originating frame.
      Exists as a class member for logging purposes only, elements of this
      instance are used to define the state of the pointing-ref-frame
      itself.*/

  // Constructor:
  PointingRefFrame();
  virtual ~PointingRefFrame(){};

  void set_originating_frame (jeod::RefFrame * originating_frame);
  void set_target_frame (jeod::RefFrame * target_frame);

  virtual void update();

 protected:
  bool setup_frames();
  virtual void activate();
  virtual void deactivate();

 // The copy constructor and assignment operator for this class are
 // declared private and are not implemented.
 private:
  PointingRefFrame (const PointingRefFrame&);
  PointingRefFrame & operator = (const PointingRefFrame&);
};
#endif
