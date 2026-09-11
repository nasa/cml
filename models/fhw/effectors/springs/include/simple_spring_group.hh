/******************************** TRICK HEADER *********************************
PURPOSE:
  (Class defining grouping of simple springs.)

ASSUMPTIONS AND LIMITATIONS:
  (Springs are aligned.)

LIBRARY DEPENDENCY:
  ((../src/simple_spring_group.cc)
   (../src/simple_spring_individual.cc))

PROGRAMMERS:
  (((Gary Turner)(OSR)(December 2014) (Antares) (new implementation))
   ((Brent Caughron) (OSR) (Jan 2021) (Antares) (Code review and IV&V)))
*******************************************************************************/

#ifndef CML_SIMPLE_SPRING_GROUP_HH
#define CML_SIMPLE_SPRING_GROUP_HH

#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "jeod/models/dynamics/dyn_body/include/body_ref_frame.hh"
#include "cml/models/dynamics/state_descriptors/separation_state/include/separation_state.hh"

#include "simple_spring_individual.hh"

namespace cml {

/*******************************************************************************
SpringGroup
Purpose:(Collection of Springs)
*******************************************************************************/
class SimpleSpringGroup {
public:
  // Input
  bool enabled{true};               /* (--)
       Master run-level spring model on/off flag. */
  unsigned int  num_springs{0};  /* (--) Total number of springs. */
  SimpleSpringIndividual * spring_array{nullptr}; /* (--)
       Dynamically allocated array of springs. */
  bool action_body_is_child{true}; /* (--)
       Specifies whether the child body in the attachment is the one that
       hosts the separation-state and is therefore the action body.
       The action force is applied along the -ve x-axis of the source frame
       of the separation state. */

  // Output
  bool separation_complete{false};         /* (--)
       Body separation complete on/off flag. */
  double total_force_struc_reaction[3]{};  /* (N)
       Total spring force on the reaction body in the structural frame. */
  double total_force_struc_action[3]{};    /* (N)
       Total spring force on the action body in the structural frame. */
  double total_torque_struc_reaction[3]{}; /* (N*m)
       Total spring tourque on the reaction body in the structural frame.*/
  double total_torque_struc_action[3]{};   /* (N*m)
       Total spring torque on the action body in the structural frame. */

protected:
  bool active{false};             /* (--) Triggered flag. */
  bool initialized{false};        /* (--) Spring group initialization complete. */
  bool initialized_bodies{false}; /* (--)
       All individual springs initialization complete. */
  double separation_speed{0.0};   /* (m/s)
       Body separation relative velocity magnitude . */
  double force_axial[3]{};        /* (N)  Spring axial force. */
  double force_struc_action[3]{}; /* (N)
       Spring force in the action body structural frame. */
  double force_struc_reaction[3]{}; /* (N)
       Spring force in the reaction body structural frame. */
  double T_spring_structure[3][3]{{1,0,0},{0,1,0},{0,0,1}}; /* (--)
       Spring axis to structural transformation matrix . */
  double T_action_reaction[3][3]{{1,0,0},{0,1,0},{0,0,1}}; /* (--)
       Action structural to reaction structural transformation matrix . */
  double moment_arm[3]{}; /* (m)
       Temporary storage for computation of moment-arm. */

  jeod::BodyRefFrame * spring_frame{nullptr}; /* (--)
       The frame in which the springs are defined.*/
  SeparationState * separation_state{nullptr}; /* (--)
       Determines whether action/reaction bodies have separated. */

  const jeod::DynBody  * action_body{nullptr};  /* (--)
       The body attached to the springs. */
  const jeod::DynBody  * reaction_body{nullptr};/* (--)
       The body the springs are pushing against. */

  // Methods:
public:
  SimpleSpringGroup() = default;
  SimpleSpringGroup(const SimpleSpringGroup& rhs) = delete;
  SimpleSpringGroup & operator= (const SimpleSpringGroup&) = delete;
  void initialize( const jeod::DynBody  & action_body_in,
                   const jeod::DynBody  & reaction_body_in,
                   SeparationState  & sep_state_in);
  void set_sep_state( SeparationState  & sep_state_in);
  void set_bodies( const jeod::DynBody  & action_body_in,
                   const jeod::DynBody  & reaction_body_in);

  void activate();
  void deactivate();
  void update();
};



} // namespace cml

#endif