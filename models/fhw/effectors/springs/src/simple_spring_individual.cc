/******************************** TRICK HEADER *********************************
PURPOSE:
  (Calculate the forces resulting from a single spring.)

ASSUMPTIONS AND LIMITATIONS:
  (Springs are aligned.)

PROGRAMMERS:
  (((Gary Turner)(OSR)(December 2014) (Antares) (new implementation))
   ((Brent Caughron) (OSR) (Jan 2021) (Antares) (Code review and IV&V)))
*******************************************************************************/

#include "../include/simple_spring_individual.hh"

/*******************************************************************************
Constructor
*******************************************************************************/
SimpleSpringIndividual::SimpleSpringIndividual()
  :
  spring_constant(0.0),
  damping_constant(0.0),
  stroke(0.0),
  final_compression(0.0),
  position_action(),
  position_reaction(),
  compression(0.0),
  axial_force(0.0),
  damping_force(0.0),
  active(false),
  initial_compression(0.0),
  extension_distance(0.0),
  extension_speed(0.0)
{ }

/*******************************************************************************
initialize
Purpose:(Used to set the initial compression of the springs.)
*******************************************************************************/
void
SimpleSpringIndividual::initialize()
{
   /* Compute initial spring compression */
  initial_compression = stroke + final_compression;
}

/*******************************************************************************
calculate_force
Purpose:(Calculates the linear force exerted by a single spring)
*******************************************************************************/
double
SimpleSpringIndividual::calculate_force( double separation_distance,
                                         double separation_speed)
{
  // If the spring is not aligned with the principle axis, then we have to
  // compute its extension and extension speed that provide the
  // separation_distance and separation_speed.
  // First, find the component of the separation_distance that lies along the
  // spring x-axis; call this d.
  // Then the length of the spring that provides the separation-distance (D)
  // is L = D * (D / d).
  // The same scaling applies to the speed: S = V * (D / d)
  // The spring x-axis is the x-axis of the reference frame associated with
  // the action_spring_point.
  // That ratio is identically 1 in the simple model because all springs
  // are assumed // alligned with the principle separation axis.
  // Consequently:
  extension_distance = separation_distance;
  extension_speed    = separation_speed;

  // Would compute an extension distance here, but in this simple case, with
  // everything aligned, extension_distance = separation_distance so set it
  // as it is passed in instead.
  compression = initial_compression - extension_distance;
  if (extension_distance > stroke) {
    axial_force = 0;
    // deactivate this spring; the group will deactivate when all spring
    // components have self-deactivated.
    active = false;
    return axial_force;
  }

  axial_force   = spring_constant * compression;
  damping_force = damping_constant * extension_speed;
  axial_force  -= damping_force;
  return axial_force;
}
