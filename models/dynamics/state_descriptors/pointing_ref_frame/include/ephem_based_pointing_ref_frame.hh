/*******************************************************************************
Purpose:
  (Define the class EphemBasedPointingRefFrame.
   This is an almost trivial extension of PointingRefFrame, adding a
   reference to the Ephemerides Manager to ensure that the necessary
   frames are updated before updating the pointing-frame.)

Programmers:
  ( ((Gary Turner) (OSR) (July 2019) (Antares) (initial))
  )

*******************************************************************************/
#ifndef CML_EPHEM_BASED_POINTING_REF_FRAME_HH
#define CML_EPHEM_BASED_POINTING_REF_FRAME_HH

#include "jeod/models/environment/ephemerides/ephem_manager/include/ephem_manager.hh"
                                                        // EphemeridesManager
#include "pointing_ref_frame.hh"

/*****************************************************************************
EphemBasedPointingFrame
Purpose:(
    Adds the Ephemerides Manager to the simplest implementation of a
    PointingRefFrame.
    This version should be used when the states of either the originating
    frame or the target frame are known only by updates coming from the
    Ephemeris Manager.)
*****************************************************************************/
class EphemBasedPointingRefFrame : public PointingRefFrame
{
 protected:
  jeod::EphemeridesManager & ephem_manager; /* (--) Reference to the
                                               Ephemerides Manager*/
  virtual void activate()
  {
    if( setup_frames()) {
      ephem_manager.update_ephemerides();
      active = true;
      update();
    }
  }

 public:
  EphemBasedPointingRefFrame(jeod::EphemeridesManager & mgr)
    :
    PointingRefFrame(),
    ephem_manager(mgr)
  {};
  virtual ~EphemBasedPointingRefFrame(){};


  virtual void initialize()
  {
    // Register the frame with the dynamics/ephem manager so that it can be
    // used to represent the state of a vehicle
    ephem_manager.add_ref_frame( pointing_frame);
    SubscriptionBase::initialize();
  }

 // The copy constructor and assignment operator for this class are
 // declared private and are not implemented.
 private:
  EphemBasedPointingRefFrame (const EphemBasedPointingRefFrame&);
  EphemBasedPointingRefFrame & operator = (const EphemBasedPointingRefFrame&);
};
#endif
