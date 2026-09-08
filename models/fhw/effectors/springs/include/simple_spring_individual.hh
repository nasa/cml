/******************************** TRICK HEADER *********************************
PURPOSE:
  (Data structures for spring definitions.)

LIBRARY DEPENDENCY:
  ((../src/simple_spring_individual.cc))

PROGRAMMERS:
  (((Gary Turner)(OSR)(December 2014) (Antares) (new implementation))
   ((Brent Caughron) (OSR) (Jan 2021) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_SIMPLE_SPRING_INDIVIDUAL_HH
#define CML_SIMPLE_SPRING_INDIVIDUAL_HH

/* Spring specific info for each spring */
class SimpleSpringIndividual{
public:
  // Input values
  double spring_constant{0.0};   /* (N/m) Spring or flange stiffness constant. */
  double damping_constant{0.0};  /* (N*s/m) Damping constant F = -bv. */
  double stroke{0.0};            /* (m) Allowable Spring compression displacement
                                        (>= 0). */
  double final_compression{0.0}; /* (m) Final displacement in compression. */
  double position_action[3]{};   /* (m) The structural position at which the spring
                                        is attached to the action-body. */
  double position_reaction[3]{}; /* (m) The structural position at which the spring
                                        pushes against the reaction-body. */

  
  // Output values
  double compression{0.0};       /* (m) Distance between spring initial and current
                                        position. */
  double axial_force{0.0};       /* (N) Spring force. */
  double damping_force{0.0};     /* (N) Force caused by spring damping. */

protected:
  // Working values
  bool   active{false};            /* (--)  Flag that determines whether the spring
                                            is flexing. */
  double initial_compression{0.0}; /* (m)   Initial spring compression (>= 0). */
  double extension_distance{0.0};  /* (m)   Current separation distance. */
  double extension_speed{0.0};     /* (m/s) Current separation speed. */

 // Methods:
public:
  SimpleSpringIndividual() = default;
  SimpleSpringIndividual(const SimpleSpringIndividual& rhs) = delete;
  SimpleSpringIndividual & operator= (const SimpleSpringIndividual&) = delete;
  void set_active( bool arg){active = arg;}
  bool get_active() const {return active;}
  void initialize();
  double calculate_force( double separation_distance,
                          double separation_speed);
};

#endif