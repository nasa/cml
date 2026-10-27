/******************************************************************************
PURPOSE:
  (Implementation of polynomial spline interpolation - linear, quadratic, cubic)

LIBRARY DEPENDENCY:
  ((../src/poly_spline.cc))

PROGRAMMERS:
  ((Jonathan Jermstad) (OSR) (Apr 2026) (Initial Version))
******************************************************************************/

#ifndef CML_POLY_SPLINE_HH
#define CML_POLY_SPLINE_HH

#include <vector>
#include "../include/math_utils.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/******************************************************************************
 * PolyCoeffs - stores polynomial spline coefficients
 ******************************************************************************/
class PolyCoeffs
{
public:

  /**************************************************************************
   * Order - supported spline orders
   **************************************************************************/
  enum class Order
  {
    LINEAR    = 1,
    QUADRATIC = 2,
    CUBIC     = 3
  };

  PolyCoeffs();

  bool valid;  /* (--)
      True if coefficients have been successfully computed and are safe to
      use for evaluation. Set by PolySpline::compute_coeffs on success. */

  Order order;  /* (--)
      Polynomial order used to compute these coefficients. Determines which
      coefficient vectors are populated and which terms are applied during
      evaluation:
        LINEAR    - only a, b populated
        QUADRATIC - only a, b, c populated
        CUBIC     - a, b, c, d all populated */

  std::vector<double> x;  /* (--)
      Knot positions (independent variable). Must be strictly monotonically
      increasing. Size n, where n is the number of data points. */

  std::vector<double> y;  /* (--)
      Knot values (dependent variable) corresponding to each position in x.
      Size n. */

  std::vector<double> h;  /* (--)
      Interval widths between consecutive knots: h[i] = x[i+1] - x[i].
      Size n-1. All values are strictly positive. */

  std::vector<double> a;  /* (--)
      Constant spline coefficients for each interval. Equal to y[i] by
      construction: a[i] = y[i]. Size n-1. */

  std::vector<double> b;  /* (--)
      Linear spline coefficients for each interval. Represents the first
      derivative contribution: b[i]*(x - x[i]). Size n-1. */

  std::vector<double> c;  /* (--)
      Quadratic spline coefficients for each interval. Represents the second
      derivative contribution: c[i]*(x - x[i])^2. Size n-1. Zero for
      LINEAR order. */

  std::vector<double> d;  /* (--)
      Cubic spline coefficients for each interval. Represents the third
      derivative contribution: d[i]*(x - x[i])^3. Size n-1. Zero for
      LINEAR and QUADRATIC order. */
};

/******************************************************************************
 * PolyExtrapConfig - extrapolation behavior outside data bounds
 ******************************************************************************/
class PolyExtrapConfig
{
public:

  /**************************************************************************
   * Mode - extrapolation behavior outside data bounds
   **************************************************************************/
  enum class Mode
  {
    EXTRAPOLATE,  // Continue spline polynomial beyond bounds (default)
    CONSTANT,     // Use endpoint values beyond bounds
    LINEAR,       // Linear extrapolation using endpoint derivatives
    FILL_VALUE    // Use specified fill value beyond bounds
  };

  PolyExtrapConfig();

  Mode below;  /* (--)
      Extrapolation mode applied when x_eval < x[0]. Defaults to
      EXTRAPOLATE. FILL_VALUE mode uses fill_value_below. */

  Mode above;  /* (--)
      Extrapolation mode applied when x_eval > x[n-1]. Defaults to
      EXTRAPOLATE. FILL_VALUE mode uses fill_value_above. */

  double fill_value_below;  /* (--)
      Value returned when x_eval < x[0] and below == FILL_VALUE.
      Ignored for all other modes. Defaults to 0.0. */

  double fill_value_above;  /* (--)
      Value returned when x_eval > x[n-1] and above == FILL_VALUE.
      Ignored for all other modes. Defaults to 0.0. */
};

/******************************************************************************
 * PolySplineConfig - computation-time configuration for spline fitting
 ******************************************************************************/
class PolySplineConfig
{
public:

