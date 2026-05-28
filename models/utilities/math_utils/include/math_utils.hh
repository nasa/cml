/*******************************TRICK HEADER************************************
 PURPOSE:
   (To provide a common location for additional math utilities)

 LIBRARY DEPENDENCY:
   ((../src/math_utils.cc))

 PROGRAMMERS:
   (((Gary Turner) (OSR) (Sep 2015) (initial version))
    ((Bingquan Wang) (OSR) (May 2017)
                   (Added the float-point number math support))
    ((Daniel Ghan) (OSR) (Jul 2020)
                   (Rearranged files, added log_protected and log10_protected))
    ((Brent Caughron) (OSR) (Oct 2020) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_MATH_UTILS_HH
#define CML_MATH_UTILS_HH

#include <vector>  // mainly for std::vector use
#include <string>  // mainly for std::string use
#include <list>
#include <cstring> // memset

#include "jeod/models/utils/quaternion/include/quat.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

// Function template definitions
#include "cml/models/utilities/math_utils/include/math_utils_private.hh"


class MathUtils : private MathUtilsPrivate {

public:
  // Generates a transformation matrix from inertial to local LVLH given the
  // inertial position and velocity vectors.
  static void generate_inertial_to_lvlh( const double position[3],
                                         const double velocity[3],
                                         double T_inrtl_lvlh[3][3]);

  // Generates a transformation matrix from inertial to local UVW frame
  // given the inertial position and velocity vectors.
  // U - Up
  // V - completes
  // W - orbital angular momentum)
  static void generate_inertial_to_uvw( const double position[3],
                                        const double velocity[3],
                                        double T_inrtl_uvw[3][3]);

  // Generates a transformation matrix from inertial frame to a locally
  // specified reference frame given
  //  - the inertial-orientation of the x-axis of the new reference frame and
  //  - the position of the origin of the new reference frame specified in the
  //    inertial frame
  // x-axis as specified
  // y-axis x-axis cross position
  // z-axis completes.
  static void generate_inrtl_to_reference( const double x_axis_inrtl[3],
                                           const double position[3],
                                           double T_inrtl_reference[3][3]);

  // Generates a transformation matrix from inertial to local VNC frame
  // given the inertial position and velocity vectors.
  // V - velocity
  // N - angular momentum (r x v)
  // C - completes (in orbital plane, generally in direction of radius vector)
  static void generate_inrtl_to_vnc( const double (&position)[3],
                                     const double (&velocity)[3],
                                     double (&T_inrtl_vnc)[3][3]);


  static void generate_T_pfix_to_enu( const double position_ecef[3],
                                      double T_pfix_to_enu[3][3]);
  static void generate_Q_enu_to_pfix( double longitude,
                                      double latitude,
                                      jeod::Quaternion & Q_enu_to_pfix);

  // Evaluate a polynomial function of x with coefficients specified as a
  // STL-vector.  The first element of the coeffs vector is the x^0 term, the
  // last is the x^n term where n = coeffs.size()-1
  static double polynomial( double x,
                            std::vector<double> & coeffs,
                            const double failed_val  = 0.0,
                            const bool   failed_flag = false);
                        // Last 2 optional arguments are for if values near inf.


  // Cholesky-decomposition of an input matrix to generate the "square root" of
  // that matrix and send it to another matrix.
  // The matrices may be stored as a 1-d array or 2-d array.  The input
  // should be symmetric, so column-major vs row-major makes no difference
  // The output is assumed column-major.
  // The addresses of the first element of each array is
  // passed in, along with the size of 1 dimension of the array (e.g. 6x6 array
  // is sized "6").  An optional 5th argument allows processing a smaller
  // sub-matrix.
  static bool cholesky_decomposition( std::string caller_id,
                                      const double * in_array,
                                      double * out_array,
                                      size_t size_in,
                                      size_t size_out = 0); // optional for
                                                            // sub-matrices


  // Compute the backward difference of a variable given its history specified
  // in a STL-list.  The order of the differencing is determined by the length
  // of the history list, up to a maximum of 4th order.
  // This method returns an averaged difference, not a derivative.  The return
  // value is something like: (dy / dt) * (DELTA t);
  // It is the responsibility of the calling function to generate and maintain
  // the history list and divide the result by (DELTA-t) if a derivative value
  // is desired.
  // This method assumes:
  //   - the historical points are equally spaced and
  //   - the front of the list is the most recent data point (and therefore
  //   - the back of the list is the oldest data point)
  static double compute_backward_difference( const std::list<double> & history);


  // Compute the derivative of a unit-vector as a function of the vector and
  // its derivative.  Note that this is distinct from the normalization of the
  // vector-derivative, which yields a unit-vector; the derivative of the unit
  // vector is not generally a unit-vector.
  static void compute_unit_vector_derivative( const double vector[3],
                                              const double vector_derivative[3],
                                              double unit_vector_derivative[3]);


  // Compare if two numbers are equal under the precision controlled by the
  // given ulp (units of last place).
  //
  // For the non-zero of val1 and val2:
  //   ulp=0.5 means only when val1 and val2 are exactly equal, return true.
  //   ulp=1.0 means even when val1 and val2 are consecutive, return true.
  //   ulp=2.0 means even when val1 and val2 are separated by one number,
  //           return true.
  //   ...
  //
  // For the zero value of val1 or val2:
  //   ulp=0.5 means only when exactly be zero, return true.
  //   ulp=1.0 means even when non-zero value is far from zero
  //           by DBL_MIN*DBL_EPSILON, return true.
  //   ulp=2.0 means even when non-zero value is far from zero
  //           by 2*DBL_MIN*DBL_EPSILON, return true.
  //   ...
  static bool is_near_equal( const float val1,
                             const float val2,
                             const float ulp = 0.5f)
  {
    return MathUtilsPrivate::check_equal<float>(val1, val2, ulp);
  }
  static bool is_near_equal( const double val1,
                             const double val2,
                             const double ulp = 0.5)
  {
    return MathUtilsPrivate::check_equal<double>(val1, val2, ulp);
  }
  template <size_t N>
  static bool is_near_equal( const std::array<double, N>& val1,
                             const std::array<double, N>& val2,
                             const double ulp = 0.5)
  {
    for (size_t ii = 0; ii < N; ii++) {
      if (! MathUtilsPrivate::check_equal<double>(val1[ii], val2[ii], ulp)) { return false;}
    }
    return true;
  }
  template <size_t N>
  static bool is_near_equal( const std::array<float, N>& val1,
                             const std::array<float, N>& val2,
                             const float ulp = 0.5)
  {
    for (size_t ii = 0; ii < N; ii++) {
      if (! MathUtilsPrivate::check_equal<float>(val1[ii], val2[ii], ulp)) { return false;}
    }
    return true;
  }

  /* has_changed_from is used to compare two values for equality.
   * It is used to evaluate whether a variable has changed from a previously
   * assigned (and stored) value. It is functionally equivalent to using "!="
   * but semantically different in that its purpose is different.
   * Because the intent is to compare against a previously assigned value, it
   * is reasonable to compare the equality of doubles and floats under these
   * circumstances.
   * Note that this is invoked by is_equal in the case of doubles and floats,
   * but the purpose of calling is_equal differs from that of calling
   * has_changed_from.*/
  template< typename T>
  static bool has_changed_from( T val1, T val2)
  { return ((val1<val2) || (val2<val1));}

  /* is_equal defaults to use ==, but specializes to use !has_changed_from for
  * float and double. Used in class-templates to test equality of values in
  * generic methods where we might be testing floats or doubles (or ints or
  * bools).
  * It should NEVER be used as a substituion for == without careful
  * consideration. It should only be used for floats and doubles when
  * comparing the value of some variable against a value that might
  * reasonably have been previously assigned to that variable.
  * For testing floats or doubles against some independent value,
  * "is_within_abs_tolerance" is a better choice.*/
  template <typename T>
  static bool is_equal( T val1, T val2) { return val1 == val2;}


  /* TODO Turner 2025/03.
   * DEPRECATED CONTENT. Remove after reasonable timeframe.*/
  template <typename T>
  static bool is_within_range( T arg1,
                               T arg2,
                               T range)
  {
    CMLMessage::inform(__FILE__,__LINE__,
      "Deprecated method used: is_within_range.\n"
      "This method name was found to be confusing.\n"
      "Use is_within_abs_tolerance instead.");
    return is_within_abs_tolerance( arg1, arg2, range);
  }
  /* end TODO*/

  template <typename T>
  static bool is_within_abs_tolerance( T val1,
                                       T val2,
                                       T tol)
  {
    // Avoiding using a difference (e.g. val1 - val2) so as to avoid
    // complication of differences evaluating to a negative value and
    // wrapping on an unsigned type.
    if (tol < 0) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid arguments\n",
        "Testing whether two values are within an absolute tolerance\n"
        "is not well-defined when the tolerance is negative.\n"
        "Returning false.\n");
      return false;
    }
    return ((val2 > val1) ?
                val1 + tol >= val2 : 
                val2 + tol >= val1);
  }
  template <typename T>
  static bool is_within_rel_tolerance( T value,
                                       T expected,
                                       double tol)
  {
    // still want to identify as false on a negative tolerance, but fully
    // evaluate a positive tolerance about a negative value.
    if (tol < 0) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid arguments\n",
        "Testing whether two values are within a relative tolerance\n"
        "is not well-defined when the tolerance is negative.\n"
        "Returning false.\n");
      return false;
    }
    return is_within_abs_tolerance( value, expected,
                                      static_cast<T>(tol*std::fabs(expected)));
  }

  template<typename T>
  static int sign (T value) {return ((T(0)<value) - (value<T(0)));}

  // Protected division for two float numbers. When it's div-0 or NaN,
  // it returns the given failed_val.
  // When failed_flag is true, it will terminate the execution
  static float divide_protected( const float dividend,
                                 const float divisor,
                                 const float failed_val  = 0.0f,
                                 const bool  failed_flag = false)
  {
    return MathUtilsPrivate::div_fp<float>(dividend, divisor, failed_val, failed_flag);
  };

  // Same functionality as the above, but for double type rather than float type.
  static double divide_protected( const double dividend,
                                  const double divisor,
                                  const double failed_val = 0.0,
                                  const bool failed_flag = false)
  {
    return MathUtilsPrivate::div_fp<double>(dividend, divisor, failed_val, failed_flag);
  };

  // Protected acos for input value:
  //   val >= 1.0: return 0
  //   val <=-1.0: return PI
  //   others:     return acos(val);
  static float acos_protected( const float val)
  {
    return MathUtilsPrivate::acos_fp<float>(val);
  };

  // Same functionality as the above, but for double type rather than float type.
  static double acos_protected( const double val)
  {
    return MathUtilsPrivate::acos_fp<double>(val);
  };

  // Protected asin for input value:
  //   val >= 1.0:  return  PI/2
  //   val <=-1.0:  return -PI/2
  //   others:      return  asin(val);
  static float asin_protected( const float val)
  {
    return MathUtilsPrivate::asin_fp<float>(val);
  };

  // Same functionality as the above, but for double type rather than float type.
  static double asin_protected( const double val)
  {
    return MathUtilsPrivate::asin_fp<double>(val);
  };
    
  /* TODO Hirad Mirhashemi 2025/10.
  * DEPRECATED CONTENT. Remove after reasonable timeframe.*/
  static float sqrt( float val) 
  { 
    CMLMessage::inform(__FILE__,__LINE__,
      "Deprecated method used: sqrt.\n"
      "This method name was not sufficiently descriptive.\n"
      "Use sqrt_protected instead.");
    return sqrt_protected(val);
  }
  static double sqrt( double val)
  { 
    CMLMessage::inform(__FILE__,__LINE__,
      "Deprecated method used: sqrt.\n"
      "This method name was not sufficiently descriptive.\n"
      "Use sqrt_protected instead.");
    return sqrt_protected(val);
  }
  /* end TODO*/

  static float sqrt_protected( float val)
  { return MathUtilsPrivate::sqrt_fp<float>(val); }
  static double sqrt_protected( double val)
  { return MathUtilsPrivate::sqrt_fp<double>(val); }

  // Protected (natural) log. If the input value is not positive, it returns the
  // given failed_val and, if failed_flag is true, terminates the execution.
  static float log_protected( const float val,
                              const float failed_val = 0.0,
                              const bool failed_flag = false)
  {
    return MathUtilsPrivate::log_fp<float>(val, failed_val, failed_flag, false);
  };

  // Same functionality as the above, but for double type rather than float type
  static double log_protected( const double val,
                               const double failed_val = 0.0,
                               const bool failed_flag = false)
  {
    return MathUtilsPrivate::log_fp<double>(val, failed_val, failed_flag, false);
  };

  // Same as log_protected but for base-10 log.
  static float log10_protected( const float val,
                                const float failed_val = 0.0,
                                const bool failed_flag = false)
  {
    return MathUtilsPrivate::log_fp<float>(val, failed_val, failed_flag, true);
  };

  // Same functionality as the above, but for double type rather than float type
  static double log10_protected( const double val,
                                 const double failed_val = 0.0,
                                 const bool failed_flag = false)
  {
    return MathUtilsPrivate::log_fp<double>(val, failed_val, failed_flag, true);
  };



