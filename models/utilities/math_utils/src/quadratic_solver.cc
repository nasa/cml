/*******************************TRICK HEADER******************************
PURPOSE: ( Setup and solution using the quadratic formula.)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Sep 2023) (ANTARES) (new)))
***********************************************************************/

#include "../include/quadratic_solver.hh"
#include <cmath> // std::abs, std::isinf
#include <cfloat> // DBL_MAX

/*****************************************************************************
Constructor
*****************************************************************************/
QuadraticSolver::QuadraticSolver()
  :
  a(0.0),
  b(0.0),
  c(0.0),
  root1(0.0),
  root2(0.0),
  roots_exist(false)
{}
/****************************************************************************/
QuadraticSolver::QuadraticSolver(
  double a_,
  double b_,
  double c_,
  bool compute_roots)
  :
  a(a_),
  b(b_),
  c(c_),
  root1(0.0),
  root2(0.0),
  roots_exist(false)
{
  if (compute_roots) {solve();}
}

/*****************************************************************************
Name: solve
Purpose:
 Solves for the roots of the quadratic function defined as
  a x^2 + b x + c
 i.e. the values of x that satisfy a x^2 + b x + c = 0
*****************************************************************************/
bool QuadraticSolver::solve( bool compute_roots)
{
  root1 = root2 = 0.0;
  roots_exist = false;

  // normalize if b^2 overflows
  if (std::isinf(b*b)) {
    a = a/b;
    c = c/b;
    b = 1;
  }

  // use Taylor series approximation when b^2 >> 4ac to
  // avoid cancellation of b in standard quadratic formula
  if ( compute_roots && b*b >= std::abs(a*c)*1E16 ) {
    // approximate the smaller root
    // (primarily affected by the cancellation)
    if (std::abs(c) / DBL_MAX < std::abs(b)) {
      root1 = -c/b;
      roots_exist = true;
    }
    // approximate the larger root
    if (std::abs(b) / DBL_MAX < std::abs(a)) {
      root2 = -b/a;
      // If the smaller root was not found, duplicate the larger root
      if (!roots_exist) {
        root1 = root2;
      }
      roots_exist = true;
    }
    // if only the smaller root was computed, duplicate it to root2
    else if (roots_exist) {
      root2 = root1;
    }
    // else neither root found, nothing to record.
  }

  // else, just use standard quadratic formula
  else {
    double delta = (b*b - 4*a*c);
    roots_exist = delta >= 0;
    if (compute_roots && roots_exist) {
      delta = std::sqrt(delta);
      root1 = (-b - delta)/ (2*a);
      root2 = (-b + delta)/ (2*a);
    }
  }
  return roots_exist;
}
