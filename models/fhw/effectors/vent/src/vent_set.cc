/*******************************TRICK HEADER******************************
PURPOSE: (Class for manipulating sets of vents and aggregating forces and
          moments)

LIBRARY DEPENDENCY:
  (../src/vent_set.cc)

PROGRAMMERS:
  (((Daniel Ghan) (OSR) (Mar 2020) (Antares) (Initial version)))
 ************************************************************************/

#include "jeod/models/utils/quaternion/include/quat.hh" // jeod::Quaternion

#include "../include/vent_set.hh"

/*****************************************************************************
Constructors
(a) baseline, do not create any vents
*****************************************************************************/
VentSet::VentSet(jeod::DynBody & dyn_body_,
                 const double & time)
  :
  dyn_body(dyn_body_),
  dyn_time(time),
  start_vents_at_activation(false),
  force{0.0, 0.0, 0.0},
  torque{0.0, 0.0, 0.0},
  vents(),
  dynamic_vents(),
  impulsive_vents(),
  ang_impulse{0.0, 0.0, 0.0}
{
  subscribe_name = "VentSet:";
}
/*****************************************************************************
(b) No mass considerations
*****************************************************************************/
VentSet::VentSet(
    size_t         num_vents,
    jeod::DynBody& dyn_body_,
    const double & time)
  :
  VentSet(dyn_body_, time)
{
  for (size_t ii = 0; ii < num_vents; ++ii) {
    SimpleVent * new_vent = new SimpleVent(time);
    new_vent->allocated_in_set = true;
    vents.push_back( new_vent);
  }
}
/************************************************************************
(c) All vents off a single tank
************************************************************************/
VentSet::VentSet(
    size_t            num_vents,
    jeod::DynBody   & dyn_body_,
    const double    & time,
    DynamicMassBody & tank)
  :
  VentSet(dyn_body_, time)
{
  for (size_t ii = 0; ii < num_vents; ++ii) {
    Vent * new_vent = new Vent( time, tank);
    new_vent->allocated_in_set = true;
    vents.push_back( new_vent);
  }
}
/************************************************************************
(d) All vents off their own individual tank
    Note -- this is dangerous because it assumes the array is the same size
            as num_vents.
************************************************************************/
VentSet::VentSet(
        size_t  num_vents,
        jeod::DynBody& dyn_body_,
        const double & time,
        DynamicMassBody * tank_array)
  :
  VentSet(dyn_body_, time)
{
  if (tank_array == NULL) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Construction error.\n",
    "Tank array has not been specified.\n");
  }
  for (size_t ii = 0; ii < num_vents; ++ii) {
    Vent * new_vent = new Vent( time, tank_array[ii]);
    new_vent->allocated_in_set = true;
    vents.push_back( new_vent);
  }
}
/************************************************************************
(e) TBS all vents off a single mass-string
************************************************************************/
/************************************************************************
(f) TBS all vents off individual DynMassBody, using varargs or variadic
        templates to specify each tank independently
************************************************************************/

/*****************************************************************************
Destructor
Purpose:(Deletes the vents allocated by the set)
*****************************************************************************/
VentSet::~VentSet()
{
  for (auto& vent:vents) {
    if (vent->allocated_in_set) {
      delete vent;
      vent = NULL;
    }
  }
}

/*****************************************************************************
initialize
Purpose:(Initializes all the vents)
*****************************************************************************/
void VentSet::initialize()
{
  initialized = true;
  for (auto& vent:vents) {
    vent->initialize();
    if (!vent->initialized) {
      // This will result in a fail(); but not immediately, to allow all the
      // vents to attempt initialization and print their error messages.
      initialized = false;
    }
  }

  if (initialized) {
    SubscriptionBase::initialize();
  }
  else {
    CMLMessage::fail(
      __FILE__,__LINE__,"Initialization failure\n",
      "One or more vents in the vent-set have failed initialization.\n"
      "The vent-set cannot be initialized.\n");
  }
}

/*****************************************************************************
update
Purpose:(Updates the model)
*****************************************************************************/
void VentSet::update()
{
  if (!active) {
    return;
  }

  if (!impulsive_vents.empty()) {
    apply_impulse();
  }

  if (!dynamic_vents.empty()) {
    collect_force_torque();
  }
}

/*****************************************************************************
start_vent(s)
Purpose:(Starts the specified vent, or all the vents, per their settings)
*****************************************************************************/
void VentSet::start_vent(unsigned int ix)
{
  if (!active) {
    CMLMessage::error(
      __FILE__,__LINE__,"Illegal instruction\n",
      "Instruction to start vent at index ", ix, " while the vent-set is inactive.\n"
      "Venting instruction not applied.\n");
    return;
  }
  if (ix >= vents.size()) {
    CMLMessage::error(
      __FILE__,__LINE__,"Illegal instruction\n",
      "Instruction to start vent at index ", ix, " but there are only ", vents.size(), " vents "
      "registered.\nVenting not applied.\n");
    return;
  }
  start_vent_internal( vents[ix]);
}
/****************************************************************************/
void VentSet::start_vents()
{
  if (!active) {
    CMLMessage::error(
      __FILE__,__LINE__,"Illegal instruction\n",
      "Instruction to start all vents while the vent-set is inactive.\n"
      "Venting instruction not applied.\n");
    return;
  }
  for (auto& vent:vents) {
    start_vent_internal(vent);
  }
}


