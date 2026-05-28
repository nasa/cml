/******************************************************************************
PURPOSE:
    (Interpolation functions for 1D, 2D, 3D, 4D and 1D-angle tables)

REFERENCE:
    ((C port of SES function))

ASSUMPTIONS AND LIMITATIONS:
    ((The fractional_distance and low_breakpont_index are precalculated by SCALE)
     (0 <= low_breakpoint_index <= num_of_breakpoints-2)
     (0.0 <= fractional_distance <= 1.0)
    )
 
PROGRAMMERS:
    (
     ((Bingquan Wang) (OSR) (May 2017) (ANTARES) (refactored from the original
                    multiple C files: table1c.c, table2c.c table3c.d, table4c.c
                    scacelc.c and table1c_angle.c written by several developers.))
    )
 
*******************************************************************************/

#ifndef TABLE_INTERP_IMPL_H
#define TABLE_INTERP_IMPL_H

#include <cstdlib> // NULL
#include <assert.h>
#include "table_interp_helper.h"

/* Function to calculate the lower breakpoint index and fractional
 * distance in interval for the given lookup value
 * Return:  0 -- success
 *         -1 -- the input value is beyond the low end, and the output is
 *               set as the low end boundary.
 *         -2 -- the input value is beyond the high end, and the output is
 *               set as the high end boundary.
 */
static inline 
int scale(
  const double value,    /*IN:  (--) Lookup value                       */
  double *frac,          /*OUT: (--) Fractional distance in interval    */
  int *low_index,        /*OUT: (--) Lower breakpoint index of interval */
  const double *tbl_bpt, /*IN:  (--) Pointer to the breakpoint table    */
  const int num_bps)     /*IN   (--) Number of breakpoints              */
{
  assert(NULL!=frac && "Pointer frac cannot be NULL!");
  assert(NULL!=low_index && "Pointer low_index cannot be NULL!");
  assert(NULL!=tbl_bpt && "Pointer tbl_val cannot be NULL!");

  const int is_asc = tbl_bpt[num_bps-1] > tbl_bpt[0];

  if ((is_asc && value<tbl_bpt[0]) || (!is_asc && value>tbl_bpt[0])) {
    *low_index = 0;
    *frac = 0.0;
    return -1;
  }

  if ((is_asc && value>tbl_bpt[num_bps-1]) || (!is_asc && value<tbl_bpt[num_bps-1])) {
    *low_index = num_bps-2;
    *frac  = 1.0;
    return -2;
  }
  
  cal_scale(value, frac, low_index, tbl_bpt, num_bps, is_asc);

  return 0;
}


/* Function to calculate the interpolated value on 1-dimension table. */
static inline 
double table1(
  const double *tbl_val, /*IN: (--) Pointer to 1-dimension value table    */
  const double frac,     /*IN: (--) Fractional distance  on X dimension   */
  const int low_index)   /*IN: (--) Lower breakpoint index on X dimension */
{
  assert(NULL!=tbl_val && "Pointer tbl_val cannot be NULL!");
  return cal_interp_1d(tbl_val, frac, low_index);
}


/* Function to calculate the interpolated value on 2-dimension table */
static inline
double table2(
  const double *tbl_val, /*IN: (--) Pointer to 1D array for the 2D value table
                                    with Y-dimension allocated first      */
  const double frac_x,   /*IN: (--) Fractional distance on X dimension   */
  const int low_index_x, /*IN: (--) Lower breakpoint index on X dimension */
  const double frac_y,   /*IN: (--) Fractional distance on Y dimension   */
  const int low_index_y, /*IN: (--) Lower breakpont index on Y dimension  */
  const int ny)          /*IN: (--) Number of breakpoints on Y dimension  */
{
  assert(NULL!=tbl_val && "Pointer tbl_val cannot be NULL!");
  return  cal_interp_2d(tbl_val, frac_x, low_index_x, frac_y, low_index_y, ny);
}


/* Function to calculate the interpolated value on 3-dimension table */
static inline 
double table3(
  const double *tbl_val,  /*IN: (--) Pointer to 1D array for the 3D value table
                                     with Y-X-Z dimensional order allocated */
  const double frac_x,    /*IN: (--) Fractional distance on X dimension     */
  const int low_index_x,  /*IN: (--) Lower breakpoint index on X dimension  */
  const int nx,           /*IN: (--) Number of breackpoints on X dimension  */
  const double frac_y,    /*IN: (--) Fractional distance on Y dimension     */
  const int low_index_y,  /*IN: (--) Lower breakponit index on Y dimension  */
  const int ny,           /*IN: (--) Number of breakpoints on Y dimension   */
  const double frac_z,    /*IN: (--) Fractional distance on Z dimension     */
  const int low_index_z)  /*IN: (--) Lower breakpoint index on Z dimension  */
{
  assert(NULL!=tbl_val && "Pointer tbl_val cannot be NULL!");
  return cal_interp_3d(tbl_val, frac_x, low_index_x, nx, frac_y, low_index_y,
                       ny, frac_z, low_index_z);
}

/* Function to calculate the interpolated value on 4-dimension table */
static inline 
double table4(
  const double *tbl_val,  /*IN: (--) Poiner to 1D array for the 4D value table
                                     with Y-X-Z-W dimensional order allocated */
  const double frac_x,    /*IN: (--) Fractional distance on X dimension       */
  const int low_index_x,  /*IN: (--) Lower breakpoint index on X dimension    */
  const int nx,           /*IN: (--) Number of breakpoints on X dimension     */
  const double frac_y,    /*IN: (--) Fractional distance on Y dimension       */
  const int low_index_y,  /*IN: (--) Lower breakpoint index on Y dimension    */
  const int ny,           /*IN: (--) Number of breakpoints on Y dimension     */
  const double frac_z,    /*IN: (--) Fractional distance on Z dimension       */
  const int low_index_z,  /*IN: (--) Lower breakpoint index on Z dimension    */
  const int nz,           /*IN: (--) Number of breakpoints on Z dimension     */
  const double frac_w,    /*IN: (--) Fractional distance on W dimension       */
  const int low_index_w)  /*IN: (--) Lower breakpoint index on W dimension    */
{
  assert(NULL!=tbl_val && "Pointer tbl_val cannot be NULL!");
  return cal_interp_4d(tbl_val, frac_x, low_index_x, nx, frac_y, low_index_y,
                       ny, frac_z, low_index_z, nz, frac_w, low_index_w);
}

#endif /* TABLE_INTERP_IMPL_H */

