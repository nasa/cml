/************************************************************************************
PURPOSE:
    (Function prototypes of 1D & 2D table interpolation)

PROGRAMMERS:
    (((Andy Barth)   (LMMS) (06-11-07) (Introduce table lookup functions from SES))
     ((Bingquan Wang) (OSR) (May, 2017) (cleaned up the code for IV&V))
    )

*************************************************************************************/

#ifndef OSIRIS_MATH_H
#define OSIRIS_MATH_H

/* Function to do one-dimension interpolation */
static inline 
double table_lookup_1d(
  double input,         /* (--) Lookup value                   */ 
  const int nbp,        /* (--) Number of breakpoints          */
  const double bp[],    /* (--) Array for the breakpoint table */
  const double data[]); /* (--) Array for the 1D value table   */

/* Function to do two-dimension interpolation */
static inline 
double table_lookup_2d(
  double input1,        /* (--) Lookup value on dimension 1             */ 
  const int nbp1,       /* (--) Number of breakpoints on dimension 1    */
  const double bp1[],   /* (--) Array for breakpoints on dimension 1    */
  double input2,        /* (--) Lookup value on dimension 2             */
  const int nbp2,       /* (--) Number of breakpoints on dimension 2    */
  const double bp2[],   /* (--) Array for breakpoints on dimension 2    */
  const double data[]); /* (--) Array for 2D value table with dimension 1
                                allocated first                         */


#include "osiris_math_impl.h"

#endif /* OSIRIS_MATH_H */

