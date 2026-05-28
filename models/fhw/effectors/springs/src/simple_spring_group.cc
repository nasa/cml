/******************************** TRICK HEADER *********************************
PURPOSE:
  (Calculate the forces and torques resulting from a group of parallel springs.)

ASSUMPTIONS AND LIMITATIONS:
  (Springs are aligned)

PROGRAMMERS:
  (((Gary Turner)(OSR)(December 2014) (Antares) (new implementation))
   ((Brent Caughron) (OSR) (Jan 2021) (Antares) (Code review and IV&V)))
*******************************************************************************/

#include <cstring> // NULL
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/simple_spring_group.hh"

/*******************************************************************************
Constructor
*******************************************************************************/
SimpleSpringGroup::SimpleSpringGroup()
  :
  enabled(true),
  num_springs(0),
  spring_array(0),
  action_body_is_child(true),
  separation_complete(false),
  total_force_struc_reaction{0,0,0},
  total_force_struc_action{0,0,0},
  total_torque_struc_reaction{0,0,0},
  total_torque_struc_action{0,0,0},
  active(false),
  initialized(false),
  initialized_bodies(false),
  separation_speed(0.0),
  force_axial{0,0,0},
  force_struc_action{0,0,0},
  force_struc_reaction{0,0,0},
  T_spring_structure{{1,0,0},{0,1,0},{0,0,1}},
  T_action_reaction{{1,0,0},{0,1,0},{0,0,1}},
  moment_arm{0,0,0},
  spring_frame(NULL),
  separation_state(NULL),
  action_body(NULL),
  reaction_body(NULL)
{}

/*******************************************************************************
initialize
Purpose:(Sets the pointers.)
*******************************************************************************/
void
SimpleSpringGroup::initialize( const jeod::DynBody & action_body_in,
                               const jeod::DynBody & reaction_body_in,
                               SeparationState         & sep_state_in)
{
  set_bodies( action_body_in,
              reaction_body_in);
  set_sep_state( sep_state_in);
}

/*******************************************************************************
set_sep_state
Purpose:
  (Sets the spring-frame based on the specification of the separation-state.)
*******************************************************************************/
void
SimpleSpringGroup::set_sep_state( SeparationState  & sep_state_in)
{
  // If model has been disabled, just return
  if (!enabled) {
    return;
  }
  // Used in case that set_sep_state() is called with set_bodies()
  // post-initialization.  Safest method is to call initialize() at
  // initialization.
  if (!sep_state_in.is_initialized()) {
    CMLMessage::fail(
      __FILE__, __LINE__, "Spring Group activation: \n",
      "Error in activating the Spring Group.\n"
      "The accompanying SeparationState not been initialized.\n"
      "Cannot proceed without a Separation State.\n");
  }

  separation_state = &sep_state_in;

  // Simple case: all springs are aligned with the separation axis.
  // Aligned with separation axis => The transformation can be taken from
  // combining the sep-state source point orientation with the vehicle
  // orientation.
  // The source-point is associated with the action-body; its x-axis points
  // along the separation-axis towards the reaction-body, the same as the
  // spring reference-frame.
  // T_parent_this is the transformation from inertial to the
  // source-point's reference frame. The structural frame also has a
  // transformation from inertial to structural. We want:
  // T_{spring->struc} = T_{inertial->struc} T_{spring->inertial}
  // So we want the transpose of T_{inertial->spring} to
  // transform the force along the spring axis into structural.
  jeod::RefFrame * sep_source = separation_state->get_source();
  // NOTE: Hitting this NULL check may be impossible do to other checks in
  // place from the separation state model.
  if (sep_source == NULL) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid separation state:\n",
      "The provided separation state is not (yet) valid for use in the Springs"
      " model.\n"
      "The source-frame for the separation-state has not (yet) been defined.\n"
      "This could be an initialization sequencing issue (frame is identified\n"
      "by the conclusion of the initialization of the sep-state), or a \n"
      "configuration error in the separation state itself.\n"
      "Sep-state initialization flag is ", separation_state->is_initialized(), "");
  }

  spring_frame = dynamic_cast<jeod::BodyRefFrame*>(sep_source);
  if (spring_frame == NULL) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid reference frame:\n",
      "The separation state provided is not valid for use in the Springs model.\n"
      "All SeparationState instances provide the state of a BodyRefFrame\n"
      "instance with respect to a RefFrame instance.\n"
      "The Spring model requires that the RefFrame also be a BodyRefFrame\n"
      "and in this case it is not.\n"
      "Frame name is ", sep_source->get_name(), "\n");
  }
  // TODO Turner Jan 2017
  // Assumption here that the sep-state's source is a member of the
  // action-body. This is an assumption, and it should be checked but
  // that isn't being done. If the spring_frame's parent is the
  // action_body's structural frame, all is well.
  // If the source belongs to the reaction-body, then the spring-frame should
  // be associated with the target instead of the source.
  // Either way, the spring-frame should be associated with the action-body.

  jeod::Matrix3x3::product_right_transpose(
                   action_body->structure.state.rot.T_parent_this,
                   spring_frame->state.rot.T_parent_this,
                   T_spring_structure);
  if (initialized_bodies) {
    initialized = true;
  }
}

