/*******************************TRICK HEADER******************************
PURPOSE: ( Setup and solution using the quadratic formula.)

LIBRARY DEPENDENCY:
  (../src/quadratic_solver.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Sep 2023) (ANTARES) (new)))
***********************************************************************/
#ifndef CML_QUADRATIC_SOLVER_HH
#define CML_QUADRATIC_SOLVER_HH


class QuadraticSolver {
 public:
  double a{0.0}; /* (--) Lead term, coefficient of x^2.*/
  double b{0.0}; /* (--) Coefficient of x^1. */
  double c{0.0}; /* (--) Coefficient of x^0. */
  double root1{0.0}; /* (--) value of one of the 2 roots of f(x).*/
  double root2{0.0}; /* (--) value of one of the 2 roots of f(x).*/
  bool   roots_exist{false}; /* (--) Flag indicating that real roots of f(x) exist.*/

  QuadraticSolver() = default;
  QuadraticSolver( double a_,
                   double b_,
                   double c_,
                   bool compute_roots = true);

  bool solve( bool compute_roots = true);

};
#endif