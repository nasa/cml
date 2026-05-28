/********************************** TRICK HEADER **************************
PURPOSE:
    (Little algorithm to test whether some coordinate pair lies within some
    polygon defined on the unit sphere by a set of (lambda,phi) polar
    coordinates with lambda representing a "longitudinal-like" angle and
    phi a "latitude-like" angle.
    This is a similar algorithm to that represented in the PolygonEnclosure
    class, which does the same for a planar polygon.

LIMITATION:
  (The polygon used to define the allowable space must be convex. At no
  point can a vertex produce a concave perimeter.)

PROGRAMMERS:
    (
     ((Gary Turner) (OSR) (May 2023) (New))
    )
**************************************************************************/
#ifndef CML_POLYGON_ENCLOSURE_SPHERE_HH
#define CML_POLYGON_ENCLOSURE_SPHERE_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include <cstring> // memcpy
#include <algorithm> // min,max
#include <array>

template<size_t N>
class PolygonEnclosureSphere
{
 protected:
//  double min_lambda; /* (--)
//    Minimum lambda-value of the coordinates of the polygon vertices.
//    Used to draw a simple rectangular box for a first-pass when qerying "is
//    this point inside the polygon"?*/
//  double max_lambda; /* (--)
//    Maximum lambda-value of the coordinates of the polygon vertices.
//    Used to draw a simple rectangular box for a first-pass when qerying "is
//    this point inside the polygon"?*/
//  double min_phi; /* (--)
//    Minimum phi-value of the coordinates of the polygon vertices.
//    Used to draw a simple rectangular box for a first-pass when qerying "is
//    this point inside the polygon"?*/
//  double max_phi; /* (--)
//    Maximum phi-value of the coordinates of the polygon vertices.
//    Used to draw a simple rectangular box for a first-pass when qerying "is
//    this point inside the polygon"?*/
  double box_min[3]; /* (--)
    Minimum [x,y,z]-values of the simple right prism used to quickly eliminate
    test points when querying "is this point inside the polygon"?*/
  double box_max[3]; /* (--)
    Maximum [x,y,z]-values of the simple right prism used to quickly eliminate
    test points when querying "is this point inside the polygon"?*/
//  double max_x;/* (--)
//    Maximum x-value of the coordinates of the polygon vertices.
//    Used to draw a simple right prism for a first-pass when querying "is
//    this point inside the polygon"?*/
//  double min_y;/* (--)
//    Minimum y-value of the coordinates of the polygon vertices.
//    Used to draw a simple right prism for a first-pass when querying "is
//    this point inside the polygon"?*/
//  double max_y;/* (--)
//    Maximum y-value of the coordinates of the polygon vertices.
//    Used to draw a simple right prism for a first-pass when querying "is
//    this point inside the polygon"?*/
//  double min_z;/* (--)
//    Minimum z-value of the coordinates of the polygon vertices.
//    Used to draw a simple right prism for a first-pass when querying "is
//    this point inside the polygon"?*/
//  double max_z;/* (--)
//    Maximum z-value of the coordinates of the polygon vertices.
//    Used to draw a simple right prism for a first-pass when querying "is
//    this point inside the polygon"?*/
//  double vertices_angular[N][2]; /* (--)
//    array of 2-D coordinates (lambda, phi) for N vertices.
//    These values are assigned by set_vertices(...)*/
  /* FIXME don't need to specify _cartesian any more.*/
//  double vertices[N][3]; /* (--)
  std::array< std::array< double, 3>, N>  vertices; /* (--)
    array of 3-D coordinates (x,y,z) for N vertices.
    These values are computed by set_vertices(...)*/
  std::array< std::array< double, 3>, N>  r_cross_next; /* (--)
    The cross product of coordinates of each vertex with those of the vertex
    immediately following it in the set of vertices.
    The "next" vertex from the vertex at the end of the list is the first
    vertex in the list, thereby closing the polygon.*/
  int direction_sign; /* (--)
    Value is +1 if polygon vertices are ordered in a counter-clockwise sequence
    on the x-y plane, -1 if they are ordered clockwise.
    Default: 0 (linearly aligned vertices) */
  bool initialized; /* (--)
    Indicates that initial snaity checks have passed and polygon is valid.*/

