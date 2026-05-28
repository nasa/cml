/******************************** TRICK HEADER **********************************
PURPOSE:
  (To provide the formula calculation and intertopation for the compution of 
   U.S. Standard Atmosphere 1976 model.)

REFERENCES:
  ((U.S. STANDARD ATMOSPHERE 1976, N77-16482,
   https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770009539.pdf))

PROGRAMMERS:
  (((Bingquan Wang) (OSR) (May 2017) (ANTARES) (initial version)))

********************************************************************************/

#ifndef STD_ATMOS_FORMULA_H
#define STD_ATMOS_FORMULA_H

#include <cassert>
#include <cmath>
#include <algorithm>
#include "../include/std_atmos_1976.hh"

//to calculate the mass density
static inline double cal_density(const double &press, //pressure, unit:N/m2
                                 const double &k_t,   //kinetic temperature, unit: K
                                 const double &mw    //mean molecular weight, kg/kmol
                                )
{
  return press/STD1976::Rs/k_t*mw;
}

//to calcualte the speed of sound
static inline double cal_cs(const double &m_t //molecular temperature, unit: K
                           )
{
  return std::sqrt(STD1976::gamma*STD1976::Rs/STD1976::M0*m_t);
}

//to calculate the mean free path
static inline double cal_mfp(const double &k_t,    //kinetic temperature, unit: K
                             const double &press   //pressure, unit:N/m2
                            )
{
  return k_t/press*STD1976::mfp_coeff;
}

//to calculate the potential height
static inline double cal_geop_h(const double & altitude  //geometric height, unit: m
                               )
{
  return altitude/(1.0+altitude/STD1976::r0);
}


//to do the trimmed one-dimension linear interpolation
static inline double interp(const double x_vals[],   //the data set of x-axis
                            const double y_vals[],   //the data set of y-axis
                            const size_t &tbl_len,   //the number of data points
                            const double &x          //the x value of interpolated point
                           )
{
  if (x <= x_vals[0]) return y_vals[0];
  if (x >= x_vals[tbl_len-1]) return y_vals[tbl_len-1];
 
  const double *ptr = std::lower_bound(x_vals, x_vals+tbl_len, x);
  const size_t idx = std::distance(x_vals, ptr);

  assert(x_vals[idx]-x_vals[idx-1] > 0);

  return y_vals[idx-1] + (x-x_vals[idx-1])/(x_vals[idx]-x_vals[idx-1])*(y_vals[idx]-y_vals[idx-1]);
}

//to do the trimmed one-dimension logarithm interpolation
static inline double log_interp(const double x_vals[],  //the data set of x-axis
                                const double y_vals[],  //the data set of y-axis
                                const size_t &tbl_len,  //the number of data points
                                const double &x         //the x value of interpolated point
                                )
{
  if (x <= x_vals[0]) return y_vals[0];
  if (x >= x_vals[tbl_len-1]) return y_vals[tbl_len-1];

  const double *ptr = std::lower_bound(x_vals, x_vals+tbl_len, x);
  const size_t idx = std::distance(x_vals, ptr);

  const double log_y2 = std::log(y_vals[idx]);
  const double log_y1 = std::log(y_vals[idx-1]);
  
  assert(x_vals[idx]-x_vals[idx-1] > 0);

  const double log_y = log_y1 + (x-x_vals[idx-1])/(x_vals[idx]-x_vals[idx-1])*(log_y2-log_y1);

  return std::exp(log_y);
}
                                 
#endif //STD_ATMOS_FORMULA_H

