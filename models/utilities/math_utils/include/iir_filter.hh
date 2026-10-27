/*******************************TRICK HEADER************************************
 PURPOSE:
   IIR (Infinite Impulse Response) filter utilities providing:
   - Polynomial operations (coefficient computation from roots)
   - Digital signal processing (IIR filters, bilinear transform)
   - Zero-phase filtering with mirror padding

 LIBRARY DEPENDENCY:
   ((../src/iir_filter.cc))

 PROGRAMMERS:
   (((Jonathan Jermstad) (OSR) (April 2026) (initial version)))
*******************************************************************************/

#ifndef CML_IIR_FILTER_HH
#define CML_IIR_FILTER_HH

#include <algorithm>  // std::reverse
#include <cmath>      // M_PI, std::tan, std::cos, std::sin, std::pow, std::abs
#include <complex>    // std::complex
#include <numeric>    // std::accumulate
#include <vector>     // std::vector
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "../include/math_utils.hh"

/******************************************************************************
 * IIRCoeffs - stores IIR filter numerator and denominator coefficients
 ******************************************************************************/
class IIRCoeffs
{
public:

  IIRCoeffs();

  bool valid;  /* (--)
      True if coefficients have been successfully computed and are safe to
      use for filtering. Set by IIRFilter::design_lowpass on success. */

  std::vector<double> num;  /* (--)
      Numerator (feedforward) coefficients of the digital filter transfer
      function. Ordered from lowest to highest power of z^-1.
      Size order+1. */

  std::vector<double> den;  /* (--)
      Denominator (feedback) coefficients of the digital filter transfer
      function. Ordered from lowest to highest power of z^-1.
      den[0] is always 1.0 (monic). Size order+1. */
};

/******************************************************************************
 * IIRFilter - designs and applies IIR lowpass filters
 ******************************************************************************/
class IIRFilter
{
public:

  double gain;  /* (--)
      Desired filter gain at the specified wavelength.
      Must be in range (0, 1). Typical value: 0.95.
      Used to compute the cutoff wavelength via:
        cutoff = wavelength * ((1/gain - 1)^(1/(2*order))) */

  int order;  /* (--)
      Filter order (number of poles).
      Higher order gives steeper rolloff but more ringing.
      Typical values: 4-6.
      Rolloff rate: -20*order dB/decade beyond cutoff. */

  double wavelength;  /* (m)
      Spatial wavelength at which the specified gain is achieved.
      For atmospheric wind filtering, this represents the spatial
      scale in meters at which the filter attenuates to the
      specified gain level. */

  double grid_spacing;  /* (m)
      Uniform grid spacing used for resampling before filtering.
      Data is interpolated onto a uniform grid with this spacing
      prior to filter application. Should be small enough to
      capture the features of interest in the original data. */

  IIRFilter();
  virtual ~IIRFilter() = default;

  bool design_and_apply_lowpass(
    const std::vector<double>& data,
    std::vector<double>& out
  );

private:

  bool poly_from_complex_roots(
    const std::vector<std::complex<double>>& roots,
    std::vector<std::complex<double>>& coeffs
  );

  bool design_analog_lowpass_poles(
    std::vector<std::complex<double>>& poles
  );

  bool bilinear_transform(
    double cutoff_wavelength,
    const std::vector<std::complex<double>>& poles,
    IIRCoeffs& coeffs
  );

  bool apply(
    const std::vector<double>& num,
    const std::vector<double>& den,
    const std::vector<double>& data,
    std::vector<double>& out
  );

  bool apply_zero_phase(
    const IIRCoeffs& coeffs,
    const std::vector<double>& data,
    std::vector<double>& out
  );

  bool design_lowpass(
    double cutoff_wavelength,
    IIRCoeffs& coeffs
  );
};

#endif // CML_IIR_FILTER_HH