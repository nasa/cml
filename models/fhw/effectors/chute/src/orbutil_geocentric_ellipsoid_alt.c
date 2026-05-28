/*
PURPOSE:
   (Conversion utility to convert from geocentric altiltude from the
    center of the earth to geocentric altitude above the geodetic ellipse.
    Also computes geocentric altitude above a geocentric earth.)

REFERENCE:
   (subroutine rig_E07 in GRAM2007)
   
ASSUMPTIONS AND LIMITATIONS:
   ((1. Central Body is perfect ellipsoid)
    (2. Planetary constants defined outside))

PROGRAMMERS:
    (
    ((Matt Posey) (LMMS) (03/08) (--) (Initial Version))
    ((Matt Posey) (LMES) (03/2010) (743) (GRAM altitude inputs))
    )
*/

#include <math.h>
#include <stdio.h>

#include "../include/orbitalutil_2.h"

int geocentric_ellipsoid_alt(  /* RETURN: -- None.                    */
   double   r_eq,              /* IN:    M  Planetary mean equitorial radius.        */
   double   r_pol,             /* IN:    M  Mean planetary polar radius.       */
   double * altitude,          /* IN:    M  Geocentric altitude above geocentric sphere.     */
   double * latitude,          /* IN:    r  Geocentric latitude above geocentric sphere.     */
   double * altitude_gram)     /* OUT:    M  Geocentric altitude above geodetic ellipse.      */
{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in orbutil_geocentric_ellipsoid_alt.c \n";
    if (!altitude || !latitude || !altitude_gram) {
       printf(error_msg);
       return(0);
    }

   double r0;              /* geodetic radius of earth */
   double eps;             /* eccentricity */
   double cos_lat;         /* Cosine of the geocentric latitude. */
   
   eps = 1 - ((r_pol/r_eq) * (r_pol/r_eq));
   cos_lat = cos( *latitude );
   
   /* Compute geodetic earth radius at this latitude */
   r0 = r_pol / sqrt(1 - eps * cos_lat * cos_lat);

   /* Solve for both altitudes by subtracting the appropriate radius from the altitude. */       
   *altitude_gram = *altitude - r0;
   *altitude = *altitude - r0;

   return(0);

}

