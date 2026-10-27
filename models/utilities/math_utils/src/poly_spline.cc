/******************************************************************************
PURPOSE:
  (Implementation of polynomial spline interpolation - linear, quadratic, cubic)
******************************************************************************/

#include <cmath>
#include "../include/poly_spline.hh"

/******************************************************************************
 * PolyCoeffs Implementation
 *****************************************************************************/

PolyCoeffs::PolyCoeffs()
  : valid(false),
    order(PolyCoeffs::Order::CUBIC)
{
}

/******************************************************************************
 * PolyExtrapConfig Implementation
 *****************************************************************************/

PolyExtrapConfig::PolyExtrapConfig()
  : below(PolyExtrapConfig::Mode::EXTRAPOLATE),
    above(PolyExtrapConfig::Mode::EXTRAPOLATE),
    fill_value_below(0.0),
    fill_value_above(0.0)
{
}

/******************************************************************************
 * PolySplineConfig Implementation
 *****************************************************************************/

PolySplineConfig::PolySplineConfig()
  : bc(PolySplineConfig::BoundaryCondition::NOT_A_KNOT),
    left_deriv(0.0),
    right_deriv(0.0)
{
}

/******************************************************************************
 * compute_coeffs
 *****************************************************************************/
bool
PolySpline::compute_coeffs(const std::vector<double>& x,
                           const std::vector<double>& y,
                           PolyCoeffs::Order order,
                           PolyCoeffs& coeffs,
                           const PolySplineConfig& config)
{
  coeffs.valid = false;
  coeffs.order = order;

  const size_t n = x.size();

  // Validate inputs
  size_t min_points = static_cast<size_t>(order) + 1;
  if (config.bc == PolySplineConfig::BoundaryCondition::NOT_A_KNOT
      && order == PolyCoeffs::Order::CUBIC) {
    min_points = 4;
  }

  if (n < min_points) {
    CMLMessage::fail(__FILE__, __LINE__,
        "PolySpline::compute_coeffs: Insufficient data points.\n"
        "At least ", min_points, " points required, received ", n, ".\n");
    return false;
  }

  if (y.size() != x.size()) {
    CMLMessage::fail(__FILE__, __LINE__,
        "PolySpline::compute_coeffs: Size mismatch.\n"
        "x size (", x.size(), ") must equal y size (", y.size(), ").\n");
    return false;
  }

  // Compute interval widths and validate monotonicity
  coeffs.h.resize(n - 1);
  for (size_t i = 0; i < n - 1; ++i) {
    coeffs.h[i] = x[i + 1] - x[i];
    if (coeffs.h[i] <= 0.0) {
      CMLMessage::fail(__FILE__, __LINE__,
          "PolySpline::compute_coeffs: Grid not monotonically increasing.\n"
          "x[", i + 1, "] - x[", i, "] = ", coeffs.h[i], ".\n");
      return false;
    }
  }

  // Store original data
  coeffs.x = x;
  coeffs.y = y;

  // Dispatch to appropriate solver
  switch (order) {
    case PolyCoeffs::Order::LINEAR:
      return compute_linear_coeffs(x, y, coeffs);

    case PolyCoeffs::Order::QUADRATIC:
      return compute_quadratic_coeffs(x, y, config, coeffs);

    case PolyCoeffs::Order::CUBIC:
      return compute_cubic_coeffs(x, y, config, coeffs);

    default:
      CMLMessage::fail(__FILE__, __LINE__,
          "PolySpline::compute_coeffs: Unsupported spline order.\n");
      return false;
  }
}

/******************************************************************************
 * compute_linear_coeffs
 *****************************************************************************/
bool
PolySpline::compute_linear_coeffs(const std::vector<double>& x,
                                  const std::vector<double>& y,
                                  PolyCoeffs& coeffs)
{
  const size_t n = x.size();
  const size_t n_intervals = n - 1;

  coeffs.a.resize(n_intervals);
  coeffs.b.resize(n_intervals);
  coeffs.c.resize(n_intervals, 0.0);
  coeffs.d.resize(n_intervals, 0.0);

  for (size_t i = 0; i < n_intervals; ++i) {
    coeffs.a[i] = y[i];
    coeffs.b[i] = (y[i + 1] - y[i]) / coeffs.h[i];
  }

  coeffs.valid = true;
  return true;
}