/*****************************************************************************
Linear Algebra Section
*****************************************************************************/
  // Addition / subtraction of C-style vectors into STL-arrays
  template <size_t N>
  static std::array<double, N> diff( const double (&lhs)[N],
                                     const double (&rhs)[N]) {
    std::array<double, N> result;
    for (size_t ii = 0; ii < N; ++ii) { result[ii] = lhs[ii] - rhs[ii];}
    return result;
  }
  template <size_t N>
  static std::array<double, N> sum( const double (&lhs)[N],
                                    const double (&rhs)[N]) {
    std::array<double, N> result;
    for (size_t ii = 0; ii < N; ++ii) { result[ii] = lhs[ii] + rhs[ii];}
    return result;
  }

  // Vector magnitudes:
  template <size_t N>
  static double vec_mag_sq(const std::array<double, N>& vec) {
    return vector_scalar_product( vec, vec);
  }

  template <size_t N>
  static double vec_mag_sq(const double (&vec)[N]) {
    return vector_scalar_product( vec, vec);
  }

  template <size_t N>
  static double vec_mag(const std::array<double, N>& vec) {
    return sqrt_protected(vec_mag_sq(vec));
  }

  template <size_t N>
  static double vec_mag(const double (&vec)[N]) {
    return sqrt_protected(vec_mag_sq(vec));
  }

  static double vec_mag_xy( const double (&vec)[3]) {
    return sqrt_protected( vec[0]*vec[0] + vec[1]*vec[1] );
  }
  static double vec_mag_xy( const std::array<double, 3>& vec) {
    return sqrt_protected( vec[0]*vec[0] + vec[1]*vec[1] );
  }
  static double vec_mag_xz( const double (&vec)[3]) {
    return sqrt_protected( vec[0]*vec[0] + vec[2]*vec[2] );
  }
  static double vec_mag_xz( const std::array<double, 3>& vec) {
    return sqrt_protected( vec[0]*vec[0] + vec[2]*vec[2] );
  }
  static double vec_mag_yz( const double (&vec)[3]) {
    return sqrt_protected( vec[1]*vec[1] + vec[2]*vec[2] );
  }
  static double vec_mag_yz( const std::array<double, 3>& vec) {
    return sqrt_protected( vec[1]*vec[1] + vec[2]*vec[2] );
  }

  // Normalized vectors:
  template <size_t N>
  static std::array<double, N> unit_vector( const std::array<double, N>& vec) {
    std::array<double, N> result;
    double mag = vec_mag(vec);
    if (mag > 0.0) {
      for (size_t ii = 0; ii < N; ii++) { result[ii] = vec[ii] / mag;}
    } else {
      zero_vector(result);
    }
    return result;
  }
  template <size_t N>
  static std::array<double, N> unit_vector( const double (&vec)[N]) {
    std::array<double, N> result;
    double mag = vec_mag(vec);
    if (mag > 0.0) {
      for (size_t ii = 0; ii < N; ii++) { result[ii] = vec[ii] / mag;}
    } else {
      zero_vector(result);
    }
    return result;
  }

  // zero vector: zeroes the specified vector:
  template <size_t N>
  static void zero_vector( std::array<double, N>& vec) {
    for (size_t ii = 0; ii < N; ii++) { vec[ii] = 0.0;}
  }
  template <size_t N>
  static void zero_vector( double (&vec)[N]) {
    for (size_t ii = 0; ii < N; ii++) { vec[ii] = 0.0;}
  }

  /* TODO Hirad Mirhashemi 2025/10.
  * DEPRECATED CONTENT. Remove after reasonable timeframe.*/
  template <size_t N>
  static std::array<double, N> piecewise_sqrt(
                                         const std::array<double, N>& vec) {
    CMLMessage::inform(__FILE__,__LINE__,
      "Deprecated method used: piecewise_sqrt.\n"
      "This method name was found to be confusing.\n"
      "Use vector_elementwise_sqrt instead.");
    return vector_elementwise_sqrt(vec);
  }

  template <size_t N>
  static std::array<double, N> piecewise_sqrt(
                                         const std::array<double, N>&& vec) {
    CMLMessage::inform(__FILE__,__LINE__,
      "Deprecated method used: piecewise_sqrt.\n"
      "This method name was found to be confusing.\n"
      "Use vector_elementwise_sqrt instead.");
    return vector_elementwise_sqrt(vec);
  }

  template <size_t N>
  static std::array<double, N> piecewise_sqrt(const double (&vec)[N]) {
    CMLMessage::inform(__FILE__,__LINE__,
      "Deprecated method used: piecewise_sqrt.\n"
      "This method name was found to be confusing.\n"
      "Use vector_elementwise_sqrt instead.");
    return vector_elementwise_sqrt(vec);
  }

  template <size_t N>
  static void piecewise_sqrt(const double (&vec)[N],
                             double (&result)[N]) {
    CMLMessage::inform(__FILE__,__LINE__,
      "Deprecated method used: piecewise_sqrt.\n"
      "This method name was found to be confusing.\n"
      "Use vector_elementwise_sqrt instead.");
    return vector_elementwise_sqrt(vec, result);
  }
  /* end TODO*/

  /* Vector element-wise square-root, returns an STL-array with values equal to the
  * square-root of the values of the provided vector / array. */
  template <size_t N>
  static std::array<double, N> vector_elementwise_sqrt(
                                         const std::array<double, N>& vec) {
    std::array<double, N> result;
    for (size_t ii = 0; ii < N; ii++) { result[ii] = sqrt_protected(vec[ii]); }
    return result;
  }

  template <size_t N>
  static std::array<double, N> vector_elementwise_sqrt(
                                         const std::array<double, N>&& vec) {
    for (size_t ii = 0; ii < N; ii++) { vec[ii] = sqrt_protected(vec[ii]); }
    return vec;
  }

  template <size_t N>
  static std::array<double, N> vector_elementwise_sqrt(const double (&vec)[N]) {
    std::array<double, N> result;
    for (size_t ii = 0; ii < N; ii++) { result[ii] = sqrt_protected(vec[ii]); }
    return result;
  }

  template <size_t N>
  static void vector_elementwise_sqrt(const double (&vec)[N],
                             double (&result)[N]) {
    for (size_t ii = 0; ii < N; ii++) { result[ii] = sqrt_protected(vec[ii]); }
  }

  // Vector cross product
  static std::array<double, 3> vector_cross_product(
                                          const std::array<double, 3>& lhs,
                                          const std::array<double, 3>& rhs) {
    return {lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]};
  }
  static std::array<double, 3> vector_cross_product(
                                          const double (&lhs)[3],
                                          const std::array<double, 3>& rhs) {
    return {lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]};
  }
  static std::array<double, 3> vector_cross_product(
                                          const std::array<double, 3>& lhs,
                                          const double (&rhs)[3]) {
    return {lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]};
  }
  static std::array<double, 3> vector_cross_product(
                                          const double (&lhs)[3],
                                          const double (&rhs)[3]) {
    return {lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]};
  }


  // templatized vector scalar product
  template< size_t N>
  static void vector_scalar_product( const double (&left)[N],
                                     const double (&right)[N],
                                     double & result)
  {
    result = vector_scalar_product(left, right);
  }

  template< size_t N>
  static double vector_scalar_product( const double (&left)[N],
                                       const double (&right)[N])
  {
    double result = 0.0;
    for (size_t ii = 0; ii < N; ++ii) { result += left[ii] * right[ii]; }
    return result;
  }

  template <size_t N>
  static double vector_scalar_product( const std::array<double, N>& lhs,
                                       const std::array<double, N>& rhs) {
    double result = 0.0;
    for (size_t ii = 0; ii < N; ii++) { result += lhs[ii] * rhs[ii]; }
    return result;
  }

  template <size_t N>
  static double vector_scalar_product( const double (&lhs)[N],
                                       const std::array<double, N>& rhs) {
    double result = 0.0;
    for (size_t ii = 0; ii < N; ii++) { result += lhs[ii] * rhs[ii]; }
    return result;
  }

  template <size_t N>
  static double vector_scalar_product( const std::array<double, N>& lhs,
                                       const double (&rhs)[N]) {
    double result = 0.0;
    for (size_t ii = 0; ii < N; ii++) { result += lhs[ii] * rhs[ii]; }
    return result;
  }

  /* Build a matrix from a 1-D array arranged in column-major format*/
  template<size_t N, size_t M>
  static void col_maj_vec_to_matrix( const double (&vec)[N*M],
                                     double (&mx)[N][M])
  {
    for (size_t i_row = 0; i_row < N; ++i_row) {
      for (size_t i_col = 0; i_col < M; ++i_col) {
        mx[i_row][i_col] = vec[i_row + N*i_col];
      }
    }
  }
  /* Build a matrix from a 1-D array arranged in row-major format*/
  template<size_t N, size_t M>
  static void row_maj_vec_to_matrix( const double (&vec)[N*M],
                                     double (&mx)[N][M])
  {
    for (size_t i_row = 0; i_row < N; ++i_row) {
      size_t M_row = M*i_row;
      for (size_t i_col = 0; i_col < M; ++i_col) {
        mx[i_row][i_col] = vec[i_col + M_row];
      }
    }
  }

  // zero a matrix:
  template< size_t dim1, size_t dim2>
  static void zero_matrix( double (&out)[dim1][dim2])
  {
    for ( size_t ii = 0; ii < dim1; ii++) {
      for ( size_t jj = 0; jj < dim2; jj++) {
        out[ii][jj] = 0.0;
      }
    }
  }


  // Templatized matrix copy
  template< size_t dim1, size_t dim2>
  static void matrix_copy( const double (&original)[dim1][dim2],
                           double (&copy)[dim1][dim2])
  {
    for ( size_t ii = 0; ii < dim1; ii++) {
      for ( size_t jj = 0; jj < dim2; jj++) {
        copy[ii][jj] = original[ii][jj];
      }
    }
  }

  /* Copy contents of a larger source matrix into a smaller target
   * submatrix with the indices of the upper left corner specified
   * in the source matrix.
   * larger source matrix is  [row1][col1]
   * smaller target matrix is [row2][col2]
   */
  template< size_t row1, size_t col1, size_t row2, size_t col2>
  static void matrix_copy_submatrix_out( const double (&original)[row1][col1],
                                         double (&copy)[row2][col2],
                                         size_t top_row_ix,
                                         size_t left_col_ix)
  {
    size_t bottom_row_out = top_row_ix  + row2;
    size_t right_col_out  = left_col_ix + col2;
    if (bottom_row_out > row1 ||
        right_col_out  > col1) {
      CMLMessage::error(__FILE__,__LINE__,
        "Cannot extract a [",row2,"]x[",col2,"] matrix from a "
        "[",row1,"]x[",col1,"] matrix\n"
        "starting at indices [",top_row_ix,",",left_col_ix,"].\n"
        "Remnant members will be zeroed.\n");
      zero_matrix<row2, col2>(copy);
      bottom_row_out = std::min (bottom_row_out, row1);
      right_col_out  = std::min (right_col_out,  col1);
    }
    for ( size_t ii = top_row_ix; ii < bottom_row_out; ii++) {
      for ( size_t jj = left_col_ix; jj < right_col_out; jj++) {
        copy[ii-top_row_ix][jj-left_col_ix] = original[ii][jj];
      }
    }
  }

  /* Copy a smaller submatrix over elements of a larger target matrix,
   * with the indices of the upper left corner specified in the target
   * matrix.
   * smaller source matrix is [row1][col1]
   * larger target matrix is  [row2][col2]
   */
  template< size_t row1, size_t col1, size_t row2, size_t col2>
  static void matrix_copy_submatrix_in( const double (&original)[row1][col1],
                                        double (&copy)[row2][col2],
                                        size_t top_row_ix,
                                        size_t left_col_ix)
  {
    size_t bottom_row_out = top_row_ix  + row1;
    size_t right_col_out  = left_col_ix + col1;
    if (bottom_row_out > row2 ||
        right_col_out  > col2) {
      CMLMessage::error(__FILE__,__LINE__,
        "Cannot push a [",row1,"]x[",col1,"] matrix onto a"
        "[",row2,"]x[",col2,"] matrix\n"
        "starting at indices [",top_row_ix,",",left_col_ix,"].\n"
        "Remnant members will be ignored.\n");
      bottom_row_out = std::min (bottom_row_out, row2);
      right_col_out  = std::min (right_col_out,  col2);
    }
    for ( size_t ii = top_row_ix; ii < bottom_row_out; ii++) {
      for ( size_t jj = left_col_ix; jj < right_col_out; jj++) {
        copy[ii][jj] = original[ii-top_row_ix][jj-left_col_ix];
      }
    }
  }
  // Templatized matrix increment
  template< size_t dim1, size_t dim2>
  static void matrix_incr( const double (&increment)[dim1][dim2],
                           double (&object)[dim1][dim2])
  {
    for ( size_t ii = 0; ii < dim1; ii++) {
      for ( size_t jj = 0; jj < dim2; jj++) {
        object[ii][jj] += increment[ii][jj];
      }
    }
  }

  // Templatized matrix decrement
  template< size_t dim1, size_t dim2>
  static void matrix_decr( const double (&increment)[dim1][dim2],
                           double (&object)[dim1][dim2])
  {
    for ( size_t ii = 0; ii < dim1; ii++) {
      for ( size_t jj = 0; jj < dim2; jj++) {
        object[ii][jj] -= increment[ii][jj];
      }
    }
  }

  // Templatized matrix scale
  template< size_t dim1, size_t dim2>
  static void matrix_scale( const double sf,
                           double (&object)[dim1][dim2])
  {
    for ( size_t ii = 0; ii < dim1; ii++) {
      for ( size_t jj = 0; jj < dim2; jj++) {
        object[ii][jj] *= sf;
      }
    }
  }

  // Templatized matrix transpose
  template< size_t dim1, size_t dim2>
  static void matrix_trans( const double (&in)[dim1][dim2],
                            double (&result)[dim2][dim1])
  {
    for ( size_t ii = 0; ii < dim2; ii++) {
      for ( size_t jj = 0; jj < dim1; jj++) {
        result[ii][jj] = in[jj][ii];
      }
    }
  }

  // Templatized matrix multiplication: [result] = [left] * [right]
  template< size_t dim1, size_t dim2, size_t dim3>
  static void matrix_mult( const double (&left)[dim1][dim2],
                           const double (&right)[dim2][dim3],
                           double (&result)[dim1][dim3])
  {
    for ( size_t ii = 0; ii < dim1; ii++) {
      for ( size_t jj = 0; jj < dim3; jj++) {
        result[ii][jj] = 0.0;
        for ( size_t kk = 0; kk < dim2; kk++) {
          result[ii][jj] += left[ii][kk] * right[kk][jj];
        }
      }
    }
  }

  // Templatized matrix multiplication: [result] = [left]' * [right]
  template< size_t dim1, size_t dim2, size_t dim3>
  static void matrix_mult_left_trans( const double (&left)[dim1][dim2],
                                      const double (&right)[dim1][dim3],
                                      double (&result)[dim2][dim3])
  {
    double left_trans[dim2][dim1];
    matrix_trans(left, left_trans);
    matrix_mult(left_trans, right, result);
  }

  // Templatized matrix multiplication: [result] = [left] * [right]'
  template< size_t dim1, size_t dim2, size_t dim3>
  static void matrix_mult_right_trans( const double (&left)[dim1][dim2],
                                       const double (&right)[dim3][dim2],
                                       double (&result)[dim1][dim3])
  {
    for (size_t ii = 0; ii < dim1; ++ii) {
      for (size_t jj = 0; jj < dim3; ++jj) {
        vector_scalar_product( left[ii], right[jj], result[ii][jj]);
      }
    }
  }

  // Templatized matrix multiplication: [result] = [left]' * [right]'
  template< size_t dim1, size_t dim2, size_t dim3>
  static void matrix_mult_trans_trans( const double (&left)[dim1][dim2],
                                       const double (&right)[dim3][dim1],
                                       double (&result)[dim2][dim3])
  {
    double left_trans[dim2][dim1];
    matrix_trans(left, left_trans);
    matrix_mult_right_trans(left_trans, right, result);
  }

  // transform a matrix: [A'] = [T] [A] [T]'.
  // A is square, T is usually square, with T' = T_inv.
  template< size_t dim1, size_t dim2>
  static void matrix_transformation( const double (&transform)[dim1][dim2],
                                     const double (&target)[dim2][dim2],
                                     double       (&result)[dim1][dim1])
  {
    double scratch[dim1][dim2];
    matrix_mult( transform, target, scratch);
    matrix_mult_right_trans( scratch, transform, result);
  }

  // transform a matrix using the inverse of the known transform:
  //     [A'] = [T]' [A] [T].
  // A is square, T is usually square, with T' = T_inv.
  template< size_t dim1, size_t dim2>
  static void matrix_inverse_transformation( const double (&transform)[dim1][dim2],
                                             const double (&target)[dim1][dim1],
                                             double       (&result)[dim2][dim2])
  {
    double scratch[dim2][dim1];
    matrix_mult_left_trans( transform, target, scratch);
    matrix_mult( scratch, transform, result);
  }

  /* transform a PV 6x6 matrix from frameA to frameB. Each 3x3 sub-matrix
   * needs transforming independently using the A-to-B transformation
   * matrix, and then collecting back together.*/
  static void transform_pv_matrix( const double (&T_mx)[3][3],
                                   const double (&pv_in)[6][6],
                                   double       (&pv_out)[6][6])
  {
    double scratch_in[3][3];
    double scratch_out[3][3];
    for (size_t ii = 0; ii < 2; ii++) {
      for (size_t jj = 0; jj < 2; jj++) {
        matrix_copy_submatrix_out<6,6,3,3>( pv_in, scratch_in, ii*3, jj*3);
        matrix_transformation<3,3> (T_mx, scratch_in, scratch_out);
        matrix_copy_submatrix_in<3,3,6,6>( scratch_out, pv_out, ii*3, jj*3);
      }
    }
  }