 public:
  bool enabled; /* (--)
    Model enabled flag.*/
  const size_t num_pts; /* (--)
    External interface to provide information about the size of this polygon.*/
//  bool lambda_is_0_to_2pi; /* (--)
//    Flag indicating that the lambda values in vertices_angles are in
//    [0,2pi) rather than the default (-pi,pi].*/
  bool apply_bounding_box;  /* (--)
    Falg determines whether to apply a bounding box around the polygon.*/



/*****************************************************************************
Constructor
*****************************************************************************/
  PolygonEnclosureSphere<N>()
    :
    direction_sign(0),
    initialized(false),
    enabled(true),
    num_pts(N),
    apply_bounding_box(true)
  {}

/*****************************************************************************
initialize()
Purpose:
  Sets the min, max values for the enclosing rectangular box,
  verifies the convex nature of the specified vertices, and identifies the
  direction in which the vertices are ordered.
*****************************************************************************/
  void initialize()
  {
    /* Check that there are at least 3 vertices. This could be done at
       construction, but that wouldn't block its usage (unless it was made to
       be a terminal fault). */
    if (N < 3) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot form a polygon with less than 3 vertices.\n"
        "Polygon initialization failed.\n");
      return;
    }

    /* Check that the polygon is convex. See model documentation for details.*/
    for (size_t ix = 0 ; ix < N-1; ix++) {
      r_cross_next[ix] = MathUtils::vector_cross_product( vertices[ix],
                                                          vertices[ix+1]);
    }
    r_cross_next[N-1] = MathUtils::vector_cross_product( vertices[N-1],
                                                         vertices[0]);

    /* First, identify the direction sign of the first pair of side (that are
       not aligned).*/
    std::array<double,3> r_cross_next_prev;
    r_cross_next_prev = MathUtils::vector_cross_product( vertices[1],
                                                         vertices[N-1]);
    direction_sign = MathUtils::sign(
                        MathUtils::vector_scalar_product( vertices[0],
                                                          r_cross_next_prev));
    size_t ix = 1;
    while (direction_sign ==0 && ix < N-1) {
      r_cross_next_prev = MathUtils::vector_cross_product( vertices[ix+1],
                                                           vertices[ix-1]);
      direction_sign = MathUtils::sign(
                          MathUtils::vector_scalar_product(vertices[ix],
                                                           r_cross_next_prev));
      ix++;
    }
    /* Don't bother checking the last vertex. If direction_sign=0 at this
      point, then all points are in a line, and the last trio will also be
      aligned and we don't have a polygon*/
    if (direction_sign == 0) {
      CMLMessage::error( __FILE__,__LINE__,
        "Vertices do not form a polygon.\n"
        "The vertices are all aligned.\n"
        "Polygon initialization failed.\n");
      return;
    }

    /* Now verify each subsequent pair of adjacent sides to ensure that no
       sides produce a concave geometry. Note -- if two edges are aligned,
       the cross product will be zero and this is not sufficient to mark
       the polygon as concave. Concave means the cross product must have the
       opposite sign.
       Continue testing direction-sign with the next side in the list.
    */
    bool convex = true;
    for ( ; ix < N-1; ix++) {
      r_cross_next_prev = MathUtils::vector_cross_product( vertices[ix+1],
                                                           vertices[ix-1]);
      if( MathUtils::sign( MathUtils::vector_scalar_product(
                                                         vertices[ix],
                                                         r_cross_next_prev))
          == -direction_sign) {
        convex = false;
        break;
      }
    }
    /* Also need to check the convex connection around the last vertex.*/
    if ( convex) {
      r_cross_next_prev = MathUtils::vector_cross_product( vertices[0],
                                                           vertices[N-2]);
      if( MathUtils::sign( MathUtils::vector_scalar_product(
                                                        vertices[N-1],
                                                        r_cross_next_prev))
          == -direction_sign) {
        convex = false;
      }
    }

    if (!convex) {
      CMLMessage::error( __FILE__,__LINE__,
        "Vertices do not form a convex polygon.\n"
        "The algorithm used to verify inclusion within the defined polygon\n"
        "will not work if any vertex generates a concave perimeter.\n"
        "Polygon initialization failed.\n");
      return;
    }
    initialized = true;

    if (apply_bounding_box) {
      // temporarily switch off the bounding box.
      apply_bounding_box = false;
      std::array<double,3> R_pole = {0,0,0};

      // for ix:
      for (size_t i_axis = 0; i_axis < 3; i_axis++) {
        MathUtils::zero_vector(R_pole);
        R_pole[i_axis] = 1;
        if (in_polygon( R_pole)) {
          box_max[i_axis] = 2;
          continue;
        }
        // else, compute box-limit from geometry
        // Start wwith the vertices.
        box_max[i_axis] = vertices[0][i_axis];
        for (size_t ix =1; ix < N; ix++) {
          box_max[i_axis] = std::max( box_max[i_axis],
                                      vertices[ix][i_axis]);
        }
        // Get max value from edges if box extends into positive values.
        /* TODO Turner 2024/05:
        *   There is almost certainly a more efficient way to do this, but a
        *   clean form that is applicable on all 3 axes eludes me right now
        *   and I don't have time to look deeper,
        *   To avoid having to implement axis-specific code, I'm leaving
        *   it in the general form despite its inefficiency.
        *   At some point, this should be cleaned up.
        */
        if (box_max[i_axis] <= 0) {
          continue;
        }
        for (size_t ix =0; ix < N; ix++) {
          if ( direction_sign * r_cross_next[ix][i_axis] >=0) {
            continue;
          }
          std::array< double,3> T; // see documentation for interpretation
          T = MathUtils::vector_cross_product(
                  MathUtils::vector_cross_product( r_cross_next[ix],
                                                   R_pole),
                  r_cross_next[ix]);
          double VxTdS = MathUtils::vector_scalar_product(
                            MathUtils::vector_cross_product( vertices[ix],
                                                             T),
                            r_cross_next[ix]);
          if (VxTdS <= 0.0) {
            continue;
          }
          double S_mag = MathUtils::vec_mag( r_cross_next[ix]);
          double T_mag = MathUtils::vec_mag(T);
          if ( VxTdS < S_mag * T_mag) {
            box_max[i_axis] = T[i_axis] / T_mag;
          }
        }
      }

      for (size_t i_axis = 0; i_axis < 3; i_axis++) {
        MathUtils::zero_vector(R_pole);
        R_pole[i_axis] = -1;
        if (in_polygon( R_pole)) {
          box_min[i_axis] = -2;
          continue;
        }
        // else, compute box-min values from geometry
        // Start with the vertices.
        box_min[i_axis] = vertices[0][i_axis];
        for (size_t ix =1; ix < N; ix++) {
          box_min[i_axis] = std::min( box_min[i_axis],
                                      vertices[ix][i_axis]);
        }
        // Get min value from edges if vertices extend into negative values.
        if (box_min[i_axis] >= 0) {
          continue;
        }
        for (size_t ix =0; ix < N; ix++) {
          if ( direction_sign * r_cross_next[ix][i_axis] <=0) {
            continue;
          }
          std::array< double,3> T; // see documentation for interpretation
          T = MathUtils::vector_cross_product(
                  MathUtils::vector_cross_product( r_cross_next[ix],
                                                   R_pole),
                  r_cross_next[ix]);
          double VxTdS = MathUtils::vector_scalar_product(
                            MathUtils::vector_cross_product( vertices[ix],
                                                             T),
                            r_cross_next[ix]);
          if (VxTdS >= 0.0) {
            continue;
          }
          double S_mag = MathUtils::vec_mag( r_cross_next[ix]);
          double T_mag = MathUtils::vec_mag(T);
          if( VxTdS < S_mag * T_mag) {
            box_min[i_axis] = T[i_axis] / T_mag;
          }
        }
      }
      apply_bounding_box = true;
    }
