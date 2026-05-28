/*******************************************************************************
Purpose:
  (Define methods for configuring an EphemBasedPointingRefFrame for the
   Earth-Moon system.)

Programmers:
  ( ((Gary Turner) (OSR) (Aug 2019) (Antares) (initial))
  )

*******************************************************************************/

#include <string>
#include "jeod/models/utils/math/include/vector3.hh"

#include "../include/earth_moon_rotating_frame.hh"

/*******************************************************************************
Constructor
Purpose:  (Construct a EarthMoonRotatingFrame object.)
*******************************************************************************/
EarthMoonRotatingFrame::EarthMoonRotatingFrame (
     jeod::EphemeridesManager & ephem_mgr)
  :
  EphemBasedPointingRefFrame(ephem_mgr),
  orientation(EMB_Moon),
  reverse_xy(false)
{ }

/*******************************************************************************
Function: EarthMoonRotatingFrame::initialize
Purpose:  (Begin initialization of a EarthMoonRotatingFrame.)

NOTE - using ephem-manager here; technically we only need the
       ref-frame-manager, but the ephem-manager IS the ref-frame-manager
       (and then some) and the parent model needs the ephem-manager elsewhere
       so using the inherited reference here also.
*******************************************************************************/
void
EarthMoonRotatingFrame::initialize ()
{
  reverse_xy = false;
  std::string frame_name;
  jeod::RefFrame * origin = NULL;
  jeod::RefFrame * target = NULL;
  switch (orientation) {
  case Earth__Moon_to_Earth:
    reverse_xy = true;
    // fallthrough to Earth_Moon:
    [[fallthrough]];
  case Earth_Moon:
    frame_name.assign("Earth.em_rot");
    origin = ephem_manager.find_ref_frame("Earth.inertial");
    target = ephem_manager.find_ref_frame("Moon.inertial");
    break;
  case EMB_Moon:
    frame_name.assign("EMBary.em_rot");
    origin = ephem_manager.find_ref_frame("EMBary.inertial");
    target = ephem_manager.find_ref_frame("Moon.inertial");
    break;
  case EMB_Earth:
    frame_name.assign("EMBary.em_rot");
    origin = ephem_manager.find_ref_frame("EMBary.inertial");
    target = ephem_manager.find_ref_frame("Earth.inertial");
    break;
  case Moon__Earth_to_Moon:
    reverse_xy = true;
    // fallthrough to Moon_Earth:
    [[fallthrough]];
  case Moon_Earth:
    frame_name.assign("Moon.em_rot");
    origin = ephem_manager.find_ref_frame("Moon.inertial");
    target = ephem_manager.find_ref_frame("Earth.inertial");
    break;
  }

  // check that necessary frames are registered with the ref-frame manager
  if ((origin == NULL) || (target == NULL)) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Incomplete specification",
      "The origin and/or target of the Earth-Moon-Rotating-Frame were not\n"
      "found by the Ephemerides Manager.\n"
      "Make sure that Earth, Moon, and EMBary are in the Ephemeris tree.\n");
    return;
  }

  set_originating_frame(origin);
  set_target_frame(target);
  pointing_frame.set_name (frame_name);

  // Push the initialization up to the EphemBasedPointingRefFrame parent class
  EphemBasedPointingRefFrame::initialize();
}

/*****************************************************************************
set_orientation
Purpose:(Configures the model)
*****************************************************************************/
void
EarthMoonRotatingFrame::set_orientation(
     FrameOrientation orient)
{
  if (initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"Reconfiguration error",
      "Once activated, the EarthMoonRotatingFrame cannot change its preferred "
      "configuration.\nModel remains at its current setting.");
    return;
  }
  orientation = orient;
}


/*****************************************************************************
update
Purpose:(Main update method)
*****************************************************************************/
void
EarthMoonRotatingFrame::update()
{
  EphemBasedPointingRefFrame::update();
  if (reverse_xy) {
    // The transformation matrix to rotate by pi about z-axis is:
    //   [[-1,0,0],[0,-1,0],[0,0,1]]
    // The effect of pre-multiplying the T_parent_this matrix is to negate the
    // top two rows.
    jeod::Vector3::negate(pointing_frame.state.rot.T_parent_this[0]);
    jeod::Vector3::negate(pointing_frame.state.rot.T_parent_this[1]);
    pointing_frame.state.rot.compute_quaternion();
  }
}
