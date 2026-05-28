/******************************** TRICK HEADER **********************************

PURPOSE:
   (Defines the class that is used for computing relative states between two
   bodies, and expressing that in the LVLH frame of one of them.)

PROGRAMMERS:
   (((Gary Turner) (OSR) (October 2020) (Antares) (new)))

********************************************************************************/

#include "../include/lvlh_separation_state.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils

/*****************************************************************************
Constructor
*****************************************************************************/
LvlhSeparationState::LvlhSeparationState()
  :
  SeparationState(),
  lvlh(),
  planet_centered_inertial(nullptr),
  lvlh_origin_frame(nullptr),
  lvlh_ref(lvlh),
  using_external_lvlh(false)
{}
/****************************************************************************/
LvlhSeparationState::LvlhSeparationState( jeod::LvlhFrame & lvlh_)
  :
  SeparationState(),
  lvlh(),
  planet_centered_inertial(nullptr),
  lvlh_origin_frame(nullptr),
  lvlh_ref(lvlh_),
  using_external_lvlh(true)
{}


/*****************************************************************************
Destructor
*****************************************************************************/
LvlhSeparationState::~LvlhSeparationState()
{
  if (!using_external_lvlh) {
    // When LvlhFrame destructs, it will unsubscribe from the two frames
    // that this model has taken over management of.  So if we have
    // unsubscribed everything, we need to give the subscription back to
    // allow LvlhFrame to unsubscribe.
    if ( planet_centered_inertial) {
      planet_centered_inertial->subscribe();
    }
    if (lvlh_origin_frame) {
      lvlh_origin_frame->subscribe();
    }
  }
}

/*****************************************************************************
intiialize
Purpose:(initializes the model based on names)
NOTE - in SeparationState, the source-body identification was provided first.
       while herre the subject-body is provided first.  It was deemed
       significantly more likely that the lvlh-origin-frame would be defaulted
       to a composite-body frame, so we wanted to source-name to be the last
       optional argument.
*****************************************************************************/
void
LvlhSeparationState::initialize( jeod::DynManager & dyn_manager,
                                 std::string planet_name,
                                 jeod::DynBody   & subject_body,
                                 jeod::DynBody   & source_body,
                                 std::string subject_name,
                                 std::string source_name)

{
  if (!enabled) {
    return;
  }

  // Make some sanity checks to ensure that the inputs are legitimate

  // Check the subject frame:
  jeod::BodyRefFrame * subject_frame;
  if (subject_name.empty()) {
    subject_frame = &subject_body.composite_body;
  } else {
    subject_frame = const_cast<jeod::BodyRefFrame *>(
                         subject_body.find_vehicle_point(subject_name));
  }
  if (subject_frame == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid frame name\n",
      "The specified frame-name '", subject_name, "' on body-name '", subject_body.name, "' could not be found\n"
      "by the dynamics manager.\n"
      "Initialization of LvlhSeparationState failed because the\n"
      "SeparationState requires a defined subject frame before it can\n"
      "generate the state for it.\n");
    return;
  }

  // Check to see whether we need to be configuring the Lvlh Frame:
  if (using_external_lvlh) {
    CMLMessage::error(
      __FILE__,__LINE__, "Invalid initialize(...) call \n",
      "This initialize(...) call will configure the LVLH frame but\n"
      "the model is configured to use an externally-provided LVLH frame.\n"
      "Forwarding initialization call to the alternative initialize(...) "
      "method.\n");
    initialize( dyn_manager, *subject_frame);
    return;
  }

  // Check the source frame:
  if (source_name.empty()) {
    lvlh_origin_frame = &source_body.composite_body;
  } else {
    lvlh_origin_frame = const_cast<jeod::BodyRefFrame *>(
                           source_body.find_vehicle_point(source_name));
  }
  if (lvlh_origin_frame == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid frame name\n",
      "The specified frame-name '", source_name, "' on body-name '", source_body.name, "' could not be found\n"
      "by the dynamics manager.\n"
      "Initialization of LvlhSeparationState failed because the LvlhFrame\n"
      "requires this frame to construct the LVLH frame.\n");
    return;
  }

  // check the planet:
  jeod::BasePlanet * planet = dyn_manager.find_base_planet (planet_name);
  if (planet == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid planet name\n",
      "The specified planet-name / celestial-body-name ", planet_name, " could not be found\n"
      "by the dynamics manager.\n"
      "Initialization of LvlhSeparationState failed because the LvlhFrame\n"
      "requires a planet to define vertical and horizontal.\n");
    return;
  }

  // with these pointers verified, dereference them and continue to the
  // initialize method, as called with references instead of name-lookups.
  initialize( dyn_manager,
              *planet,
              *subject_frame,
              *lvlh_origin_frame);
}