/******************************************************************************
 * compute_quadratic_coeffs
 *****************************************************************************/
bool
PolySpline::compute_quadratic_coeffs(const std::vector<double>& x,
                                     const std::vector<double>& y,
                                     const PolySplineConfig& config,
                                     PolyCoeffs& coeffs)
{
  const size_t n = x.size();
  const size_t n_intervals = n - 1;

  coeffs.a.resize(n_intervals);
  coeffs.b.resize(n_intervals);
  coeffs.c.resize(n_intervals);
  coeffs.d.resize(n_intervals, 0.0);

  // For quadratic splines, we solve for the first derivatives at each knot
  // then compute coefficients from those.
  //
  // Continuity of first derivative gives us n-2 equations for n unknowns,
  // so we need 2 boundary conditions. We use one BC and propagate forward.

  std::vector<double> m(n);  // First derivatives at knots

  // Set initial derivative based on boundary condition
  if (config.bc == PolySplineConfig::BoundaryCondition::CLAMPED) {
    m[0] = config.left_deriv;
  }
  else if (config.bc == PolySplineConfig::BoundaryCondition::FIRST_LINEAR) {
    // First segment is linear: m[0] = m[1] = slope of first segment
    m[0] = (y[1] - y[0]) / coeffs.h[0];
  }
  else if (config.bc == PolySplineConfig::BoundaryCondition::NATURAL) {
    // c[0] = 0 implies m[0] = (y[1] - y[0]) / h[0] (linear first segment)
    m[0] = (y[1] - y[0]) / coeffs.h[0];
  }
  else {
    // Default: use finite difference estimate
    m[0] = (y[1] - y[0]) / coeffs.h[0];
  }

  // Propagate forward using continuity of first derivative
  // At each interior knot: m[i+1] = 2 * (y[i+1] - y[i]) / h[i] - m[i]
  for (size_t i = 0; i < n_intervals; ++i) {
    m[i + 1] = 2.0 * (y[i + 1] - y[i]) / coeffs.h[i] - m[i];
  }

  // Compute coefficients for each interval
  // S_i(x) = a[i] + b[i]*(x - x[i]) + c[i]*(x - x[i])^2
  for (size_t i = 0; i < n_intervals; ++i) {
    coeffs.a[i] = y[i];
    coeffs.b[i] = m[i];
    coeffs.c[i] = (m[i + 1] - m[i]) / (2.0 * coeffs.h[i]);
  }

  coeffs.valid = true;
  return true;
}

/******************************************************************************
 * compute_cubic_coeffs
 *****************************************************************************/
