/********************************** TRICK HEADER **************************
PURPOSE:
    (Little algorithm to test whether some coordinate pair lies within some
    planar polygon, defined by a set of (x,y) coordinate pairs describing the
    locations of the vertices.
    This is not limited to a conventional physical plane in space, it may be
    used for any generic parameter space using any 1-D vector or scalr variable
    for each axis. For example, it could describe a temperature~time bounding
    condition, querying whether the temperature is within acceptable bounds at
    a given time.)

LIMITATION:
  (The polygon used to define the allowable 2-D space must be convex. At no
  point can a vertex produce a concave perimeter.)

PROGRAMMERS:
    (
     ((Quang Do, Gary Turner) (OSR) (July 2023) (New with ANTARES V&V))
    )
**************************************************************************/
#ifndef CML_POLYGON_ENCLOSURE_HH
#define CML_POLYGON_ENCLOSURE_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include <cstring> // memcpy
#include <algorithm> // min,max

template<size_t N>
class PolygonEnclosure
{
 protected:
  const double & x; /* (--)
    Reference to the x-axis value of the (x,y) coordinates of the test point
    -- is this point inside the polygon?*/
  const double & y; /* (--)
    Reference to the y-axis value of the (x,y) coordinates of the test point
    -- is this point inside the polygon?*/
  double min_x; /* (--)
    Minimum x-value of the coordinates of the polygon vertices.
    Used to draw a simple rectangular box for a first-pass when qerying "is
    this point inside the polygon"?*/
  double max_x;/* (--)
    Maximum x-value of the coordinates of the polygon vertices.
    Used to draw a simple rectangular box for a first-pass when qerying "is
    this point inside the polygon"?*/
  double min_y;/* (--)
    Minimum y-value of the coordinates of the polygon vertices.
    Used to draw a simple rectangular box for a first-pass when qerying "is
    this point inside the polygon"?*/
  double max_y;/* (--)
    Maximum y-value of the coordinates of the polygon vertices.
    Used to draw a simple rectangular box for a first-pass when qerying "is
    this point inside the polygon"?*/
  double vertices[N][2]; /* (--)
    array of 2-D coordinates (x,y) for N vertices.*/
  double r_cross_next[N]; /* (--)
    The cross product of coordinates of each vertex with those of the vertex
    immediately following it in the set of vertices.
    Because the vertices are both in the x-y plane, the vector product of
    their position vectors has only a z-axis component so this is a single
    value per vector-product.
    The "next" vertex from the vertex at the end of the list is the first
    vertex in the list, thereby closing the polygon.*/
  double r_to_next[N][2]; /* (--)
    the difference in  coordinate values from this vertex to the vertex
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
    External interface to output information about the size of this polygon.*/


/*****************************************************************************
Constructor
*****************************************************************************/
  PolygonEnclosure<N>(
    const double & x_,
    const double & y_)
    :
    x(x_),
    y(y_),
    direction_sign(0),
    initialized(false),
    enabled(true),
    num_pts(N)
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
       be a terminal fault).*/
    if (N < 3) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot form a polygon with less than 3 vertices.\n"
        "Polygon initialization failed.\n");
      return;
    }

    /* For a polygon to be convex, the cross-product of sequential line segments
       must result in a vector with a consistent direction. Because we are
       dealing with polygons on a plane (e.g. x-y plane), the cross-product
       will produce a value perpendicular to that plane (z-axis), so we really
       only need to compute that z-axis value and compare the sign of that
       value.
       Start by generating the vectors defining the sides of the polygon,
       r_to_next. */
    for (size_t ix = 0 ; ix < N-1; ix++) {
      r_to_next[ix][0] = vertices[ix+1][0] - vertices[ix][0];
      r_to_next[ix][1] = vertices[ix+1][1] - vertices[ix][1];
      r_cross_next[ix] = vertices[ix][0] * vertices[ix+1][1] -
                         vertices[ix][1] * vertices[ix+1][0];
    }
    r_to_next[N-1][0 ] = vertices[0][0] - vertices[N-1][0];
    r_to_next[N-1][1] = vertices[0][1] - vertices[N-1][1];
    r_cross_next[N-1] = vertices[N-1][0] * vertices[0][1] -
                        vertices[N-1][1] * vertices[0][0];

    // First, identify the direction sign of the first pair of side (that are
    // not aligned).
    size_t ix = 0;
    while (direction_sign ==0 && ix < N-1) {
      direction_sign = MathUtils::sign( r_to_next[ix][0] * r_to_next[ix+1][1] -
                                        r_to_next[ix][1] * r_to_next[ix+1][0]);
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
    for ( ; ix < N; ix++) {
      if( MathUtils::sign( r_to_next[ix][0] * r_to_next[ix+1][1] -
                           r_to_next[ix][1] * r_to_next[ix+1][0])
          == -direction_sign) {
        convex = false;
        break;
      }
    }
    /* Also need to check the convex connection between the last and first
       edges*/
    if ( !convex ||
         ( MathUtils::sign( r_to_next[N-1][0] * r_to_next[0][1] -
                            r_to_next[N-1][1] * r_to_next[0][0])
           == -direction_sign)) {
      CMLMessage::error( __FILE__,__LINE__,
        "Vertices do not form a convex polygon.\n"
        "The algorithm used to verify inclusion within the defined polygon\n"
        "will not work if any vertex generates a concave perimeter.\n"
        "Polygon initialization failed.\n");
      return;
    }

    min_x = max_x = vertices[0][0];
    min_y = max_y = vertices[0][1];
    for (ix =1; ix < N; ix++) {
      min_x = std::min (min_x, vertices[ix][0]);
      max_x = std::max (max_x, vertices[ix][0]);
      min_y = std::min (min_y, vertices[ix][1]);
      max_y = std::max (max_y, vertices[ix][1]);
    }
    initialized = true;
  }


/*****************************************************************************
in_polygon
Purpose:
  Determines whether the current point lies within the perimeter of the polygon.
*****************************************************************************/
  bool in_polygon()
  {
    if (!initialized ||!enabled) {return false;}

    /* First check if current point (x,y) lies within the rectangular
       box bounding the polygon. If not, it cannot be inside the polygon,*/
    if ( (x < min_x) || (x > max_x) ||
         (y < min_y) || (y > max_y)) {
      return false;
    }

    /* Else check for the point being "outside" of each side of the polygon.
       If it is outside any side, it is outside the polygon.
       If it passes testing for every side, it is inside the polygon.i
       See model documentation for algorithm, this may not be obvious.
       Note that a point "on the perimeter line" is interpreted as being
       inside the perimeter.*/
    for (size_t  ix = 0 ; ix < N; ix++) {
      if ( MathUtils::sign( r_cross_next[ix] -
                            (x * r_to_next[ix][1] -
                             y * r_to_next[ix][0]))
           == -direction_sign) {
        return false;
      }
    }
    return true;
  }

/*****************************************************************************
set_xy
Purpose:
  Sets the vertices data
*****************************************************************************/
  void set_xy(double (&vals)[N][2])
  {
    if (initialized) {
      CMLMessage::error(__FILE__,__LINE__,
        "Cannot define xy data after the polygon has been initialized.\n"
        "Check configuration.\n");
    } else {
      memcpy( vertices, vals, num_pts*2*sizeof(double));
    }
  }

 private:
  PolygonEnclosure<N>( const PolygonEnclosure<N> &);
  PolygonEnclosure<N>& operator=( const PolygonEnclosure<N>&);
};
#endif
