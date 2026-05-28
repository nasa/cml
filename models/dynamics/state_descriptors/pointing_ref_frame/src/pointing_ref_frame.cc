/*******************************************************************************
Purpose:
  (Define methods for defining and updating the pointing frame.)

Programmers:
  ( ((Michael McCarthy, Gary Turner) (OSR) (June 2019) (Antares) (initial))
  )

*******************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/pointing_ref_frame.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
PointingRefFrame::PointingRefFrame()
  :
  originating_frame(NULL),
  target_frame(NULL),
  pointing_frame(),
  target_wrt_originating_state()
{
  // Initialize the frame's state:
  // The position and velocity will always be 0
  // The attitude will be updated regularly
  // The attitude rate will always be along the frame's z-axis
  jeod::Vector3::initialize( pointing_frame.state.trans.position);
  jeod::Vector3::initialize( pointing_frame.state.trans.velocity);
  jeod::Vector3::unit (2,    pointing_frame.state.rot.ang_vel_unit);
  jeod::Matrix3x3::identity( pointing_frame.state.rot.T_parent_this);
  pointing_frame.state.rot.Q_parent_this.make_identity();
}

/*****************************************************************************
set_originating_frame
Purpose:(sets the originating_frame pointer)
*****************************************************************************/
void
PointingRefFrame::set_originating_frame(
    jeod::RefFrame * orig_frame)
{
  if (active) {
    CMLMessage::error(
      __FILE__,__LINE__,"Reconfiguration error\n",
      "Once activated, the PointingRefFrame ", pointing_frame.get_name(), " cannot change its originating "
      "frame.\nOriginating-frame remains at its current setting.");
    return;
  }
  if (orig_frame == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Configuration error\n",
      "Attempt to assign the originating-frame of PointingRefFrame ", pointing_frame.get_name(), " to be "
      "NULL.\nThis is not a valid setting.\nAttempt failed.\n");
    return;
  }
  originating_frame = orig_frame;
}

/*****************************************************************************
set_target_frame
Purpose:(sets the target_frame pointer)
*****************************************************************************/
void
PointingRefFrame::set_target_frame(
    jeod::RefFrame * targ_frame)
{
  if (active) {
    CMLMessage::error(
      __FILE__,__LINE__,"Reconfiguration error\n",
      "Once activated, the PointingRefFrame ", pointing_frame.get_name(), " cannot change its target "
      "frame.\nOriginating-frame remains at its current setting.");
    return;
  }
  if (targ_frame == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Configuration error\n",
      "Attempt to assign the target-frame of PointingRefFrame ", pointing_frame.get_name(), " to be "
      "NULL.\nThis is not a valid setting.\nAttempt failed.\n");
    return;
  }
  target_frame = targ_frame;
}

