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
  double spring_constant;     /* (N/m) Spring or flange stiffness constant. */
  double damping_constant;    /* (N*s/m) Damping constant F = -bv. */
  double stroke;              /* (m) Allowable Spring compression displacement
                                     (>= 0). */
  double final_compression;   /* (m) Final displacement in compression. */
  double position_action[3];  /* (m) The structural position at which the spring
                                     is attached to the action-body. */
  double position_reaction[3];/* (m) The structural position at which the spring
                                     pushes against the reaction-body. */

  
  // Output values
  double compression;         /* (m) Distance between spring initial and current
                                     position. */
  double axial_force;         /* (N) Spring force. */
  double damping_force;       /* (N) Force caused by spring damping. */

protected:
  // Working values
  bool   active;              /* (--)  Flag that determines whether the spring
                                       is flexing. */
  double initial_compression; /* (m)   Initial spring compression (>= 0). */
  double extension_distance;  /* (m)   Current separation distance. */
  double extension_speed;     /* (m/s) Current separation speed. */

 // Methods:
public:
  SimpleSpringIndividual(); 
  void set_active( bool arg){active = arg;};
  bool get_active(){return active;};
  void initialize();
  double calculate_force( double distance,
                          double speed);
private:
  // operator= and copy constructor declared private and not implemented
  SimpleSpringIndividual(const SimpleSpringIndividual& rhs);
  SimpleSpringIndividual & operator= (const SimpleSpringIndividual&);
};

#endif
