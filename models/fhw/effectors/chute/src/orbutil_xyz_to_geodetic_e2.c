/*
PURPOSE:
   (Conversion utility to convert from planet fixed position to
    geodetic altitude, latitude, and longitude.)

REFERENCE:
   (((LaFargue, Michelle, M., etal.)
     (dyn_v1.3/models/environment/atmos/jacchia/src/gnj_ef_to_geo.c)
     (Trick Orbital Mechanics Source Code) (1992)))

ASSUMPTIONS AND LIMITATIONS:
   ((1. Central Body is perfect ellipsoid)
    (2. Planetary constants defined outside))

PROGRAMMERS:
   (((Edwin Z. Crues) (NASA) (Mar 2005) (--) (Initial version))
    ((Scott Piggott) (OSR) (Jun 2006) (--) (Use atan2 instead of atan))
    ((Scott Piggott) (OSR) (July 2006) (Fixed assumptions and lim.)))
*/

#include <math.h>
#include <stdio.h>

/* Model includes. */
#include "trick/vector_macros.h"
#include "../include/orbitalutil_2.h"

void xyz_to_geodetic_e2 ( /* RETURN: -- None.                                */
   double   r_eq,        /* IN:  M  Planetary mean equatorial radius.        */
   double   e2,          /* IN:  -- Planetary elliptic eccentricity squared. */
   double   pfix_xyz[3], /* IN:  M  Planet fixed position.                   */
   double * altitude,    /* OUT: M  Geodetic altitude.                       */
   double * latitude,    /* OUT: r  Geodetic latitude.                       */
   double * longitude  ) /* OUT: r  Longitude.                               */
{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in orbutil_xyz_to_geodetic_e2.c \n";
    if (!pfix_xyz || !altitude || !latitude || !longitude) {
       printf(error_msg);
       return;
    }

   double x;  /* X position in the plane of the ellipse.     */
   double x2; /* Square of the X position.                   */
   double z;  /* Z position in the plane of the ellipse.     */
   double z2; /* Square of the Z position.                   */
   double r;  /* Radial magnitude of the position.           */
   double r2; /* Square of radial magnitude of the position. */

   double gc_latitude;  /* Geocentric latitude. */

   double radius_p;
   double gnj_del;
   double del_lat;

   /* Compute the X position in plane of ellipse. */
   x2 = (pfix_xyz[0] * pfix_xyz[0]) + (pfix_xyz[1] * pfix_xyz[1]);
   x  = sqrt( x2 );

   /* Compute (actually copy) the Z position in plane of ellipse. */
   z2 = (pfix_xyz[2] * pfix_xyz[2]);
   z  = pfix_xyz[2];

   /* Compute the planet centered radial magnitude of the position. */
   r2 = (x2 + z2);

   /* Exit if position magnitude is zero */
   if (r2 < 1E-16) {
       *altitude  = -r_eq;
       *latitude  = 0.0;
       *longitude = 0.0;
       return;
   }

   r  = sqrt( r2 );

   radius_p = r_eq / sqrt(1.0 + (e2 * (z2 / r2)) / (1.0 - e2));

   gnj_del = (e2 * ((z * x) / r2)) / (1.0 - (e2 * (x2 / r2)));

   del_lat = (radius_p * gnj_del) / r;

   /* Compute the geocentric latitude. */
   gc_latitude = atan2( z, x );

   /* Compute the geodetic latitude. */
   *latitude = gc_latitude + del_lat;

   /* Compute the geodetic altitude. */
   *altitude = (r - radius_p) * (1.0 - (0.50 * gnj_del * del_lat));

   /* Compute the longitude from the X and Y components. */
   *longitude = atan2( pfix_xyz[1], pfix_xyz[0] );

}