/*****************************************************************************
initialize
Purpose:(initializes the model based on references)
*****************************************************************************/
void
LvlhSeparationState::initialize( jeod::DynManager     & dyn_manager,
                                 jeod::BasePlanet     & planet,
                                 jeod::BodyRefFrame   & subject_frame,
                                 jeod::BodyRefFrame   & lvlh_origin_frame_)
{
  if (!enabled) {
    return;
  }

  if (using_external_lvlh) {
    CMLMessage::error(
      __FILE__,__LINE__, "Invalid initialize(...) call \n",
      "This initialize(...) call will configure the LVLH frame but\n"
      "the model is configured to use an externally-provided LVLH frame.\n"
      "Forwarding initialization call to the alternative initialize(...)\n"
      "method.\n");
      initialize( dyn_manager, subject_frame);
      return;
  }

  // Some potential for confusion here with the use of source-frame and
  // subject-frame.
  // in JEOD parlance (used in setting up the LvlhFrame instance) the
  // subject-frame is the frame to be used for defining the origin of the
  // LVLH frame.
  // In this model the subject-frame is the frame whose state we are finding
  // *relative to* that LVLH frame.
  // So subject-frame, as passed in in the argument list is wholly independent
  // of the subject-frame in the LvlhFrame model; subject-frame in the
  // LvlhFrame model is the lvlh-origin-frame as passed in here.

  // Furthermore, the source-frame in the SeparationState model is not passed
  // in here at all.  It is created by this model's call to
  // LvlhFrame::initialize() method, and then passed on to the
  // SeparationState model.

  // Push the subject and planet informationstraight to the LvlhFrame model
  lvlh.set_subject_frame (lvlh_origin_frame_);
  lvlh.set_planet( planet);

  // Create the new frame and get it added to the tree
  lvlh.initialize( dyn_manager);

  // Note -- initialization adds subscriptions to the new lvlh frame and to the
  // planet's inertial frame.  These may not be necessary, so save accesssors
  // to the two frames for subscription management and unsubscribe from them
  // for now.  Subscriptions will be added back in at model activation.
  lvlh_origin_frame        = & lvlh_origin_frame_;
  planet_centered_inertial = & planet.inertial;
  // NOTE -- if inertial does not exist, code would have failed in
  //         LvlhFrame::initialize().
  lvlh_origin_frame->unsubscribe();
  planet_centered_inertial->unsubscribe();

  // initialzie the separation state, which includes the SubscriptionBase
  // initialization.
  SeparationState::initialize( lvlh.frame,
                               subject_frame);
}


/*****************************************************************************
initialize
Purpose: initializes the model using an external LVLS frame.
*****************************************************************************/
void
LvlhSeparationState::initialize( jeod::DynManager     & dyn_manager,
                                 jeod::BodyRefFrame   & subject_frame)
{
  if (!enabled) {
    return;
  }

  if (!using_external_lvlh) {
    CMLMessage::fail(
      __FILE__,__LINE__, "Invalid configuration at initialization\n",
      "The LvlhSeparationState::initialize(...) method called requires\n"
      "an externally generated LVLH frame.\n"
      "The internal LVLH frame has not been configured and so cannot be "
      "used.\n");
  }
  planet_centered_inertial =
       &(dyn_manager.find_base_planet (lvlh_ref.planet_name)->inertial);
  lvlh_origin_frame = dynamic_cast< jeod::BodyRefFrame* >(
                    dyn_manager.find_ref_frame (lvlh_ref.subject_name));

  if (planet_centered_inertial == nullptr ||
      lvlh_origin_frame == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__, "initialization error\n",
      "Could not find the frames associated with the external LVLH frame.\n"
      "It is possible that it has not been properly configured.\n"
      "Initialization of the LVLH Separation State cannot proceed without\n"
      "a fully defined LVLH-frame.\n"
      "Aborting initialization.\n");
    return;
  }
  // initialize the separation state, which includes the SubscriptionBase
  // initialization.
  SeparationState::initialize( lvlh_ref.frame,
                               subject_frame);
}

/*****************************************************************************
update
Purpose:(Main executable)
Notes:
  Because there could potentially be multiple users of this LVLH frame and
    the update is not free, it is worth checking the timestamps first to
    evaluate whether the lvlh-frame is already sync'd with the lvlh-origin-frame.
    - If the timestamp of the lvlh_origin_frame has moved on since it was
      assigned to the lvlh-frame (the last time the lvlh-frame was updated)
      we need to update the lvlh-frame to re-sync.
    - If the lvlh-frame has never been updated (its timestamp remains at 0.0),
      we need to update the lvlh-frame.
    - If the two frames have a common non-zero timestamp, they are already
      sync'd and we do not need to update the lvlh-frame.
*****************************************************************************/
void
LvlhSeparationState::update()
{
  if (!active) {
    return;
  }
  double timestamp = lvlh_ref.frame.timestamp();
  if ( MathUtils::has_changed_from( lvlh_origin_frame->timestamp(),
                                    timestamp)
       ||
       !MathUtils::has_changed_from( timestamp,
                                     0.0)) {
    lvlh_ref.update();
  }
  SeparationState::update();
}

/*****************************************************************************
activate
Purpose:(Executes at model activation, when the model is subscribed.)
*****************************************************************************/
void
LvlhSeparationState::activate()
{
  // NOTE -- in SeparationState, we use dyn_manager->subscribe_to_frame,
  //         which applies some sanity checks and eventually calls
  //         <frame>->subscribe.
  //         Here, we go straight to <frame>.subscribe().
  if (!frames_subscribed) {
    planet_centered_inertial->subscribe();
    lvlh_origin_frame->subscribe();
  }
  SeparationState::activate();
}

/*****************************************************************************
deactivate
Purpose:(Executes at model deactivation, when the model is completely
         unsubscribed.
         Unsubscribe those frames that this model subscribed to when it was
         activated, otherwise the Dyn-manager will continue to generate their
         states, possibly redundantly.)
*****************************************************************************/
void
LvlhSeparationState::deactivate()
{
  if (frames_subscribed) {
    planet_centered_inertial->unsubscribe();
    lvlh_origin_frame->unsubscribe();
  }
  SeparationState::deactivate();
}