/*****************************************************************************
stop_vent(s)
Purpose:(Turns off the vents)
*****************************************************************************/
void VentSet::stop_vent(unsigned int ix)
{
  if (ix >= vents.size()) {
    CMLMessage::error(
      __FILE__,__LINE__,"Illegal instruction\n",
      "Instruction to stop vent at index ", ix, " but there are only ", vents.size(), " vents "
      "registered.\nInstruction ignored.\n");
    return;
  }
  vents[ix]->stop_venting();
}
/****************************************************************************/
void VentSet::stop_vents()
{
  for (auto& vent:vents) {
    vent->stop_venting();
  }
}

/*****************************************************************************
use_impulse_mode
Purpose:(Simple interface to push "impulse-mode" switch up to all vents.)
*****************************************************************************/
void VentSet::use_impulse_mode(bool mode)
{
  for (auto& vent:vents) {
    vent->use_impulse_mode(mode);
  }
}

/*****************************************************************************
get_vent
Purpose:(Returns a pointer to the vent at specified index)
*****************************************************************************/
SimpleVent * VentSet::get_vent(size_t ii)
{
  size_t vents_size = vents.size();
  if (ii < vents_size) {
    return vents[ii];
  }
  else {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid request\n",
      "Index ", ii, " does not exist in the vents vector.\n"
      "The vents vector has only ", vents_size, " elements\n");
    // unreachable statement due to above FATAL error but required
    // because the compiler is expecting this method to return a pointer.
    return NULL;
  }
}


/*****************************************************************************
collect_force_torque
Purpose:(Collects the force and torque from all active vents)
*****************************************************************************/
void VentSet::collect_force_torque()
{
  jeod::Vector3::initialize(force);  // in dyn_body struc
  jeod::Vector3::initialize(torque); // in dyn_body struc

  double moment_arm[3];
  for (auto& vent:dynamic_vents) {
    vent->update();
  }

  // Remove any vents that are not still venting.  The predicate for remove_if
  // is locally defined (specified by []) to be the result of "not venting".
  dynamic_vents.remove_if( [](SimpleVent * v){return !v->venting;});

  for (auto& vent:dynamic_vents) {
    jeod::Vector3::incr(vent->force, force);
    jeod::Vector3::diff( vent->location,
                         dyn_body.mass.composite_properties.position,
                         moment_arm);
    jeod::Vector3::cross_incr( moment_arm,
                               vent->force,
                               torque);
  }
}

/*****************************************************************************
apply_impulse
Purpose:(Collects the effects of impulsive vents and applies the combined
         effect to the state of the root body.)
*****************************************************************************/
void VentSet::apply_impulse()
{
  double ang_impulse_struc[3] = {0.0, 0.0, 0.0};
  // Determine whether the dyn-body is the root of its tree.  We have to
  // override the state of the root body, so if the dyn-body is NOT the root,
  // we have some extra work to do:
  if( dyn_body.is_root_body() ) {
    double moment_arm[3];
    for (auto& vent:impulsive_vents) {
      vent->check_status();
      jeod::Vector3::diff( vent->location,
                           dyn_body.mass.composite_properties.position,
                           moment_arm);
      // combine moment arm and linear impulse to get angular impulse
      // moment arm from CM to mass point in dyn_body structure.
      jeod::Vector3::cross_incr( moment_arm,
                                 vent->impulse,
                                 ang_impulse_struc); // in dyn_body struc
    }
    dyn_body.mass.composite_properties.Q_parent_this.left_quat_transform(
        ang_impulse_struc, ang_impulse);  // in dyn-body body-frame
    apply_impulse_to_body( dyn_body);
  }
  else {
    // Identify the root body; cast away the const-ness because we have to
    // apply the impulse to the root body.
    jeod::DynBody * root_body = const_cast<jeod::DynBody *>(dyn_body.get_root_body());
    // Sanity check -- root body pointer must be non-NULL to continue.
    if (root_body == NULL) {
      // Possibly an unreachable error? 'dyn_body' is protected, so there is no
      // way to set root_body to NULL post initialization via external ways.
      // JEOD is guaranteed to never return a NULL for a root_body because a
      // jeod::DynBody, at a minimum, points to itself as the root_body, unless a
      // parent body chain is built.
      CMLMessage::error (
        __FILE__, __LINE__, "Cannot find root body\n",
        "Could not find root body for body ", dyn_body.name.get_name(), ".\n"
        "Impulse must be applied to root body.\n"
        "No impulse applied to this body.");
      return;
    }
    // Compute the moment arm from the root-body's CoM to the vent location.
    // This is done in 2 steps
    // Step-1 compute the vector from dyn-body struc to root-CoM in dyn-struc
    //        Note -  this is common for all vents.
    double dyn_struct_to_root_cm[3];
    root_body->composite_body.compute_position_from( dyn_body.structure,
                                                     dyn_struct_to_root_cm);
    double moment_arm[3];
    for (auto& vent:impulsive_vents) {
      vent->check_status();
      // Step2 - For each vent, generate the unique root-CoM-to-vent vector,
      //         expressed in dyn-struc.  This is the moment-arm over which
      //         the vent's impulse generates a rotational / angular impulse.
      jeod::Vector3::diff( vent->location,
                           dyn_struct_to_root_cm,
                           moment_arm);

      // Now collect the angular-impulses
      jeod::Vector3::cross_incr( moment_arm,
                                 vent->impulse,
                                 ang_impulse_struc);
    }

    // At this point, the angular-impulse is still expressed in the dyn-body
    // structural frame.  We need to express it in the root-body body frame.
    // There is no direct transformation available.
    // First, express in a common frame (inertial)
    jeod::Quaternion Q_this_parent;
    double ang_impulse_inertial[3];
    dyn_body.structure.state.rot.Q_parent_this.conjugate(Q_this_parent);
    Q_this_parent.left_quat_transform(ang_impulse_struc, ang_impulse_inertial);

    // so that it can be transformed express in root-body Body frame
    root_body->composite_body.state.rot.Q_parent_this.left_quat_transform(
        ang_impulse_inertial, ang_impulse);

    apply_impulse_to_body( *root_body);
  }
  // All impulsive vents have been processed.  Clear the list.
  impulsive_vents.clear();
}

