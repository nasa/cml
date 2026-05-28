/********************************** TRICK HEADER **************************
PURPOSE:
    (Parachute Atmosphere data structure)

REFERENCE:
    (( -- ))

ASSUMPTIONS AND LIMITATIONS:
    (( -- ))

LIBRARY DEPENDENCY:
    (
     (parachute_state_atmos.o)
    )

PROGRAMMERS:
     (((Jeff Semrau) (HW) (Apr 2009) (Initial Version)))
     (((Jeff Semrau) (HW) (Jan 2011) (Updated to use planet data structure, not planet_fixed_posn data structure)))
**************************************************************************/

#ifndef PARACHUTE_PRE_ATMOS
#define PARACHUTE_PRE_ATMOS

#include "jeod/models/utils/planet_fixed/planet_fixed_posn/include/planet_fixed_posn.hh"
#include "jeod/models/environment/planet/include/planet.hh"


struct State_Atmos_Input    
{
   double       inertial_pos[3];             /* (m)    Earth centered inertial position vector */
   double       inertial_vel[3];             /* (m/s)  Earth centered inertial velocity vector */
   double       r_eq;                        /* (m)    Earth mean equatorial radius            */
   double       e_ellip_sq;                  /* (--)   Square of planet ellipsoid eccentricity */
   double       r_pol;                       /* (m)    Earth mean polar radius                 */
   double       pc_ang_vel0;                 /* (rad/s)  Earth angular velocity, X component     */
   double       pc_ang_vel1;                 /* (rad/s)  Earth angular velocity, Y component     */
   double       pc_ang_vel2;                 /* (rad/s)  Earth angular velocity, Z component     */
   double       eci2ecef[3][3];              /* (--)   Earth Centered Inertial to Earth Centered/Earth Fixed Transformation */

};

struct State_Atmos_Output    
{
   //Translational 
   double       ecef_pos[3];                 /* (m)    ECEF position vector                    */
   double       geocen_alt;                  /* (m)    Geocentric altitude                     */
   double       geocen_alt_gram;             /* (m)    Geocentric altitude GRAM                */
   double       geocen_lat;                  /* (rad)  Geocentric latitude                     */
   double       geocen_lon;                  /* (rad)  Geocentric longitude                    */
   double       geod_alt;                    /* (m)    Geodetic altitude                       */
   double       geod_lat;                    /* (rad)  Geodetic latitude                       */
   double       geod_lon;                    /* (rad)  Geodetic longitude                      */
   double       Vmag_earthrel;               /* (m/s)  Magnitude of earth relative velocity    */
 
};

struct State_Atmos_Work   
{

   double       inertial_lat;                /* (rad)  Latitude measured wrt ECI frame                         */
   double       inertial_lon;                /* (rad)  Longitude measured wrt ECI frame                        */
   double       VLocalIDueToEarthOmega[3];   /* (m/s)  Velocity vector of local frame due to earth rotation    */
   double       VOmega;                      /* (m/s)  Velocity magnitude of local frame due to earth rotation */
   double       VRelECEFinInertial[3];       /* (m/s)  Earth relative velocity vector in ECI frame             */
   double       ecef_vel[3];                 /* (m/s)  ECEF velocity vector                                    */
   double       Verl[3];                     /* (m/s)  Earth relative velocity vector in local frame (no wind) */

};
   
struct STATE_DATA {
   State_Atmos_Input   input;
   State_Atmos_Output  output;
   State_Atmos_Work     work;
}; 

int parachute_state_atmos (STATE_DATA  &S);

#endif