/*******************************************************************************
initialize
Purpose:(Sets the pointers.)
*******************************************************************************/
void
SimpleSpringGroup::set_bodies( const jeod::DynBody & action_body_in,
                               const jeod::DynBody & reaction_body_in)
{
  // If model has been disabled, just return
  if (!enabled) {
    return;
  }

  action_body = &action_body_in;
  reaction_body = &reaction_body_in;

  for( unsigned int ii = 0; ii < num_springs; ii++) {
    spring_array[ii].initialize();
  }

  initialized_bodies = true;
  // NOTE: With the current order of initialization done in the main
  // initialize() function above this check may never be hit. As it can only
  // be done if the set_bodies() function is called after the set_sep_state()
  // function. This scenario could happen if the set_bodies() and set_sep_state()
  // functions were called separately outside of the initialize() function. But
  // as normal use is to just use the initialize() function, this seems unlikely.
  if (separation_state != NULL) {
    initialized = true;
  }
}

/*******************************************************************************
activate
Purpose:(Activates the spring-model.)
*******************************************************************************/
void
SimpleSpringGroup::activate()
{
  if (!enabled) {
    CMLMessage::error(
      __FILE__, __LINE__, "Spring Group activation: \n",
      "Error in activating the Spring Group.\n"
      "The model has been disabled\n");
    return;
  }

  if (!initialized) {
    CMLMessage::error(
      __FILE__, __LINE__, "Spring Group activation: \n",
      "Error in activating the Spring Group.\n"
      "The model has NOT been initialized\n");
    return;
  }

  separation_state->subscribe(); // activates and updates the model
  // NOTE: This check may be impossible to hit due to the subscribe() call
  // just above. As having a faulty subscribe() after having a good initialize
  // and activation seems unlikely.
  if (!separation_state->is_active()) {
    CMLMessage::error(
      __FILE__, __LINE__, "Spring Group activation: \n",
      "Error in activating the Spring Group.\n"
      "The associated separation-state is not available.\n");
    return;
  }

  active = true;
  for( unsigned int ii = 0; ii < num_springs; ii++) {
    spring_array[ii].set_active(true);
  }
  
  if ((action_body->get_parent_body() == reaction_body) && (action_body_is_child)) {
    jeod::Matrix3x3::transpose( action_body->mass.structure_point.T_parent_this,
                                T_action_reaction);
  } else if ((reaction_body->get_parent_body() == action_body) && (!action_body_is_child)) {
    jeod::Matrix3x3::copy( reaction_body->mass.structure_point.T_parent_this,
                           T_action_reaction);
  } else {
    CMLMessage::error(
        __FILE__,__LINE__,"Spring Group activation error: \n",
        "Cannot determine T_action_reaction when bodies, ", action_body->name.get_name(),
        " and ", reaction_body->name.get_name(), ", are not attached.\n");
  }
}

/*******************************************************************************
deactivate
Purpose:(Called when the springs are fully extended.)
*******************************************************************************/
void
SimpleSpringGroup::deactivate()
{
  jeod::Vector3::initialize( total_force_struc_reaction);
  jeod::Vector3::initialize( total_force_struc_action);
  for( unsigned int ii = 0; ii < num_springs; ii++) {
    spring_array[ii].set_active(false);
  }
  // Unsubscribe to the sep-state so that it can be deactivated.
  separation_state->unsubscribe();
}

