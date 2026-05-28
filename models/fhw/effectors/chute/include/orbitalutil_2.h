#ifndef CHUTES_ORBITALUTIL_2_H_ 
#define CHUTES_ORBITALUTIL_2_H_

#ifdef __cplusplus
extern "C" {
#endif

int pfix_to_topo(
    double geocen_lat,
    double geocen_lon,
    double T_ECEF_topd[3][3]);

int geocentric_ellipsoid_alt(  /* RETURN: -- None.                    */
   double   r_eq,          /* (m)  Planetary mean equitorial radius.           */
   double   r_pol,         /* (m)  Mean planetary polar radius.                */
   double * altitude,      /* (m)  Geocentric altitude above geocentric sphere.*/
   double * latitude,      /* (rad)  Geocentric latitude above geocentric sphere.*/
   double * altitude_gram);/* (m)  Geocentric altitude above geodetic ellipse. */

void xyz_to_geocentric (
   double   pfix_xyz[3], /* (m)  Cartesian planet fixed position.    */
   double * r,           /* (m)  Radial position from planet center. */
   double * latitude,    /* (rad)  Geocentric latitude.                */
   double * longitude ); /* (rad)  Longitude.                          */

void xyz_to_geodetic_e2 (
   double   r_eq,        /* (m)  Planetary mean equatorial radius.        */
   double   e2,          /* (-)- Planetary elliptic eccentricity squared. */
   double   pfix_xyz[3], /* (m)  Planet fixed position.                   */
   double * altitude,    /* (m)  Geodetic altitude.                       */
   double * latitude,    /* (rad)  Geodetic latitude.                       */
   double * longitude  );/* (rad)  Longitude.                               */

#ifdef __cplusplus
}
#endif

#endif
