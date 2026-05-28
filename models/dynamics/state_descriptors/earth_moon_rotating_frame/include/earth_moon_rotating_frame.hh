/*******************************************************************************
Purpose:
  (Define the class EarthMoonRotatingFrame, the class used for
   expressing the subject DynBody's state in the Synodic Earth-Moon
   Rotating Frame.
   This model uses the EphemBasedPointingRefFrame model to define the
   Synodic frame and adds configuration information to make the Synodic
   frame easily configurable.

Library dependencies:
  ((../src/earth_moon_rotating_frame.cc))

Programmers:
  ( ((Gary Turner) (OSR) (Aug 2019) (Antares) (initial))
  )

*******************************************************************************/
#ifndef CML_EARTH_MOON_ROTATING_FRAME_HH
#define CML_EARTH_MOON_ROTATING_FRAME_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/dynamics/state_descriptors/pointing_ref_frame/include/ephem_based_pointing_ref_frame.hh"

/*****************************************************************************
EarthMoonRotatingFrame
Purpose:(Contains configuration data and a PointingRefFrame instance
         representing a frame rotating with the Earth-Moon system.)
*****************************************************************************/
class EarthMoonRotatingFrame : public EphemBasedPointingRefFrame {

 // Member data
 public:
  enum FrameOrientation {
    Earth_Moon           = 1, // x-axis is from Earth to Moon, origin at Earth
    Earth__Moon_to_Earth = 2, // x-axis is from Moon to Earth, origin at Earth
    EMB_Moon             = 3, // x-axis is from EMBary to Moon, origin at EMBary
    EMB_Earth            = 4, // x-axis is from EMBary to Earth, origin at EMBary
    Moon_Earth           = 5, // x-axis is from Moon to Earth, origin at Moon.
    Moon__Earth_to_Moon  = 6  // x-axis is from Earth to Moon, origin at Moon.
  };
 protected:
  FrameOrientation orientation; /* (--)
       Sets the orientation and origin of the frame.*/
  bool reverse_xy; /* (--)
       Flag indicates the necessity to reverse the x and y-axes to
       support Earth__Moon_to_Earth and Moon__Earth_to_Moon.*/

 public:
  // Constructor and destructor
  explicit EarthMoonRotatingFrame (jeod::EphemeridesManager & ephem_manager);
  virtual ~EarthMoonRotatingFrame(){};

  virtual void initialize();
  virtual void update();
  void set_orientation(FrameOrientation orientation);

 // The copy constructor and assignment operator for this class are
 // declared private and are not implemented.
 private:
   EarthMoonRotatingFrame (const EarthMoonRotatingFrame&);
   EarthMoonRotatingFrame & operator = (const EarthMoonRotatingFrame&);
};
#endif