/*******************************************************************************
update
Purpose:()
Assumptions: (The springs move together; the separation distance between the
              two vehicles is identical for all springs.)
             (The structural reference frames of the two vehicles are identical)
*******************************************************************************/
void
SimpleSpringGroup::update()
{
  if (!active) {
    return;
  }

  if (action_body->mass.is_progeny_of(reaction_body->mass) ||
      reaction_body->mass.is_progeny_of(action_body->mass) ) {
    CMLMessage::warn (
      __FILE__,__LINE__,"Spring Group activation error: \n",
      "Model has been activated while the two bodies, ", action_body->name.get_name(),
      " and ", reaction_body->name.get_name(), " are attached.\n");
    return;
  }

  // Verify that at least one of the springs is still doing something.
  active = false;
  for( unsigned int ii = 0; ii < num_springs; ii++) {
    if (spring_array[ii].get_active()) {
      active = true;
      break;
    }
  }

  // If all of the springs are finished, the separation is complete.
  if (!active) {
    separation_complete = true;
    deactivate();
    return;
  }

  // The separation-state has (presumably) already run and provides the state
  // between the the two vehicles. SeparationState does not provide
  // separation-speed.
  separation_speed = jeod::Vector3::vmag( separation_state->rel_state.trans.velocity);

  // Zero-out the accumulating force-vector for accumulation of forces from all
  // springs.
  jeod::Vector3::initialize( total_force_struc_action);
  jeod::Vector3::initialize( total_force_struc_reaction);
  jeod::Vector3::initialize( total_torque_struc_action);
  jeod::Vector3::initialize( total_torque_struc_reaction);

  // For each spring:
  for (unsigned int ii = 0; ii < num_springs; ii++) {
    if (spring_array[ii].get_active()) {
      // Calculate the axial force. Because the springs are aligned with the
      // separation axis, the separation-state distance is also the length of
      // the springs.
      // Calculate_force returns a force magnitude. The direction of this
      // force (applied to the action-body) is along the negative x-axis.
      force_axial[0] = -spring_array[ii].calculate_force (
                                separation_state->separation_distance,
                                separation_speed);
      // force_axial[1] and [2] are unchanged at 0.0 from construction.

      jeod::Vector3::transform( T_spring_structure,
                          force_axial,
                          force_struc_action);

      // Add the torques on the action body:   tau = r_action x F
      jeod::Vector3::diff( spring_array[ii].position_action,
                     action_body->mass.composite_properties.position,
                     moment_arm);
      jeod::Vector3::cross_incr( moment_arm,
                           force_struc_action,
                           total_torque_struc_action);
      // Accumulate the forces on the action-body
      jeod::Vector3::incr( force_struc_action,
                     total_force_struc_action);
     
      // Negate for Newton III and transform to the reaction body struc frame
      // body.mass.structure_point.T_parent_this
      // NOTE:  Assumption that vehicles separate axially implies that
      //        the structural frames remain in fixed relative orientation.
      //        With that assumption, the transformation of the 
      //        action-reaction force-pair from the action-body's struc-frame
      //        to the reaction-body's struc frame is predetermined and
      //        constant.
      //        If vehicles start rotating relative to one another, then
      //        need to transform to a vehicle-independent frame, such as
      //        inertial.
      //        etc.
      jeod::Vector3::transform( T_action_reaction,
                          force_struc_action,
                          force_struc_reaction);
      jeod::Vector3::negate( force_struc_reaction);
      // Add the torques on the reaction body:   tau = r_reaction x F
      jeod::Vector3::diff( spring_array[ii].position_reaction,
                     reaction_body->mass.composite_properties.position,
                     moment_arm);
      jeod::Vector3::cross_incr( moment_arm,
                           force_struc_reaction,
                           total_torque_struc_reaction);
      // Accumulate the forces on the reaction-body
      jeod::Vector3::incr( force_struc_reaction,
                     total_force_struc_reaction);

    }
  }
}