//      /* Start the bounding box by finding the max and min values of the
//         vertices.*/
//      min_x = max_x = vertices[0][0];
//      min_y = max_y = vertices[0][1];
//      min_z = max_z = vertices[0][2];
//      int min_z_ix = 0;
//      int max_z_ix = 0;
//      for (ix =1; ix < N; ix++) {
//        min_x = std::min (min_x, vertices[ix][0]);
//        max_x = std::max (max_x, vertices[ix][0]);
//        min_y = std::min (min_y, vertices[ix][1]);
//        max_y = std::max (max_y, vertices[ix][1]);
//        if (vertices[ix][2] < min_z) {
//          min_z = vertices[ix][2];
//          min_z_ix = ix;
//        }
//        if (vertices[ix][2] > max_z) {
//          max_z = vertices[ix][2];
//          max_z_ix = ix;
//        }
//      }
//
//      // Then consider the effects of wrap-around:
//      /* If there are points with + and - x values, the polygon y-value
//         extends to the sphere edge, not bounded by the vertices.*/
//      if (min_x * max_x < 0) {
//        if (min_y > 0) { max_y = 2;}
//        if (max_y < 0) { min_y = -2;}
//      }
//      /* Similarly, if there are points with + and - y values, the polygon
//       * x-value extends to the sphere edge, not bounded by the vertices.*/
//      if (min_y * max_y < 0) {
//        if (min_x > 0) { max_x = 2;}
//        if (max_x < 0) { min_x = -2;}
//      }
//      /* For x- and y-axis consideration, FIXME need a symmetric solution to
//       * that for z.
//      */
//
//      /* The z-axis bounding is slightly different. If the polygon encompasses
//       * a pole, the z-values are bound by the sphere, not by the vertices.
//       * Otherwise. if the vertex with highest z-value has a positive z-value,
//       * or the vertex with smallest z-value has a negative z-value, the
//       * polygon may be bound by one of the great circles passing through that
//       * extreme vertex.*/
//      double R_pole[3] = {0,0,1};
//      if (in_polygon( R_pole)) {
//        max_z =  2;
//      }
//      else if (max_z > 0) {
//        // test the great-circle coming into this vertex:
//        int adjacent_ix = (max_z_ix ==0)? N-1 : max_z_ix-1;
//        if (extremum_between(adjacent_ix)) {
//          max_z = std::max( max_z,
//                            ( MathUtils::vec_mag_xy( r_cross_next[adjacent_ix]) /
//                              MathUtils::vec_mag( r_cross_next[adjacent_ix])));
//        }
//        // test the great-circle coming out of this vertex:
//        adjacent_ix = (max_z_ix == N-1)? 0 : max_z_ix+1;
//        if (extremum_between( max_z_ix)) {
//          max_z = std::max( max_z,
//                            ( MathUtils::vec_mag_xy( r_cross_next[max_z_ix]) /
//                              MathUtils::vec_mag( r_cross_next[max_z_ix])));
//        }
//      }
//      R_pole[2] = -1;
//      if (in_polygon( R_pole)) {
//        min_z =  -2;
//      }
//      else if (min_z < 0) {
//        // test the great-circle coming into this vertex:
//        int adjacent_ix = (min_z_ix ==0)? N-1 : min_z_ix-1;
//        if (extremum_between(adjacent_ix)) {
//          min_z = std::min( min_z,
//                            -(MathUtils::vec_mag_xy( r_cross_next[adjacent_ix])/
//                              MathUtils::vec_mag( r_cross_next[adjacent_ix])));
//        }
//        // test the great-circle coming out of this vertex:
//        adjacent_ix = (min_z_ix == N-1)? 0 : min_z_ix+1;
//        if (extremum_between( min_z_ix)) {
//          min_z = std::min( min_z,
//                            -(MathUtils::vec_mag_xy( r_cross_next[min_z_ix]) /
//                              MathUtils::vec_mag( r_cross_next[min_z_ix])));
//        }
//      }
//    }