bool
PolySpline::compute_cubic_coeffs(const std::vector<double>& x,
                                 const std::vector<double>& y,
                                 const PolySplineConfig& config,
                                 PolyCoeffs& coeffs)
{
  const size_t n = x.size();
  const size_t n_intervals = n - 1;

  // Build tridiagonal system for second derivatives M[0..n-1]
  std::vector<double> diag(n, 0.0);
  std::vector<double> upper(n, 0.0);
  std::vector<double> lower(n, 0.0);
  std::vector<double> upper2(n, 0.0);  // For not-a-knot fill-in
  std::vector<double> rhs(n, 0.0);

  double lower2_last = 0.0;  // For not-a-knot fill-in in last row

  // Set up boundary conditions and interior equations
  if (config.bc == PolySplineConfig::BoundaryCondition::NOT_A_KNOT) {
    // First row: h[1]*M[0] - (h[0]+h[1])*M[1] + h[0]*M[2] = 0
    diag[0]   =  coeffs.h[1];
    upper[0]  = -(coeffs.h[0] + coeffs.h[1]);
    upper2[0] =  coeffs.h[0];
    rhs[0]    =  0.0;

    // Last row: h[n-2]*M[n-3] - (h[n-3]+h[n-2])*M[n-2] + h[n-3]*M[n-1] = 0
    lower2_last  =  coeffs.h[n - 2];
    lower[n - 1] = -(coeffs.h[n - 3] + coeffs.h[n - 2]);
    diag[n - 1]  =  coeffs.h[n - 3];
    rhs[n - 1]   =  0.0;
  }
  else if (config.bc == PolySplineConfig::BoundaryCondition::NATURAL) {
    // M[0] = 0, M[n-1] = 0
    diag[0]  = 1.0;
    upper[0] = 0.0;
    rhs[0]   = 0.0;

    lower[n - 1] = 0.0;
    diag[n - 1]  = 1.0;
    rhs[n - 1]   = 0.0;
  }
  else if (config.bc == PolySplineConfig::BoundaryCondition::CLAMPED) {
    // 2*h[0]*M[0] + h[0]*M[1] = 6*((y[1]-y[0])/h[0] - left_deriv)
    diag[0]  = 2.0 * coeffs.h[0];
    upper[0] = coeffs.h[0];
    rhs[0]   = 6.0 * ((y[1] - y[0]) / coeffs.h[0] - config.left_deriv);

    // h[n-2]*M[n-2] + 2*h[n-2]*M[n-1] = 6*(right_deriv - (y[n-1]-y[n-2])/h[n-2])
    lower[n - 1] = coeffs.h[n - 2];
    diag[n - 1]  = 2.0 * coeffs.h[n - 2];
    rhs[n - 1]   = 6.0 * (config.right_deriv - (y[n - 1] - y[n - 2]) / coeffs.h[n - 2]);
  }
  else {
    CMLMessage::fail(__FILE__, __LINE__,
        "PolySpline::compute_cubic_coeffs: Unsupported boundary condition.\n");
    return false;
  }

  // Interior rows
  for (size_t i = 1; i < n - 1; ++i) {
    lower[i] = coeffs.h[i - 1];
    diag[i]  = 2.0 * (coeffs.h[i - 1] + coeffs.h[i]);
    upper[i] = coeffs.h[i];
    rhs[i]   = 6.0 * ((y[i + 1] - y[i]) / coeffs.h[i]
                    - (y[i] - y[i - 1]) / coeffs.h[i - 1]);
  }

  // Forward elimination
  std::vector<double> M(n);

  if (config.bc == PolySplineConfig::BoundaryCondition::NOT_A_KNOT) {
    // Handle not-a-knot fill-in

    // Step 1: Eliminate lower[1] using row 0
    if (MathUtils::is_near_equal(diag[0], 0.0)) {
      CMLMessage::fail(__FILE__, __LINE__,
          "PolySpline::compute_cubic_coeffs: Zero pivot in row 0.\n");
      return false;
    }

    double factor = lower[1] / diag[0];
    diag[1]   -= factor * upper[0];
    upper[1]  -= factor * upper2[0];
    upper2[1]  = 0.0;
    rhs[1]    -= factor * rhs[0];
    lower[1]   = 0.0;

    // Step 2: Eliminate lower[2] using row 1
    if (MathUtils::is_near_equal(diag[1], 0.0)) {
      CMLMessage::fail(__FILE__, __LINE__,
          "PolySpline::compute_cubic_coeffs: Zero pivot in row 1.\n");
      return false;
    }

    factor = lower[2] / diag[1];
    diag[2]   -= factor * upper[1];
    upper[2]  -= factor * upper2[1];
    rhs[2]    -= factor * rhs[1];
    lower[2]   = 0.0;

    // Standard tridiagonal elimination for rows 3 to n-2
    for (size_t i = 3; i < n - 1; ++i) {
      if (MathUtils::is_near_equal(diag[i - 1], 0.0)) {
        CMLMessage::fail(__FILE__, __LINE__,
            "PolySpline::compute_cubic_coeffs: Zero pivot in row ", i - 1, ".\n");
        return false;
      }
      factor = lower[i] / diag[i - 1];
      diag[i] -= factor * upper[i - 1];
      rhs[i]  -= factor * rhs[i - 1];
      lower[i] = 0.0;
    }

    // Handle last row's lower2_last (coefficient for M[n-3])
    if (MathUtils::is_near_equal(diag[n - 3], 0.0)) {
      CMLMessage::fail(__FILE__, __LINE__,
          "PolySpline::compute_cubic_coeffs: Zero pivot in row ", n - 3, ".\n");
      return false;
    }

    factor = lower2_last / diag[n - 3];
    lower[n - 1] -= factor * upper[n - 3];
    rhs[n - 1]   -= factor * rhs[n - 3];

    // Eliminate lower[n-1] using row n-2
    if (MathUtils::is_near_equal(diag[n - 2], 0.0)) {
      CMLMessage::fail(__FILE__, __LINE__,
          "PolySpline::compute_cubic_coeffs: Zero pivot in row ", n - 2, ".\n");
      return false;
    }

    factor = lower[n - 1] / diag[n - 2];
    diag[n - 1] -= factor * upper[n - 2];
    rhs[n - 1]  -= factor * rhs[n - 2];
    lower[n - 1] = 0.0;

    // Back substitution
    if (MathUtils::is_near_equal(diag[n - 1], 0.0)) {
      CMLMessage::fail(__FILE__, __LINE__,
          "PolySpline::compute_cubic_coeffs: Zero pivot in row ", n - 1, ".\n");
      return false;
    }

    M[n - 1] = rhs[n - 1] / diag[n - 1];

    for (size_t i = n - 2; i >= 2; --i) {
      M[i] = (rhs[i] - upper[i] * M[i + 1]) / diag[i];
    }

    // Row 1 has upper2[1] term from fill-in
    M[1] = (rhs[1] - upper[1] * M[2] - upper2[1] * M[3]) / diag[1];

    // Row 0 has upper2[0] term from not-a-knot condition
    M[0] = (rhs[0] - upper[0] * M[1] - upper2[0] * M[2]) / diag[0];
  }
  else {
    // Standard Thomas algorithm for natural/clamped BCs

    // Forward elimination
    for (size_t i = 1; i < n; ++i) {
      if (MathUtils::is_near_equal(diag[i - 1], 0.0)) {
        CMLMessage::fail(__FILE__, __LINE__,
            "PolySpline::compute_cubic_coeffs: Zero pivot in row ", i - 1, ".\n");
        return false;
      }
      double factor = lower[i] / diag[i - 1];
      diag[i] -= factor * upper[i - 1];
      rhs[i]  -= factor * rhs[i - 1];
    }

    // Back substitution
    if (MathUtils::is_near_equal(diag[n - 1], 0.0)) {
      CMLMessage::fail(__FILE__, __LINE__,
          "PolySpline::compute_cubic_coeffs: Zero pivot in row ", n - 1, ".\n");
      return false;
    }

    M[n - 1] = rhs[n - 1] / diag[n - 1];

    for (size_t i = n - 1; i-- > 0; ) {
      M[i] = (rhs[i] - upper[i] * M[i + 1]) / diag[i];
    }
  }

  // Convert second derivatives M to polynomial coefficients
  // S_i(x) = a[i] + b[i]*(x - x[i]) + c[i]*(x - x[i])^2 + d[i]*(x - x[i])^3
  coeffs.a.resize(n_intervals);
  coeffs.b.resize(n_intervals);
  coeffs.c.resize(n_intervals);
  coeffs.d.resize(n_intervals);

  for (size_t i = 0; i < n_intervals; ++i) {
    double hi = coeffs.h[i];
    coeffs.a[i] = y[i];
    coeffs.b[i] = (y[i + 1] - y[i]) / hi - hi * (2.0 * M[i] + M[i + 1]) / 6.0;
    coeffs.c[i] = M[i] / 2.0;
    coeffs.d[i] = (M[i + 1] - M[i]) / (6.0 * hi);
  }

  coeffs.valid = true;
  return true;
}