  /**************************************************************************
   * BoundaryCondition - boundary condition types
   **************************************************************************/
  enum class BoundaryCondition
  {
    NOT_A_KNOT,   // Third derivative continuous at second/second-to-last knots (cubic)
    NATURAL,      // Second derivative = 0 at endpoints
    CLAMPED,      // First derivative specified at endpoints via left/right_deriv
    FIRST_LINEAR  // First segment is linear (quadratic only)
  };

  PolySplineConfig();

  BoundaryCondition bc;  /* (--)
      Boundary condition applied when computing spline coefficients.
      Determines how the system of equations is closed at the endpoints.
      CLAMPED requires left_deriv and right_deriv to be set.
      FIRST_LINEAR is only valid for QUADRATIC order.
      Defaults to NOT_A_KNOT. */

  double left_deriv;  /* (--)
      Prescribed first derivative at x[0]. Only used when bc == CLAMPED.
      Ignored for all other boundary conditions. Defaults to 0.0. */

  double right_deriv;  /* (--)
      Prescribed first derivative at x[n-1]. Only used when bc == CLAMPED.
      Ignored for all other boundary conditions. Defaults to 0.0. */
};

/******************************************************************************
 * PolySpline - computes and evaluates polynomial spline interpolations
 ******************************************************************************/
class PolySpline
{
public:

  // Convenience aliases
  using Order       = PolyCoeffs::Order;
  using ExtrapMode  = PolyExtrapConfig::Mode;
  using BoundaryCondition = PolySplineConfig::BoundaryCondition;

  /**************************************************************************
   * compute_coeffs - compute spline coefficients
   **************************************************************************/
  static bool compute_coeffs(const std::vector<double>& x,
                             const std::vector<double>& y,
                             PolyCoeffs::Order order,
                             PolyCoeffs& coeffs,
                             const PolySplineConfig& config = PolySplineConfig());

  /**************************************************************************
   * evaluate - evaluate spline at a single point
   **************************************************************************/
  static bool evaluate(const PolyCoeffs& coeffs,
                       double x_eval,
                       double& y_eval,
                       const PolyExtrapConfig& extrap = PolyExtrapConfig());

  /**************************************************************************
   * evaluate_derivative - evaluate spline derivative at a single point
   **************************************************************************/
  static bool evaluate_derivative(const PolyCoeffs& coeffs,
                                  double x_eval,
                                  double& dy_eval,
                                  int deriv_order = 1,
                                  const PolyExtrapConfig& extrap = PolyExtrapConfig());

  /**************************************************************************
   * evaluate_array - evaluate spline at multiple points
   **************************************************************************/
  static bool evaluate_array(const PolyCoeffs& coeffs,
                             const std::vector<double>& x_eval,
                             std::vector<double>& y_eval,
                             const PolyExtrapConfig& extrap = PolyExtrapConfig());

  /**************************************************************************
   * resample - convenience function to compute and evaluate in one call
   **************************************************************************/
  static bool resample(const std::vector<double>& x,
                       const std::vector<double>& y,
                       const std::vector<double>& x_resampled,
                       std::vector<double>& y_resampled,
                       PolyCoeffs::Order order       = PolyCoeffs::Order::CUBIC,
                       const PolySplineConfig& config = PolySplineConfig(),
                       const PolyExtrapConfig& extrap = PolyExtrapConfig());

private:

  static bool compute_linear_coeffs(const std::vector<double>& x,
                                    const std::vector<double>& y,
                                    PolyCoeffs& coeffs);

  static bool compute_quadratic_coeffs(const std::vector<double>& x,
                                       const std::vector<double>& y,
                                       const PolySplineConfig& config,
                                       PolyCoeffs& coeffs);

  static bool compute_cubic_coeffs(const std::vector<double>& x,
                                   const std::vector<double>& y,
                                   const PolySplineConfig& config,
                                   PolyCoeffs& coeffs);

  static int find_interval(const PolyCoeffs& coeffs,
                           double x_eval);

  static double evaluate_at_interval(const PolyCoeffs& coeffs,
                                     size_t i,
                                     double x_eval);

  static double evaluate_derivative_at_interval(const PolyCoeffs& coeffs,
                                                size_t i,
                                                double x_eval,
                                                int deriv_order);
};

#endif // CML_POLY_SPLINE_HH