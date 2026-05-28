/*******************************TRICK HEADER******************************
PURPOSE: (Compute the points of intersection of a vector with an ellipsoid

LIBRARY DEPENDENCY:
   (())

PROGRAMMERS:
  (((Gary Turner) (OSR) (Sep 2023) (ANTARES) (new)))

NOTES:
- Ellipsoid defined by (x/a)^2 + (y/b)^2 + (z/c)^2 = 1
- {a,b,c} stored as r[3]
- algorithm computes the points {x,y,z} along the vector (end1 -> end2)
  such that {x,y,z} values satisfy the requirement defining the surface
  of the ellipsoid.
  - this simplifies to finding s in [0,1] such that
       end1 + s * (end2 - end1) satisfies the constraint, i.e.:
       (1-s)end1 + (s)end2      satisfies the constraint
***********************************************************************/
#ifndef CML_MATHUTILS_ELLIPSOID_INTERSECTION_HH
#define CML_MATHUTILS_ELLIPSOID_INTERSECTION_HH

#include <array>
#include "math_utils.hh"
#include "quadratic_solver.hh"
#include "std_array_ops.hh"

class EllipsoidIntersection{
 private:
  const double (&end1)[3]; /* (m)
    Reference to the position of one end of the line being tested.*/
  const double (&end2)[3]; /* (m)
    Reference to the position at the other end of the line being tested.*/
  std::array <double,3> inv_r; /* (m)
    reciprocal of the semi-axes of the ellipsoid along the three axes.
    Set at construction.*/
  QuadraticSolver quadratic; /* (--)
    Tool for solving quadratic equation. */

 public:
  bool intersection;
  std::array<double, 3> root1;
  std::array<double, 3> root2;

  EllipsoidIntersection( const double (&end1_)[3],
                         const double (&end2_)[3],
                         double r1,
                         double r2,
                         double r3)
    :
    end1(end1_),
    end2(end2_),
    // If division fails, set result to 0.0 (default) without terminating
    inv_r{ MathUtils::divide_protected( 1, r1),
           MathUtils::divide_protected( 1, r2),
           MathUtils::divide_protected( 1, r3)}
  {}
  virtual ~EllipsoidIntersection(){};

/*****************************************************************************
update
Purpose: Finds the intersection points
Algorithm:
  Let P = end1, Q = end2
  Then x = P_x + s (Q_x - P_x)
   so  x^2 = s^2 ( ((Q-P)_x)^2 ) +
             s   (2 (P_x) (Q-P)_x ) +
             1   ((P_x)^2)
  Similarly for y^2 and z^2.
  Scale these by the respective r[i] values such that
           p_x = P_x / r[0]
           q_x = Q_x / r[0]
           p_y = P_y / r[1]
           ... etc. and we have:
   (x^2 / a^2) = = s^2 ( ((q-p)_x)^2 ) +
                   s   (2 (p_x) (q-p)_x ) +
                   1   ((p_x)^2)
   and the ellispoid surface constraint of
               (x/a)^2 + (y/b)^2 + (z/c)^2 = 1
   becomes:
     s^2 ( ((q-p)_x)^2 + ((q-p)_y)^2 ((q-p)_z)^2 ) +
     s   (2 ( (p_x) (q-p)_x + (p_y) (q-p)_y + (p_z) (q-p)_z )) +
     1   ((p_x)^2 + (p_y)^2 + (p_z)^2) - 1.0
     = 0.0
   Now solve for s as a quadratic equation.
*****************************************************************************/
  bool update( bool compute_roots = true) {
    std::array<double,3>  Q_P = MathUtils::diff( end2, end1);
    std::array<double,3>  q_p = inv_r * Q_P;
    std::array<double,3>  p = inv_r * end1;


    quadratic.a = MathUtils::vector_scalar_product( q_p, q_p);
    quadratic.b = MathUtils::vector_scalar_product( p, q_p) * 2;
    quadratic.c = MathUtils::vector_scalar_product( p, p) - 1.0;

    intersection = quadratic.solve( compute_roots);
    if (intersection && compute_roots) {
      root1 = Q_P * quadratic.root1 + end1;
      root2 = Q_P * quadratic.root2 + end1;
    }
    return intersection;
  }
  double get_scaled_root1() const {return quadratic.root1;}
  double get_scaled_root2() const {return quadratic.root2;}

 private: // delete / not implemented.
  EllipsoidIntersection( const EllipsoidIntersection&);
  EllipsoidIntersection& operator= (const EllipsoidIntersection&);
};
#endif