/*****************************************************************************
apply_impulse_to_body
Purpose:(Common application method for applying impulse to the root-body of
         the mass-tree; does not matter whether or not dyn-body is root.
*****************************************************************************/
void VentSet::apply_impulse_to_body(jeod::DynBody & root_body)
{
  double lin_impulse[3] = {0.0, 0.0, 0.0}; // linear impulse (force * time)
  // accumulate the linear-impulses
  for (auto& vent:impulsive_vents) {
    jeod::Vector3::incr( vent->impulse,
                         lin_impulse); // in dyn-body struc frame
  }

  jeod::Quaternion Q_this_parent;
  dyn_body.structure.state.rot.Q_parent_this.conjugate(Q_this_parent);
  double lin_impulse_inertial[3];
  Q_this_parent.left_quat_transform(lin_impulse, lin_impulse_inertial);
                                                   // now in inertial frame

  double inverse_mass =
              MathUtils::divide_protected(1,
                                          root_body.mass.composite_properties.mass,
                                          0.0,
                                          false);
  double delta_v[3];
  jeod::Vector3::scale( lin_impulse_inertial,
                        inverse_mass,
                        delta_v);  // delta velocity, inrtl
  jeod::Vector3::incr( delta_v,
                       root_body.composite_body.state.trans.velocity);


  double inverse_inertia[3][3];
  if ( !jeod::Matrix3x3::invert_symmetric( root_body.mass.composite_properties.inertia,
                                     inverse_inertia)) {
    double delta_w[3];
    jeod::Vector3::transform( inverse_inertia,
                              ang_impulse,
                              delta_w); // delta rate, body frame
    jeod::Vector3::incr( delta_w,
                         root_body.composite_body.state.rot.ang_vel_this);
  }
  else {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid inertia\n",
      "Cannot generate a delta-omega value from the angular impulse because\n"
      "the inversion of the inertia matrix failed.\n");
  }

  // Update the rest of the root body's reference frames
  root_body.propagate_state();
}

/*****************************************************************************
activate
Purpose:(Activates the model)
*****************************************************************************/
void VentSet::activate()
{
  active = true;
  if (start_vents_at_activation) {
    start_vents();
  }
}

/*****************************************************************************
deactivate
Purpose:(Deactivates the model)
*****************************************************************************/
void VentSet::deactivate()
{
  // Make sure that the mass depletion from the last timestep has been recorded.
  for (auto& vent:dynamic_vents) {
    vent->update_mass_demand();
  }
  jeod::Vector3::initialize(torque);
  jeod::Vector3::initialize(force);
  active = false;
}

/*****************************************************************************
start_vent_internal
Purpose:(Internal method common to start_vent() and start_vents())
*****************************************************************************/
void VentSet::start_vent_internal(SimpleVent * vent)
{
  if (!vent->start_venting()) {
    return;
  }
  if (vent->apply_as_impulse) {
    if (!std::count(impulsive_vents.begin(), impulsive_vents.end(), vent)) {
      impulsive_vents.push_back(vent);
    }
  }
  else if (!std::count(dynamic_vents.begin(), dynamic_vents.end(), vent)) {
    dynamic_vents.push_back(vent);
  }
}
