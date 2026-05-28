/*******************************************************************************
PURPOSE:
   (Declare the prototypes for table interpolation functions.)

PROGRAMMERS:
   (((Edgar Medina) (NASA) (April 2005) (ARES) (Initial implementation for ARES))
    ((Pat Galvin) (ESCG) (7/07) (RDLaa09781) (Add table1_angle function))
    ((Bingquan Wang) (OSR) (May, 2017) (ANTARES) (cleaned up the code for IV&V))
   )

*******************************************************************************/

#ifndef TABLE_INTERP_H
#define TABLE_INTERP_H


/* Function to calculate the lower breakpoint index and percent fractional
 * distance in interval for the given lookup value.
 * Return:  0 -- success
 *         -1 -- the input value is beyond the low end, and the output is
 *               set as the low end boundary.
 *         -2 -- the input value is beyond the high end, and the output is
 *               set as the high end boundary.
 */
static inline 
int scale(
  const double value,    /* (--) Lookup value                                 */
  double *frac,          /* (--) Fractional distance in interval to return    */
  int *low_index,        /* (--) Lower breakpoint index of interval to return */
  const double *tbl_bpt, /* (--) Pointer to the breakpoint table              */
  const int num_bps);    /* (--) Number of breakpoints                        */


/* Function to calculate the interpolated value on 1-dimension table. */
static inline 
double table1(     
  const double *tbl_val, /* (--) Pointer to 1-dimension value table    */
  const double frac,     /* (--) Fractional distance on X dimension    */
  const int low_index);  /* (--) Lower breakpoint index on X dimension */


/* Function to calculate the interpolated angle on 1-dimension table. */
static inline 
double  table1_angle(
  const double *tbl_angle, /* (--) Pointer to 1-dimension angle table    */
  const double frac,       /* (--) Fractional distance on X dimension    */
  const int low_index);    /* (--) Lower breakpoint index on X dimension */


/* Function to calculate the interpolated value on 2-dimension table */
static inline 
double table2(
  const double *tbl_val, /* (--) Pointer to 1D array for the 2D value table 
                                 with Y-dimension allocated first      */
  const double frac_x,   /* (--) Fractional distance on X dimension   */
  const int low_index_x, /* (--) Lower breakpoint index on X dimension */
  const double frac_y,   /* (--) Fractional distance on Y dimension   */
  const int low_index_y, /* (--) Lower breakpont index on Y dimension  */
  const int ny);         /* (--) Number of breakpoints on Y dimension  */


/* Function to calculate the interpolated value on 3-dimension table */
static inline 
double table3(
  const double *tbl_val,  /* (--) Pointer to 1D array for the 3D value table 
                                  with Y-X-Z dimensional order allocated */
  const double frac_x,    /* (--) Fractional distance on X dimension     */
  const int low_index_x,  /* (--) Lower breakpoint index on X dimension  */
  const int nx,           /* (--) Number of breackpoints on X dimension  */
  const double frac_y,    /* (--) Fractional distance on Y dimension     */
  const int low_index_y,  /* (--) Lower breakponit index on Y dimension  */
  const int ny,           /* (--) Number of breakpoints on Y dimension   */
  const double frac_z,    /* (--) Fractional distance on Z dimension     */
  const int low_index_z); /* (--) Lower breakpoint index on Z dimension  */


/* Function to calculate the interpolated value on 4-dimension table */
static inline 
double table4(
  const double *tbl_val,  /* (--) Poiner to 1D array for the 4D value table
                                  with Y-X-Z-W dimensional order allocated */
  const double frac_x,    /* (--) Fractional distance on X dimension       */
  const int low_index_x,  /* (--) Lower breakpoint index on X dimension    */
  const int nx,           /* (--) Number of breakpoints on X dimension     */
  const double frac_y,    /* (--) Fractional distance on Y dimension       */
  const int low_index_y,  /* (--) Lower breakpoint index on Y dimension    */
  const int ny,           /* (--) Number of breakpoints on Y dimension     */
  const double frac_z,    /* (--) Fractional distance on Z dimension       */
  const int low_index_z,  /* (--) Lower breakpoint index on Z dimension    */
  const int nz,           /* (--) Number of breakpoints on Z dimension     */
  const double frac_w,    /* (--) Fractional distance on W dimension       */
  const int low_index_w); /* (--) Lower breakpoint index on W dimension    */


#include "table_interp_impl.h"
#include "table_angle_impl.h"


#endif /* TABLE_INTERP_H*/