//    initialized = true;
//    min_lambda = max_lambda = vertices_angular[0][0];
//    min_phi    = max_phi    = vertices_angular[0][1];
//    for (ix =1; ix < N; ix++) {
//      min_lambda = std::min (min_lambda, vertices_angles[ix][0]);
//      max_lambda = std::max (max_lambda, vertices_angles[ix][0]);
//      min_phi = std::min (min_phi, vertices_angles[ix][1]);
//      max_phi = std::max (max_phi, vertices_angles[ix][1]);
//    }
//
//    // Check for polar inclusions:
//    double np[3] = {0,0,1};
//    double sp[3] = {0,0,-1};
//    if (in_polygon( 0, -M_PI_2)) {min_phi = -M_PI_2;}
//
//    // Check for great-circles reaching more extreme phi values
//    double lambda_next = vertices_angles[0];
//    for (ix = N-1; ix >= 0; ix--) {
//      /* If the two vertices produce a polar great-circle (or are very
//         close), don't check for great-circle turning points between them.*/
//      if ( std::abs(r_cross_next[ix][2]) < 1.0e-6) {
//        lambda_next = vertices_angles[ix];
//        continue;
//      }
//      double lambda_min_phi = std::atan2( r_cross_next[ix][1],
//                                          r_cross_next[ix][0]);
//      if ( lambda_is_0_to_2pi &&
//           (lambda_min_phi < 0)) {
//        lambda_min_phi += 2*M_PI;
//      }
//
//      if ( (MathUtils::sign( lambda_next         - lambda_min_phi) !=
//           (MathUtils::sign( vertices_angles[ix] - lambda_min_phi) {
//        min_phi = std::min (min_phi,
//                            std::atan( MathUtils::vec_mag_xy(r_cross_next[ix])/
//                                       std::abs(r_cross_next[ix][2])));
//        lambda_next = vertices_angles[ix];
//        continue;
//      }
//      // else:
//      double lambda_max_phi = lambda_min_phi + M_PI;
//      if (   ( lambda_is_0_to_2pi && (lambda_max_phi >= 2 * M_PI))
//          || (!lambda_is_0_to_2pi && (lambda_max_phi > M_PI))) {
//        lambda_max_phi -= 2*M_PI;
//      }
//      if ( (MathUtils::sign( lambda_next         - lambda_max_phi) !=
//           (MathUtils::sign( vertices_angles[ix] - lambda_max_phi) {
//        max_phi = std::max (max_phi,
//                            std::atan( MathUtils::vec_mag_xy(r_cross_next[ix])/
//                                       std::abs(r_cross_next[ix][2])));
//      }
//      lambda_next = vertices_angles[ix];
//    }
  }


