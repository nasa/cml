/***************************************************************************
PURPOSE:
   ( Performs a 1-d table lookup on input wind vector and ensures delta
     angle of < 180 deg is used. The algorithm should work for all supplied
     angles.)

ASSUMPTIONS AND LIMITATIONS:
   ((The fractions and low indices are precalculated by SCALE)
    (The output angle in "value" is between 0 and 2*PI.)
    (No limitations constrain the input angles, but they must
     be in the unit of radians.))

PROGRAMMERS:
   (((Pat Galvin) (ESCG) (July 2007) (RDLaa09781) (Initial implementation))
    ((Jon Berndt) (ESCG) (July 2008) (RDLaa10628) (Algorithm refinement))
    ((Bingquan Wang) (OSR) (May 2017) (ANTARES) (Refactoring include
                                  - cleaned up the code for IV&V,
                                  - cleaned the tedious algorithm,
                                  - replaced "long double" to "double".))
    ((Daniel Ghan) (OSR) (November 2019) (Antares) (Eliminated unnecessary
                                                    trigonometric operations))
   )

***************************************************************************/

#ifndef TABLE_ANGLE_IMPL_H
#define TABEL_ANGLE_IMPL_H

#include <assert.h>
#include <math.h>
#include <cstdlib> // NULL

/* Function to calculate the interpolated angle on 1-dimension table. */
static inline
double table1_angle(
  const double *tbl_angle, /*IN: (--) Pointer to 1-dimension angle table    */
  const double frac,       /*IN: (--) Fractional distance on X dimension    */
  const int low_index)     /*IN: (--) Lower breakpoint index on X dimension */
{
  static const double full_circle = 2.0*M_PI;

  assert(NULL!=tbl_angle && "Pointer tbl_angle cannot be NULL!");

  double diff = tbl_angle[low_index + 1] - tbl_angle[low_index];
  // Make the difference go by the shortest route between the two tabulated
  // values -- the angle transition from 160 degrees to -160 degrees is
  // 40 degrees (160-to-200), not -320 degrees.
  while (diff > M_PI) diff -= full_circle;
  while (diff < -M_PI) diff += full_circle;

  double result = tbl_angle[low_index] + frac*diff;

  while (full_circle <= result) result -= full_circle;
  while (0.0 > result) result += full_circle;

  return result;
}

#endif /* TABLE_ANGLE_IMPL_H */
