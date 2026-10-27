/******************************************************************************
PURPOSE:
  (Implementation of IIR (Infinite Impulse Response) filter utilities)
******************************************************************************/

#include "../include/iir_filter.hh"

/*****************************************************************************
 * IIRCoeffs Implementation
 *****************************************************************************/

IIRCoeffs::IIRCoeffs()
  : valid(false)
{
}

/*****************************************************************************
 * IIRFilter Implementation
 *****************************************************************************/

IIRFilter::IIRFilter()
  : gain(0.95),
    order(6),
    wavelength(2428.0),
    grid_spacing(10.0)
{
}

/*****************************************************************************
 * poly_from_complex_roots
 *****************************************************************************/
bool
IIRFilter::poly_from_complex_roots(
    const std::vector<std::complex<double>>& roots,
    std::vector<std::complex<double>>& coeffs)
{
  const size_t n = roots.size();
  coeffs.assign(n + 1, {0.0, 0.0});
  coeffs[0] = {1.0, 0.0};

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = i + 1; j > 0; --j) {
      coeffs[j] = coeffs[j] - roots[i] * coeffs[j - 1];
    }
  }
  return true;
}

/*****************************************************************************
 * design_analog_lowpass_poles
 *****************************************************************************/
bool
IIRFilter::design_analog_lowpass_poles(
    std::vector<std::complex<double>>& poles)
{
  if (order < 1) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::design_analog_lowpass_poles: Invalid filter order.\n"
        "Order must be >= 1, received ", order, ".\n"
        "Cannot design analog filter.\n");
    return false;
  }

  const size_t n = static_cast<size_t>(order);
  poles.resize(n);
  for (size_t k = 0; k < n; ++k) {
    double theta = M_PI * (2.0 * static_cast<double>(k) + order + 1.0) / (2.0 * order);
    poles[k] = std::complex<double>(std::cos(theta), std::sin(theta));
  }
  return true;
}

/*****************************************************************************
 * bilinear_transform
 *****************************************************************************/
bool
IIRFilter::bilinear_transform(
    double cutoff_wavelength,
    const std::vector<std::complex<double>>& poles,
    IIRCoeffs& coeffs)
{
  coeffs.valid = false;

  if (poles.empty()) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::bilinear_transform: Empty poles vector.\n"
        "At least one pole is required for filter design.\n");
    return false;
  }

  if (cutoff_wavelength <= 0.0) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::bilinear_transform: Invalid cutoff wavelength.\n"
        "Cutoff wavelength must be > 0, received ", cutoff_wavelength, ".\n");
    return false;
  }

  if (grid_spacing <= 0.0) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::bilinear_transform: Invalid sample wavelength.\n"
        "Sample wavelength must be > 0, received ", grid_spacing, ".\n");
    return false;
  }

  const size_t n = poles.size();
  const double fs = 2.0;
  const double K = 2.0 * fs * std::tan(M_PI * grid_spacing / cutoff_wavelength);

  // Bilinear transform each pole: s -> z
  std::vector<std::complex<double>> digital_poles(n);
  for (size_t i = 0; i < n; ++i) {
    std::complex<double> sK = K * poles[i];
    std::complex<double> denom = 1.0 - sK / (2.0 * fs);
    if (MathUtils::is_near_equal(std::abs(denom), 0.0)) {
      CMLMessage::fail(__FILE__, __LINE__,
          "IIRFilter::bilinear_transform: Numerical instability.\n"
          "Denominator near zero during pole transformation.\n"
          "Check filter parameters.\n");
      return false;
    }
    digital_poles[i] = (1.0 + sK / (2.0 * fs)) / denom;
  }

  // Denominator: poly(digital_poles)
  std::vector<std::complex<double>> den_complex;
  if (!poly_from_complex_roots(digital_poles, den_complex)) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::bilinear_transform: "
        "Failed to compute denominator polynomial.\n");
    return false;
  }

  std::vector<double> den(n + 1);
  for (size_t i = 0; i <= n; ++i) {
    den[i] = den_complex[i].real();
  }

  // Numerator: poly(-ones(n)), scaled to match DC gain
  std::vector<std::complex<double>> neg_ones(n, {-1.0, 0.0});
  std::vector<std::complex<double>> z_complex;
  if (!poly_from_complex_roots(neg_ones, z_complex)) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::bilinear_transform: "
        "Failed to compute numerator polynomial.\n");
    return false;
  }

  std::vector<double> z(n + 1);
  for (size_t i = 0; i <= n; ++i) {
    z[i] = z_complex[i].real();
  }

  // Scale to match DC gain
  const double sum_den = std::accumulate(den.begin(), den.end(), 0.0);
  const double sum_z   = std::accumulate(z.begin(), z.end(), 0.0);

  if (MathUtils::is_near_equal(sum_z, 0.0)) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::bilinear_transform: "
        "Cannot compute DC gain scaling.\n"
        "Sum of numerator coefficients is zero.\n");
    return false;
  }

  if (MathUtils::is_near_equal(den[0], 0.0)) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::bilinear_transform: "
        "Cannot normalize coefficients.\n"
        "Leading denominator coefficient is zero.\n");
    return false;
  }

  const double scale = MathUtils::divide_protected(sum_den, sum_z);

  // Normalize coefficients
  coeffs.num.resize(n + 1);
  coeffs.den.resize(n + 1);
  for (size_t i = 0; i <= n; ++i) {
    coeffs.num[i] = MathUtils::divide_protected(z[i] * scale, den[0]);
    coeffs.den[i] = MathUtils::divide_protected(den[i], den[0]);
  }

  coeffs.valid = true;
  return true;
}