///*****************************************************************************
//Name: extremum_between
//Purpose:
//  Consider the great-circle defined by two adjacent vertices
//  This method identifies whether either of the z-axis extrema of that great
//  circle lie between those vertices.
//Note:
//  The argument specifies the index of the first vertex, the second
//  vertex is the one immediately following the one specified.
//*****************************************************************************/
//  bool extremum_between( unsigned int ix1)
//  {
//    // next index:
//    unsigned int ix2 = (ix1 == N-1)? 0 : ix1+1;
//    return
//      ( MathUtils::sign( vertices[ix1][0] * r_cross_next[ix1][1] -
//                         vertices[ix1][1] * r_cross_next[ix1][0])
//        !=
//        MathUtils::sign( vertices[ix2][0] * r_cross_next[ix1][1] -
//                         vertices[ix2][1] * r_cross_next[ix1][0])
//      );
//  }

/*****************************************************************************
in_polygon
Purpose:
  Determines whether the current point lies within the perimeter of the
  polygon.
*****************************************************************************/
  bool in_polygon( double lambda,
                   double phi)
  {
    if (!initialized ||!enabled) {return false;}
    double cos_phi = std::cos( phi);
    double R_P[3] = {cos_phi * std::cos( lambda),
                     cos_phi * std::sin( lambda),
                     std::sin( phi)};
    return in_polygon( R_P);
  }