/*****************************************************************************
extract_correlation_coeffs
Purpose: Extract the correlation coefficients from a Covariance matrix
Assumption: Covariance is populated on the lower diagonal.
*****************************************************************************/
  template <size_t N>
  static bool extract_correlation_coefficients( const double cov[N][N],
                                                double corr[N][N])
  {
    // extract the diagonals
    double sqrt_diag[N];
    for (size_t i_row = 0; i_row < N; ++i_row) {
      if (cov[i_row][i_row] < 0) {
        CMLMessage::error(__FILE__,__LINE__,
          "Invalid covariance matrix, a diagonal element is negative.\n"
          "Correlation coefficients not defined.\n Aborting.\n");
        return false;
      }
      if (cov[i_row][i_row] <= 0.0) { // i.e. == 0.0
        for (size_t i_lower_col = 0; i_lower_col < i_row; ++i_lower_col) {
          if ( !is_equal( cov[i_row][i_lower_col], 0.0)) {
            CMLMessage::error(__FILE__,__LINE__,
              "Invalid covariance matrix, a diagonal element is zero\n"
              "with non-zero off-diagonals.\n"
              "Correlation coefficients not defined.\n Aborting.\n");
              return false;
          }
        }
        for (size_t i_lower_row = i_row+1; i_lower_row < N; ++i_lower_row) {
          if ( !is_equal( cov[i_lower_row][i_row], 0.0)) {
            CMLMessage::error(__FILE__,__LINE__,
              "Invalid covariance matrix, a diagonal element is zero\n"
              "with non-zero off-diagonals.\n"
              "Correlation coefficients not defined.\n Aborting.\n");
              return false;
          }
        }
        // Avoid blowing up on 0/0:
        sqrt_diag[i_row] = 1.0;
        CMLMessage::inform(__FILE__,__LINE__,
          "Covariance matrix contains a diagonal element that is zero\n"
          "with zero off-diagonals.\n"
          "Correlation coefficients not defined.\n"
          "Setting diagonal element to 1.0.\n");
      }
      else {
        sqrt_diag[i_row] = sqrt_protected(cov[i_row][i_row]);
      }
    }
    for (size_t i_row = 0; i_row < N; ++i_row) {
      corr[i_row][i_row] = 1.0;
      for (size_t i_lower_col = 0; i_lower_col < i_row; ++i_lower_col) {
        corr[i_row][i_lower_col] = corr[i_lower_col][i_row] = cov[i_row][i_lower_col] /
                                      (sqrt_diag[i_row] * sqrt_diag[i_lower_col]);
      }
    }
    return true;
  }


