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

#include "jeod/models/utils/ref_frames/include/ref_frame.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

/**
 * Defines a reference frame (the Pointing Frame) based on two other reference
 * frames: the Originating Frrame and the Target Frame
 *
 * The Pointing Frame is then defined by:
 *  - origin at the origin of the Originating Frame
 *  - x axis: Along the line from the origin of the Originating Frame
 *            to the origin of the Target Frame.
 *  - y-axis: Completes the orthogonal basis.
 *  - z-axis: Aligned with the angular momentum vector resulting from the
 *            relative linear motion of the Target Frame with respect to
 *            the Originating Frame.
 *  - Attitude-rate will always be on the local z-axis with value necessary for
 *    the x-axis to track the motion of the Target Frame.
 * The frame is typically used to describe a vehicle state relative to a line
 * joining two bodies such as planets. A common application would be a
 * Synodic Frame, such as the Earth-Moon rotating frame.
 *
 * Assumptions:
 *  - Both the Originating Frame and Target Frame must be registered in the
 *    simulation's frame-manager to provide a mechanism for deriving the
 *    relative state between them.
 *
 * Design Considerations:
 *  - Originating Frame and Target Frame are pointers rather than references
 *    to allow for assignment of the reference frames after construction.
 *  - The jeod::RefFrame instance is a class member (has-a) rather than making
 *    this class a derivation of jeod::RefFrame (is-a). This choice is driven by
 *    a desire to avoid conflict between the two competing subscription
 *    mechanisms found in CML and JEOD. It was considered preferable to use the
 *    CML subscription pattern for the class implementation and to trigger the
 *    JEOD subscription process from the CML subscription process.
 */
class PointingRefFrame : public SubscriptionBase {
 protected:
  jeod::RefFrame * originating_frame {nullptr}; /**< (--)
      Pointer to the Originating Frame. Note this cannot be const due to
      subscribe/unsubscribe operations, but the PointingRefFrame class should
      not influence any other aspect of this RefFrame instance.*/
  jeod::RefFrame * target_frame {nullptr}; /**< (--)
      Pointer to the Target Frame. Note this cannot be const due to
      subscribe/unsubscribe operations, but the PointingRefFrame class should
      not influence any other aspect of this RefFrame instance.*/

 public:
  jeod::RefFrame pointing_frame; /**< (--) The generated reference frame */
  jeod::RefFrameState target_wrt_originating_state; /**< (--)
      The state of the Target Frame with respect to the Originating Frame.
      Exists as a class member for logging purposes only, elements of this
      instance are used to define the state of the pointing-ref-frame
      itself.*/

  /**
   * Constructor
   */
  PointingRefFrame();

  /**
   * Copy constructor deleted
   */
  PointingRefFrame (const PointingRefFrame&) = delete;

  /**
   * Copy assignment operator deleted
   */
  PointingRefFrame & operator = (const PointingRefFrame&) = delete;

  /**
   * Set the Originating Frame pointer
   *
   * After calling @ref SubscriptionBase::initialize, the Originating Frame may
   * not be changed.
   *
   * @param originating_frame Non-null pointer to the Originating Frame
   */
  void set_originating_frame (jeod::RefFrame * originating_frame);

  /**
   * Set the Target Frame pointer
   *
   * After calling @ref SubscriptionBase::initialize, the Target Frame may not
   * be changed.
   *
   * @param target_frame Non-null pointer to the Target Frame
   */
  void set_target_frame (jeod::RefFrame * target_frame);

  /**
   * Update the state of the Pointing Frame
   */
  virtual void update();

 protected:
  /**
   * Add the Pointing Frame as a child of the Originating Frame and ensure that
   * the Originating Frame and Target Frame are valid and subscribed
   */
  bool setup_frames();

  /**
   * @ref SubscriptionBase::activate "Activate" the model
   */
  void activate() override;

  /**
   * @ref SubscriptionBase::deactivate "Deactivate" the model
   */
  void deactivate() override;
};
#endif
