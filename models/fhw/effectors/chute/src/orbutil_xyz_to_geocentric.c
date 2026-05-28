/*
PURPOSE:
   (Conversion utility to convert from planet fixed position to geocentric
    altitude, latitude, and longitude.)

REFERENCE:
   (((Bate, Roger R., Mueller, Donald D., and White, Jerry E.)
        (Fundamental of Astrodynamics) (Dover Publications, Inc.: New York)
        (1971) (Pages 97-98) (ISBN:0-486-60061-0))
    ((Vallado, David A.) (Fundamentals of Astrodynamics and Applications)
        (McGraw-Hill) (1997) (Pages 202-205) (ISBN:0-07-066834-5)))

ASSUMPTIONS AND LIMITATIONS:
   ((None))

PROGRAMMERS:
   (((Edwin Z. Crues) (NASA) (Mar 2005) (--) (Initial version))
    ((Scott Piggott) (OSR) (July 2006) (Fixed assumptions and lim.)))
*/

#include <math.h>
#include "trick/vector_macros.h"

#include "../include/orbitalutil_2.h"


void xyz_to_geocentric ( /* RETURN: -- None.                            */
   double   pfix_xyz[3], /* IN:  M  Cartesian planet fixed position.    */
   double * r,           /* OUT: M  Radial position from planet center. */
   double * latitude,    /* OUT: r  Geocentric latitude.                */
   double * longitude )  /* OUT: r  Longitude.                          */
{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in orbutil_xyz_to_geocentric.c \n";
    if (!pfix_xyz || !r || !latitude || !longitude) {
       printf(error_msg);
       return;
    }

    /* Compute the radial distance from planet center. */
    *r = V_MAG( pfix_xyz );

    if (*r > 0.0) {

       /* Compute the latitude from the Z position. */
       *latitude = asin( pfix_xyz[2] / *r );

       /* Compute the longitude from X and Y positions. */
       *longitude = atan2( pfix_xyz[1], pfix_xyz[0] );
    }
    else {
       *latitude  = 0.0;
       *longitude = 0.0;
    }

}