/*******************************************************************************
update
Purpose:  (Updates the state of the pointing frame, specifically its
           orientation and angular rate.)
*******************************************************************************/
void
PointingRefFrame::update()
{
  if (!active) {
    return;
  }

  // Note -- need position and velocity relative state; relative attitude and
  // relative attitude rate are irrelevant.
  // The easiest method of obtaining p & v is compute_relative_state.
  target_frame->compute_relative_state( *originating_frame,
                                         target_wrt_originating_state);

  double axis_scratch[3];

  // check distance between orig. and target
  double pos_r = jeod::Vector3::vmag( target_wrt_originating_state.trans.position );
  if (MathUtils::is_near_equal( pos_r, 0.0, 1.0)) {
    // if pos = 0, T = Tprev (= Identity for first update) and ang_vel_this = 0
    CMLMessage::warn(
      __FILE__,__LINE__, "Proximity Error\n",
      "The relative position vector is zero, resulting in an undefined "
      "pointing reference frame.\n"
      "Pointing ref-frame orientation will instead be retained from the most "
      "recent successful calculation.\n"
      "Pointing ref-frame will be assigned zero angular rate relative to the "
      "originating frame.\n");
    pointing_frame.state.rot.ang_vel_this[2] = 0;
    return;
  }
  // else:
  // x-axis
  jeod::Vector3::normalize( target_wrt_originating_state.trans.position,
                      pointing_frame.state.rot.T_parent_this[0]);
  // z-axis
  jeod::Vector3::cross( target_wrt_originating_state.trans.position,
                  target_wrt_originating_state.trans.velocity,
                  axis_scratch);
  // Check for the target frame having a radial relative velocity; in this
  // case the z-axis is undefined:
  double vmag = jeod::Vector3::vmag(axis_scratch);
  if (MathUtils::is_near_equal( vmag, 0.0, 1.0)) {
    // Alternative #1:
    // use the last known y-axis in place of the velocity vector, this
    // should keep the z-axis close to where it should be:
    jeod::Vector3::cross( target_wrt_originating_state.trans.position,
                    pointing_frame.state.rot.T_parent_this[1],
                    axis_scratch);
    // but also check for the (rather unlikely) possibility that the old  
    // y-axis and new x-axis are perfectly aligned:
    vmag = jeod::Vector3::vmag(axis_scratch);
    // if that doesn't happen, use alternative #1; drop a warning and move on:
    if (vmag > 0.0) {
      CMLMessage::warn(
        __FILE__,__LINE__, "Alignment of r and v vectors\n",
        "The relative position and velocity vectors between the target "
        "and originating frames are aligned.\nUsing the relative position and "
        "most recent y-axis to define pointing-ref-frame z-axis.");
    }
    // otherwise, try alternative #2, based on the old-z:
    else {
      // (note: z-old and x-new cannot be aligned because at this point,
      //   x-new is aligned with y-old.
      // z-new = x-new x (z-old x x-new)
      double axis_scratch2[3];
      jeod::Vector3::cross( pointing_frame.state.rot.T_parent_this[2],// old z
                      target_wrt_originating_state.trans.position,// new x
                      axis_scratch2);
      jeod::Vector3::cross( target_wrt_originating_state.trans.position,// new x
                      axis_scratch2,
                      axis_scratch);

      // Send message
      CMLMessage::warn(
        __FILE__,__LINE__, "Alignment of r and v vectors\n",
        "The relative position, velocity, and the most recent y-axis are "
        "aligned.\nComputing z-axis from most recent z-axis value.");
    }
  }
  // normalize axis-scratch -- however it was obtained -- and assign to z-axis:
  jeod::Vector3::normalize(axis_scratch,
                     pointing_frame.state.rot.T_parent_this[2]);

  // y-axis
  jeod::Vector3::cross( pointing_frame.state.rot.T_parent_this[2],
                  pointing_frame.state.rot.T_parent_this[0],
                  axis_scratch);
  // normalize axis-scratch and assign to y-axis:
  jeod::Vector3::normalize(axis_scratch,
                     pointing_frame.state.rot.T_parent_this[1]);

  // Angular rate of the state will always lie on its own z-axis (by definition)
  // so omega-vec = [0, 0, omega]
  // Similarly, position of target lies on x-axis only (by definition), so
  //   r-vec = [r, 0, 0]
  // Thus, omega-vec x r-vec = [ 0 , omega r, 0]
  // v-vec = dot-r-vec + omega-vec x r-vec
  // [u v w]  =  [ dot r, omega r, 0]
  // Thus the magnitude of the angular velocity can be obtained by dividing:
  //  -  (the y-component of the relative velocity of the target frame wrt
  //      the originating frame) by the
  //  -  (relative distance between the originating frame and the target frame.)

  // Evaluation of v/r would be really difficult to fail, pos_r has been
  // checked for non-zero, which would mean that the velocity would have to be
  // extraordinarily large and pos_r very small to combine to produce an
  // overflow.  In that case, just use the previous value for omega.
  pointing_frame.state.rot.ang_vel_this[2] = MathUtils::divide_protected(
           jeod::Vector3::dot(target_wrt_originating_state.trans.velocity,
                        pointing_frame.state.rot.T_parent_this[1]),
           pos_r, // none-zero, already checked.
           pointing_frame.state.rot.ang_vel_this[2],
           false);

  // use T-mx to generate quaternion.
  pointing_frame.state.rot.compute_quaternion();
}


/*****************************************************************************
setup_frames
Purpose:(run from model activation; common base method for this class and
         its derivatives.)
*****************************************************************************/
bool
PointingRefFrame::setup_frames()
{
  if (target_frame == NULL || originating_frame == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Incomplete specification\n",
      "The target-frame and/or originating-frame of the\n"
      "Pointing_Reference-Frame have not been assigned.\n"
      "The Pointing-Reference-Frame ", pointing_frame.get_name(), " cannot be activated.\n");
    return false;
  }
  target_frame->subscribe();
  originating_frame->subscribe();

  // Add this frame to the tree as a child of the originating frame just
  // identified.
  if (!pointing_frame.is_progeny_of( *originating_frame)) {
    originating_frame->add_child(pointing_frame);
  }
  pointing_frame.subscribe();
  return true;
}

/*****************************************************************************
activate
Purpose:(Activates the model when its subscription count rises from 0 to 1)
*****************************************************************************/
void
PointingRefFrame::activate()
{
  // setup_frames does NULL checks.  If these pass, model is good to go.
  if (setup_frames()) {
    active = true;
    update();
  }

  /* NOTE
     if this instance relies on Ephemeris Frames for its originating-frame
     and/or target-frame, it may also be necessary to rebuild the Ephemeris
     tree before being able to successfully update the pointing-frame.
     The Ephemeris tree maintains a minimal set of frames, including only
     those frames necessary for the current sim configuration.  So if either
     of the two frames subscribed as part of setup_frames() is an Ephemeris
     Frame and WAS NOT previously subscribed, its state may be old or,
     more likely, the frame may not even exist in the reference-frame tree.
     The consequence is that attempting to update the relative state between
     the two frames will at best produce stale data, but most likely result
     in early termination of the sim.

     The solution to this is to simply rebuild the Ephemeris tree before
     executing the frame update.

     However, this simple instance does not contain a reference to the
     Ephemerides Manager.  This was deliberate to keep this class's
     dependencies small.  If either of the two frames are Ephemeris Frames,
     the derived EphemBasedPointingRefFrame should be used instead of this
     simple PointingRefFrame class.
     The only difference between these two classes is the presence of a
     reference to the Ephemerides manager, and the presence in the
     EphemBasedPointingRefFrame::activate() method of a call to rebuild
     the tree before calling the frame update.
  */
}



/*****************************************************************************
deactivate
Purpose:(Deactivates the model when its subscription count drops to 0)
*****************************************************************************/
void
PointingRefFrame::deactivate()
{
  originating_frame->unsubscribe();
  target_frame->unsubscribe();
  pointing_frame.unsubscribe();
  active = false;
}
