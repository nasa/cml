/******************************************************************************
PURPOSE:
    (Interpolation functions for 1D and 2D tables)

PROGRAMMERS:
    (
     ((Bingquan Wang) (OSR) (May 2017) (ANTARES) (refaoctored from the original
                            C files: table_lookup_1d.c and  table_lookup_2d.c by 
                            several developers.))
    )
*******************************************************************************/

#ifndef OSIRIS_MATH_IMPL_H
#define OSIRIS_MATH_IMPL_H

#include <stddef.h>
#include <assert.h>
#include "table_interp_helper.h"

/* Function to do one-dimension interpolation */
static inline 
double table_lookup_1d(
  double input,         /*IN: (--) Lookup value                   */
  const int nbp,        /*IN: (--) Number of breakpoints          */
  const double bp[],    /*IN: (--) Array for the breakpoint table */
  const double data[])  /*IN: (--) Array for the 1D value table   */
{
  assert(NULL!=bp && "Pointer bp cannot be NULL!");
  assert(NULL!=data && "Pointer data cannot be NULL!");

  /*limit input*/
  if (bp[0] < bp[nbp-1]) {
    if (bp[0] > input) input = bp[0];
    if (bp[nbp-1] < input) input = bp[nbp-1];
  }
  else {
    if (bp[0] < input) input = bp[0];
    if (bp[nbp-1] > input) input = bp[nbp-1];
  }

  /*calculate the interpolation*/ 
  double frac = 0.0;
  int index = 0.0;
  cal_scale(input, &frac, &index, bp, nbp, bp[0]<bp[nbp-1]);

  return cal_interp_1d(data, frac, index);
}

/* Function to do two-dimension interpolation */
static inline 
double table_lookup_2d(
  double input1,       /* (--) Lookup value on dimension 1              */
  const int nbp1,      /* (--) Number of breakpoints on dimension 1     */
  const double bp1[],  /* (--) Array for breakpoints on dimension 1     */
  double input2,       /* (--) Lookup value on dimension 2              */
  const int nbp2,      /* (--) Number of breakpoints on dimension 2     */
  const double bp2[],  /* (--) Array for breakpoints on dimension 2     */
  const double data[]) /* (--) Array for 2D value table with dimension 1
                               allocated first                          */
{
  assert(NULL!=bp1 && "Pointer bp1 cannot be NULL!");
  assert(NULL!=bp2 && "Pointer bp2 cannot be NULL!");
  assert(NULL!=data && "Pointer data cannot be NULL!");

  /*limit inputs*/
  if (bp1[0] < bp1[nbp1-1]) {
    if (bp1[0] > input1) input1 = bp1[0];
    if (bp1[nbp1-1] < input1) input1 = bp1[nbp1-1];
  }
  else {
    if (bp1[0] < input1) input1 = bp1[0];
    if (bp1[nbp1-1] > input1) input1 = bp1[nbp1-1]; 
  }

  if (bp2[0] < bp2[nbp2-1]) {
    if (bp2[0] > input2) input2 = bp2[0];
    if (bp2[nbp2-1] < input2) input2 = bp2[nbp2-1];
  }
  else {
    if (bp2[0] < input2) input2 = bp2[0];
    if (bp2[nbp2-1] > input2) input2 = bp2[nbp2-1]; 
  }

  /*calculate the interpolation*/
  double frac1=0.0, frac2=0.0;
  int index1=0, index2=0;
 
  cal_scale(input1, &frac1, &index1, bp1, nbp1, bp1[0]<bp1[nbp1-1]);
  cal_scale(input2, &frac2, &index2, bp2, nbp2, bp2[0]<bp2[nbp2-1]);  

  return cal_interp_2d(data, frac2, index2, frac1, index1, nbp1); 
}

#endif /*OSIRIS_MATH_IMPL_H*/

