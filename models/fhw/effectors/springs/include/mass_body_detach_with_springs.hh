/******************************** TRICK HEADER *********************************
PURPOSE:
  (Data structures for spring definitions.)

ASSUMPTIONS AND LIMITATIONS:
  (Uses simple-springs model, inherits assumptions.)

LIBRARY DEPENDENCY:
  ((../src/simple_spring_individual.cc))

PROGRAMMERS:
  (((Gary Turner)(OSR)(December 2014) (Antares) (new implementation))
   ((Brent Caughron) (OSR) (Jan 2021) (Antares) (Code review and IV&V)))
*******************************************************************************/

#ifndef CML_MASS_BODY_DETACH_WITH_SPRINGS_HH
#define CML_MASS_BODY_DETACH_WITH_SPRINGS_HH

#include "jeod/models/dynamics/body_action/include/body_detach.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "simple_spring_group.hh"

/* spring specific info for each spring */
class MassBodyDetachWithSprings : public jeod::BodyDetach
{
public:
  SimpleSpringGroup springs; // (--) collection of springs.
  bool child_is_action_body; /* (--)
       Specifies whether the child body in the attachment is the one that
       hosts the separation-state and is therefore the action body.
       The action force is applied along the -ve x-axis of the source frame
       of the separation state. */

  MassBodyDetachWithSprings() : BodyDetach(),
                                springs(),
                                child_is_action_body(true){};

  // NOTE - initialize(jeod::DynManager) is called from Dynamics Manager as a
  // consequence of adding this body action with the add_body_action method
  // call.
  void initialize( const jeod::DynBody & parent,
                   SeparationState & sep_state)
  {
    const auto* subject = get_subject_dyn_body();
    if (subject != nullptr) {
      if (child_is_action_body) {
        springs.initialize( *subject,
                            parent,
                            sep_state);
      } else {
        springs.initialize( parent,
                            *subject,
                            sep_state);
      }
    } else {
      // The subject is either a MassBody with no DynBody parent or was never
      // defined.
      CMLMessage::warn(
        __FILE__,__LINE__,"jeod::DynBody is NULL:\n",
        "The subject body on which the springs will separate from is not a "
        "jeod::DynBody as it should be, so it has no state.\n"
        "The springs model requires a jeod::DynBody on which to act and "
        "separate from.\n There is no reason to have the springs model act on "
        "a non-dynamic body due to there being no state on which the springs "
        "dynamics can modify.\n Due to this the springs were not initialized "
        "correctly and their effect will not be applied when the bodies "
        "detach.\n");
    }
  };

  void initialize( const jeod::DynBody & parent)
  {
    const auto* subject = get_subject_dyn_body();
    if (subject != nullptr) {
      if (child_is_action_body) {
        springs.set_bodies( *subject,
                             parent);
      } else {
        springs.set_bodies(  parent,
                            *subject);
      }
    } else {
      // The subject is either a MassBody with no DynBody parent or was never
      // defined.
      CMLMessage::warn(
        __FILE__,__LINE__,"jeod::DynBody is NULL:\n",
        "The subject body on which the springs will separate from is not a "
        "jeod::DynBody as it should be, so it has no state.\n"
        "The springs model requires a jeod::DynBody on which to act and "
        "separate from.\n There is no reason to have the springs model act on "
        "a non-dynamic body due to there being no state on which the springs "
        "dynamics can modify.\n Due to this the springs were not initialized "
        "correctly and their effect will not be applied when the bodies "
        "detach.\n");
    }
  };

  virtual void apply( jeod::DynManager & dyn_manager)
  {  
    springs.action_body_is_child = child_is_action_body;
    springs.activate();
    jeod::BodyDetach::apply(dyn_manager);
  };

private:
  // operator= and copy constructor declared private and not implemented
  MassBodyDetachWithSprings(const MassBodyDetachWithSprings& rhs);
  MassBodyDetachWithSprings & operator= (const MassBodyDetachWithSprings&);
};
#endif
