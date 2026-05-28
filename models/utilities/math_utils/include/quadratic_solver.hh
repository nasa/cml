/*******************************TRICK HEADER******************************
PURPOSE: ( Setup and solution using the quadratic formula.)

LIBRARY DEPENDENCY:
  (../src/quadratic_solver.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Sep 2023) (ANTARES) (new)))
***********************************************************************/
#ifndef CML_MATHUTILS_QUADRATIC_SOLVER_HH
#define CML_MATHUTILS_QUADRATIC_SOLVER_HH


class QuadraticSolver {
 public:
  double a; /* (--) Lead term, coefficient of x^2.*/
  double b; /* (--) Coefficient of x^1. */
  double c; /* (--) Coefficient of x^0. */
  double root1; /* (--) value of one of the 2 roots of f(x).*/
  double root2; /* (--) value of one of the 2 roots of f(x).*/
  bool   roots_exist; /* (--) Flag indicating that real roots of f(x) exist.*/

  QuadraticSolver();
  QuadraticSolver( double a,
                   double b,
                   double c,
                   bool compute_roots = true);

  bool solve( bool compute_roots = true);

};
#endif
