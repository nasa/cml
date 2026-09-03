/*******************************************************************************
Purpose:
  (Define the class EphemBasedPointingRefFrame.
   This is an almost trivial extension of PointingRefFrame, adding a
   reference to the Ephemerides Manager to ensure that the necessary
   frames are updated before updating the Pointing Frame.)

Programmers:
  ( ((Gary Turner) (OSR) (July 2019) (Antares) (initial))
  )

*******************************************************************************/
#ifndef CML_EPHEM_BASED_POINTING_REF_FRAME_HH
#define CML_EPHEM_BASED_POINTING_REF_FRAME_HH

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "jeod/models/environment/ephemerides/ephem_manager/include/ephem_manager.hh"
#include "pointing_ref_frame.hh"

/**
 * Extension of the @ref PointingRefFrame for cases where the Originating Frame
 * or Target Frame are ephemeris-based frames.
 *
 * This version should be used when the states of either the Originating Frame
 * or the Target Frame are known only by updates coming from the Ephemeris
 * Manager.
 */
class EphemBasedPointingRefFrame : public PointingRefFrame
{
 protected:
  jeod::EphemeridesManager & ephem_manager; /* (--) Reference to the
                                               Ephemerides Manager*/
  /**
   * @ref PointingRefFrame::setup_frames "Sets up reference frames" and tells the
   *      JEOD Ephemeris Manager to update the ephemerides
   */
  void activate() override
  {
    if( setup_frames()) {
      ephem_manager.update_ephemerides();
      active = true;
      update();
    }
  }

 public:
  /**
   * Constructor
   *
   * @param mgr Reference to the JEOD Ephemeris Manager, which for most
   *            simulations will be the `jeod::DynManager` instance
   */
  explicit EphemBasedPointingRefFrame(jeod::EphemeridesManager & mgr)
    :
    ephem_manager(mgr)
  {}

  /**
   * Copy constructor deleted
   */
  EphemBasedPointingRefFrame (const EphemBasedPointingRefFrame&) = delete;

  /**
   * Copy assignment operator deleted
   */
  EphemBasedPointingRefFrame & operator = (const EphemBasedPointingRefFrame&) = delete;

  /**
   * Register the Pointing Frame with the dynamics/ephem manager so that it can
   * be used to represent the state of a vehicle
   */
  void initialize() override
  {
    // Register the frame with the dynamics/ephem manager so that it can be
    // used to represent the state of a vehicle
    ephem_manager.add_ref_frame( pointing_frame);
    SubscriptionBase::initialize();
  }
};
#endif