/******************************************************************************
 * find_interval
 *****************************************************************************/
int
PolySpline::find_interval(const PolyCoeffs& coeffs, double x_eval)
{
  const size_t n = coeffs.x.size();

  if (x_eval <= coeffs.x[0]) {
    return -1;  // Below lower bound
  }
  else if (x_eval >= coeffs.x[n - 1]) {
    return static_cast<int>(n);   // Above upper bound
  }
  else {
    size_t lo = 0;
    size_t hi = n - 1;
    while (hi - lo > 1) {
      size_t mid = (lo + hi) / 2;
      if (coeffs.x[mid] <= x_eval) {
        lo = mid;
      }
      else {
        hi = mid;
      }
    }
    return static_cast<int>(lo);
  }
}

/******************************************************************************
 * evaluate_at_interval
 *****************************************************************************/
double
PolySpline::evaluate_at_interval(const PolyCoeffs& coeffs, size_t i, double x_eval)
{
  double dx = x_eval - coeffs.x[i];
  return coeffs.a[i] + dx * (coeffs.b[i] + dx * (coeffs.c[i] + dx * coeffs.d[i]));
}

/******************************************************************************
 * evaluate_derivative_at_interval
 *****************************************************************************/
double
PolySpline::evaluate_derivative_at_interval(const PolyCoeffs& coeffs, size_t i,
                                            double x_eval, int deriv_order)
{
  double dx = x_eval - coeffs.x[i];

  switch (deriv_order) {
    case 0:
      return coeffs.a[i] + dx * (coeffs.b[i] + dx * (coeffs.c[i] + dx * coeffs.d[i]));
    case 1:
      return coeffs.b[i] + dx * (2.0 * coeffs.c[i] + dx * 3.0 * coeffs.d[i]);
    case 2:
      return 2.0 * coeffs.c[i] + dx * 6.0 * coeffs.d[i];
    case 3:
      return 6.0 * coeffs.d[i];
    default:
      return 0.0;
  }
}