/****************************************************************************/
  bool in_polygon( const double (&R_P)[3])
  {
    if (!initialized ||!enabled) {return false;}

    // First check if current point (x,y) lies within the rectangular
    // box bounding the polygon. If not, it cannot be inside the polygon,
    if ( apply_bounding_box) {
      for (size_t i_axis = 0; i_axis < 3; i_axis++) {
        if ( (R_P[i_axis] < box_min[i_axis]) ||
             (R_P[i_axis] > box_max[i_axis])) {
          return false;
        }
      }
    }
    /* Else check for the point being "outside" of each side of the polygon.
       If it is outside any side, it is outside the polygon.
       If it passes testing for every side, it is inside the polygon.i
       See model documentation for algorithm, this may not be obvious.
       Note that a point "on the perimeter line" is interpreted as being
       inside the perimeter.*/
    for (size_t  ix = 0 ; ix < N; ix++) {
      if ( MathUtils::sign( MathUtils::vector_scalar_product( r_cross_next[ix],
                                                              R_P))
           == -direction_sign) {
        return false;
      }
    }
    return true;
  }
/****************************************************************************/
  bool in_polygon( const std::array<double,3> & R_P)
  {
    if (!initialized ||!enabled) {return false;}

    // First check if current point (x,y) lies within the rectangular
    // box bounding the polygon. If not, it cannot be inside the polygon,
    if ( apply_bounding_box) {
      for (size_t i_axis = 0; i_axis < 3; i_axis++) {
        if ( (R_P[i_axis] < box_min[i_axis]) ||
             (R_P[i_axis] > box_max[i_axis])) {
          return false;
        }
      }
    }
    /* Else check for the point being "outside" of each side of the polygon.
       If it is outside any side, it is outside the polygon.
       If it passes testing for every side, it is inside the polygon.i
       See model documentation for algorithm, this may not be obvious.
       Note that a point "on the perimeter line" is interpreted as being
       inside the perimeter.*/
    for (size_t  ix = 0 ; ix < N; ix++) {
      if ( MathUtils::sign( MathUtils::vector_scalar_product( r_cross_next[ix],
                                                              R_P))
           == -direction_sign) {
        return false;
      }
    }
    return true;
  }

/*****************************************************************************
set_vertices
Purpose:
  Sets the vertices data
*****************************************************************************/
  void set_vertices(double (&vals)[N][2])
  {
    if (initialized) {
      CMLMessage::error(__FILE__,__LINE__,
        "Cannot define xy data after the polygon has been initialized.\n"
        "Check configuration.\n");
    } else {
      for (size_t ix = 0; ix < N; ix++) {
        double cos_phi = std::cos( vals[ix][1]);
        vertices[ix][0] = cos_phi * std::cos( vals[ix][0]);
        vertices[ix][1] = cos_phi * std::sin( vals[ix][0]);
        vertices[ix][2] = std::sin( vals[ix][1]);
      }
    }
  }
/*****************************************************************************
set_vertices
Purpose:
  Sets the vertices data
*****************************************************************************/
  void set_vertices(double (&vals)[N][3])
  {
    if (initialized) {
      CMLMessage::error(__FILE__,__LINE__,
        "Cannot define xy data after the polygon has been initialized.\n"
        "Check configuration.\n");
    } else {
      for (unsigned int ii = 0; ii < N; ii++) {
        std::copy( std::begin(vals[N]),
                   std::end(vals[N]),
                   vertices[N].data());
      }
    }
  }

 private:
  PolygonEnclosureSphere<N>( const PolygonEnclosureSphere<N> &);
  PolygonEnclosureSphere<N>& operator=( const PolygonEnclosureSphere<N>&);
};
#endif