/*****************************************************************************
matrix_inv_using_cholesky
Purpose:
 Computes inverse of a square positive-definite matrix using
 Cholesky decomposition.
*****************************************************************************/
  template <size_t dimension>
  static bool matrix_inv_using_cholesky(
          const double   (in_array)[dimension][dimension],
          double         (&result)[dimension][dimension],
          bool           fail_on_error = true)
  {
    double C_decomp[dimension][dimension];
    bool success = cholesky_decomposition( "Matrix-Inverse",
                                           &in_array[0][0],
                                           &C_decomp[0][0],
                                           dimension);
    if (success) { // if decomposition succeeded, check for zeros.
      for (size_t ii = 0; ii < dimension; ii++) {
        if (is_near_equal(C_decomp[ii][ii], 0)) {
          success = false;
          break;
        }
      }
    }
    // if failed in either the decomposition or the zero-test, cannot use this
    // method to find the inverse of the matrix.
    if (!success) {
      if (fail_on_error) {
        CMLMessage::fail(
          __FILE__,__LINE__,"Matrix Inverse failed\n",
          "Cholesky decomposition of the provided matrix failed,\n"
          "causing the matrix-inversion to fail.\n"
          "Check that the provided matrix is positive semi-definite.\n");
      }
      else {
        CMLMessage::error(
          __FILE__,__LINE__,"Matrix Inverse failed\n",
          "Cholesky decomposition of the provided matrix failed,\n"
          "causing the matrix-inversion to fail.\n"
          "Check that the provided matrix is positive semi-definite.\n"
          "Per setting of fail_on_error, setting the inverse to a zero matrix"
          "\n and continuing to allow calling function to recover.\n");
        memset( result, 0, sizeof(result));
      }
      return false;
    }

    // With a well-formed decomposed matrix, we can find the inverse of the
    // input matrix from the inverse of the decomposed matrix.
    // [in] = [C_decomp] * [C_decomp]'  => [inv(in)] = [inv(C)]' * [inv(C)]
    // and the inverse of the decomposed matrix is arithmetically simple to
    // compute.
    double C_inv[dimension][dimension] = {0};
    for ( size_t ii = 0; ii < dimension; ii++) {
      // Note -- division by zero already checked above and method has
      // returned if any diagonal elements are zero.
      C_inv[ii][ii] = 1/C_decomp[ii][ii];
      for ( size_t jj = ii+1; jj < dimension; jj++) {
        for (size_t kk = ii; kk < jj; kk++) {
          C_inv[jj][ii] -= C_decomp[jj][kk] * C_inv[kk][ii];
        }
        C_inv[jj][ii] /= C_decomp[jj][jj];
      }
    }
    matrix_mult_left_trans( C_inv, C_inv, result);
    return true;
  }
};

/*****************************************************************************
Forward declaration of template specializations;
implementation is found in src/math_utils.cc
*****************************************************************************/
template<> bool MathUtils::is_equal<float>( float val1, float val2);
template<> bool MathUtils::is_equal<double>( double val1, double val2);
template<> bool MathUtils::is_within_abs_tolerance<bool>( bool, bool, bool);

#endif