/******************************************************************************
 * evaluate
 *****************************************************************************/
bool
PolySpline::evaluate(const PolyCoeffs& coeffs,
                     double x_eval,
                     double& y_eval,
                     const PolyExtrapConfig& extrap)
{
  if (!coeffs.valid) {
    CMLMessage::fail(__FILE__, __LINE__,
        "PolySpline::evaluate: Invalid spline coefficients.\n"
        "Call compute_coeffs first.\n");
    return false;
  }

  const size_t n = coeffs.x.size();
  const int i = find_interval(coeffs, x_eval);

  // Handle below lower bound
  if (i == -1) {
    switch (extrap.below) {
      case PolyExtrapConfig::Mode::EXTRAPOLATE:
        y_eval = evaluate_at_interval(coeffs, 0, x_eval);
        break;
      case PolyExtrapConfig::Mode::CONSTANT:
        y_eval = coeffs.y[0];
        break;
      case PolyExtrapConfig::Mode::LINEAR:
        y_eval = coeffs.y[0] + coeffs.b[0] * (x_eval - coeffs.x[0]);
        break;
      case PolyExtrapConfig::Mode::FILL_VALUE:
        y_eval = extrap.fill_value_below;
        break;
    }
    return true;
  }

  // Handle above upper bound
  if (i == static_cast<int>(n)) {
    switch (extrap.above) {
      case PolyExtrapConfig::Mode::EXTRAPOLATE:
        y_eval = evaluate_at_interval(coeffs, n - 2, x_eval);
        break;
      case PolyExtrapConfig::Mode::CONSTANT:
        y_eval = coeffs.y[n - 1];
        break;
      case PolyExtrapConfig::Mode::LINEAR: {
        double dy_end = evaluate_derivative_at_interval(coeffs, n - 2, coeffs.x[n - 1], 1);
        y_eval = coeffs.y[n - 1] + dy_end * (x_eval - coeffs.x[n - 1]);
        break;
      }
      case PolyExtrapConfig::Mode::FILL_VALUE:
        y_eval = extrap.fill_value_above;
        break;
    }
    return true;
  }

  // Normal case: within bounds
  y_eval = evaluate_at_interval(coeffs, static_cast<size_t>(i), x_eval);
  return true;
}

/******************************************************************************
 * evaluate_derivative
 *****************************************************************************/