/*****************************************************************************
 * apply
 *****************************************************************************/
bool
IIRFilter::apply(
    const std::vector<double>& num,
    const std::vector<double>& den,
    const std::vector<double>& data,
    std::vector<double>& out)
{
  if (den.empty()) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::apply: Empty denominator coefficients.\n"
        "At least one denominator coefficient is required.\n");
    return false;
  }

  if (num.empty()) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::apply: Empty numerator coefficients.\n"
        "At least one numerator coefficient is required.\n");
    return false;
  }

  if (num.size() != den.size()) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::apply: Coefficient size mismatch.\n"
        "Numerator size (", num.size(), ") must equal denominator size (",
        den.size(), ").\n");
    return false;
  }

  const size_t n_data = data.size();
  const size_t n_state = den.size() - 1;

  out.assign(n_data, 0.0);
  std::vector<double> z(n_state, 0.0);

  for (size_t i = 0; i < n_data; ++i) {
    out[i] = num[0] * data[i] + (n_state > 0 ? z[0] : 0.0);
    for (size_t j = 0; j + 1 < n_state; ++j) {
      z[j] = num[j + 1] * data[i] - den[j + 1] * out[i] + z[j + 1];
    }
    if (n_state > 0) {
      z[n_state - 1] = num[n_state] * data[i] - den[n_state] * out[i];
    }
  }
  return true;
}

/*****************************************************************************
 * apply_zero_phase
 *****************************************************************************/
bool
IIRFilter::apply_zero_phase(
    const IIRCoeffs& coeffs,
    const std::vector<double>& data,
    std::vector<double>& out)
{
  const size_t n = data.size();

  if (n < 2) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::apply_zero_phase: Insufficient data points.\n"
        "Data must have at least 2 points, received ", n, ".\n"
        "Cannot apply mirror padding.\n");
    return false;
  }

  if (!coeffs.valid) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::apply_zero_phase: Invalid filter coefficients.\n"
        "IIRCoeffs not properly initialized.\n");
    return false;
  }

  // Build mirror-padded signal: [reflected_lo | data | reflected_hi]
  std::vector<double> mirrored(3 * n - 2);

  // Low mirror: 2*data[0] - data[n-1:0:-1]
  for (size_t i = 0; i < n - 1; ++i) {
    mirrored[i] = 2.0 * data[0] - data[n - 1 - i];
  }

  // Center: original data
  for (size_t i = 0; i < n; ++i) {
    mirrored[n - 1 + i] = data[i];
  }

  // High mirror: 2*data[n-1] - data[n-2::-1]
  for (size_t i = 0; i < n - 1; ++i) {
    mirrored[2 * n - 1 + i] = 2.0 * data[n - 1] - data[n - 2 - i];
  }

  // Forward pass
  std::vector<double> fwd;
  if (!apply(coeffs.num, coeffs.den, mirrored, fwd)) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::apply_zero_phase: Forward filter pass failed.\n");
    return false;
  }

  // Reverse
  std::reverse(fwd.begin(), fwd.end());

  // Backward pass
  std::vector<double> bwd;
  if (!apply(coeffs.num, coeffs.den, fwd, bwd)) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::apply_zero_phase: Backward filter pass failed.\n");
    return false;
  }

  // Reverse back and trim to original data region [n-1 : 2n-1]
  std::reverse(bwd.begin(), bwd.end());
  const auto first = static_cast<std::ptrdiff_t>(n - 1);
  const auto last  = static_cast<std::ptrdiff_t>(2 * n - 1);
  out.assign(bwd.begin() + first, bwd.begin() + last);

  return true;
}

/*****************************************************************************
 * design_lowpass
 *****************************************************************************/
bool
IIRFilter::design_lowpass(
    double cutoff_wavelength,
    IIRCoeffs& coeffs)
{
  std::vector<std::complex<double>> poles;
  if (!design_analog_lowpass_poles(poles)) {
    return false;
  }
  return bilinear_transform(cutoff_wavelength, poles, coeffs);
}

/*****************************************************************************
 * design_and_apply_lowpass
 *****************************************************************************/
bool
IIRFilter::design_and_apply_lowpass(
    const std::vector<double>& data,
    std::vector<double>& out)
{
  if (gain <= 0.0 || gain >= 1.0) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::design_and_apply_lowpass: Invalid gain value.\n"
        "Gain must be in range (0, 1), received ", gain, ".\n"
        "Cannot compute cutoff wavelength.\n");
    return false;
  }

  if (wavelength <= 0.0) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::design_and_apply_lowpass: Invalid wavelength.\n"
        "wavelength must be > 0, received ", wavelength, ".\n");
    return false;
  }

  if (grid_spacing <= 0.0) {
    CMLMessage::fail(__FILE__, __LINE__,
        "IIRFilter::design_and_apply_lowpass: Invalid sample wavelength.\n"
        "Sample wavelength must be > 0, received ", grid_spacing, ".\n");
    return false;
  }

  // Compute cutoff wavelength from desired gain
  const double cutoff = wavelength * std::pow(
      MathUtils::divide_protected(1.0, gain) - 1.0,
      MathUtils::divide_protected(1.0, 2.0 * order));

  IIRCoeffs coeffs;
  if (!design_lowpass(cutoff, coeffs)) {
    return false;
  }

  return apply_zero_phase(coeffs, data, out);
}