bool
PolySpline::evaluate_derivative(const PolyCoeffs& coeffs,
                                double x_eval,
                                double& dy_eval,
                                int deriv_order,
                                const PolyExtrapConfig& extrap)
{
  if (!coeffs.valid) {
    CMLMessage::fail(__FILE__, __LINE__,
        "PolySpline::evaluate_derivative: Invalid spline coefficients.\n"
        "Call compute_coeffs first.\n");
    return false;
  }

  if (deriv_order < 0 || deriv_order > 3) {
    CMLMessage::fail(__FILE__, __LINE__,
        "PolySpline::evaluate_derivative: Invalid derivative order ", deriv_order, ".\n"
        "Must be between 0 and 3.\n");
    return false;
  }

  const size_t n = coeffs.x.size();
  const int i = find_interval(coeffs, x_eval);

  // Handle below lower bound
  if (i == -1) {
    switch (extrap.below) {
      case PolyExtrapConfig::Mode::EXTRAPOLATE:
        dy_eval = evaluate_derivative_at_interval(coeffs, 0, x_eval, deriv_order);
        break;
      case PolyExtrapConfig::Mode::CONSTANT:
        dy_eval = (deriv_order == 0) ? coeffs.y[0] : 0.0;
        break;
      case PolyExtrapConfig::Mode::LINEAR:
        if (deriv_order == 0) {
          dy_eval = coeffs.y[0] + coeffs.b[0] * (x_eval - coeffs.x[0]);
        }
        else if (deriv_order == 1) {
          dy_eval = coeffs.b[0];
        }
        else {
          dy_eval = 0.0;
        }
        break;
      case PolyExtrapConfig::Mode::FILL_VALUE:
        dy_eval = (deriv_order == 0) ? extrap.fill_value_below : 0.0;
        break;
    }
    return true;
  }

  // Handle above upper bound
  if (i == static_cast<int>(n)) {
    switch (extrap.above) {
      case PolyExtrapConfig::Mode::EXTRAPOLATE:
        dy_eval = evaluate_derivative_at_interval(coeffs, n - 2, x_eval, deriv_order);
        break;
      case PolyExtrapConfig::Mode::CONSTANT:
        dy_eval = (deriv_order == 0) ? coeffs.y[n - 1] : 0.0;
        break;
      case PolyExtrapConfig::Mode::LINEAR: {
        double dy_end = evaluate_derivative_at_interval(coeffs, n - 2, coeffs.x[n - 1], 1);
        if (deriv_order == 0) {
          dy_eval = coeffs.y[n - 1] + dy_end * (x_eval - coeffs.x[n - 1]);
        }
        else if (deriv_order == 1) {
          dy_eval = dy_end;
        }
        else {
          dy_eval = 0.0;
        }
        break;
      }
      case PolyExtrapConfig::Mode::FILL_VALUE:
        dy_eval = (deriv_order == 0) ? extrap.fill_value_above : 0.0;
        break;
    }
    return true;
  }

  // Normal case: within bounds
  dy_eval = evaluate_derivative_at_interval(coeffs, static_cast<size_t>(i), x_eval, deriv_order);
  return true;
}

/******************************************************************************
 * evaluate_array
 *****************************************************************************/
bool
PolySpline::evaluate_array(const PolyCoeffs& coeffs,
                           const std::vector<double>& x_eval,
                           std::vector<double>& y_eval,
                           const PolyExtrapConfig& extrap)
{
  if (!coeffs.valid) {
    CMLMessage::fail(__FILE__, __LINE__,
        "PolySpline::evaluate_array: Invalid spline coefficients.\n"
        "Call compute_coeffs first.\n");
    return false;
  }

  const size_t n_eval = x_eval.size();
  y_eval.resize(n_eval);

  for (size_t i = 0; i < n_eval; ++i) {
    if (!evaluate(coeffs, x_eval[i], y_eval[i], extrap)) {
      return false;
    }
  }

  return true;
}

/******************************************************************************
 * resample
 *****************************************************************************/
bool
PolySpline::resample(const std::vector<double>& x,
                     const std::vector<double>& y,
                     const std::vector<double>& x_resampled,
                     std::vector<double>& y_resampled,
                     PolyCoeffs::Order order,
                     const PolySplineConfig& config,
                     const PolyExtrapConfig& extrap)
{
  if (x_resampled.empty()) {
    CMLMessage::fail(__FILE__, __LINE__,
        "PolySpline::resample: Empty x_resampled array.\n"
        "At least one evaluation point is required.\n");
    return false;
  }

  PolyCoeffs coeffs;
  if (!compute_coeffs(x, y, order, coeffs, config)) {
    return false;
  }

  return evaluate_array(coeffs, x_resampled, y_resampled, extrap);
}