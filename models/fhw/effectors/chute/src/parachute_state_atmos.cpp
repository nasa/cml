/******************************* TRICK HEADER **********************************
PURPOSE:
    ((This function provides for the computation of chute geocentric parameters. ))
 
REFERENCE:
    ((  None ))
 
ASSUMPTIONS AND LIMITATIONS:
    (( None ))
 
LIBRARY DEPENDENCY:
     (parachute_state_atmos.o)
     (orbutil_geocentric_ellipsoid_alt.o)
     (orbutil_xyz_to_geodetic_e2.o)
     (orbutil_xyz_to_geocentric.o)

 PROGRAMMERS:
    (((Jeff Semrau) (HW) (Mar 2009) (Initial Version)))
    ((Jeff Semrau) (HW) (Oct 2012) (lmbp2122) (Corrected frame error, pfix rotation in in ECEF, not Inertial))
    )
*******************************************************************************/

#include <cmath> // sin cos sqrt

#include "../include/parachute_pre_atmos.hh"
#include "../include/orbitalutil_2.h"

#include "trick/trick_math.h"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

int parachute_state_atmos(
        STATE_DATA&   S)                     /* INOUT:  --    Chute Geocentric Data Structure */

{ 

   double posmag, posmag_xy;
   double omegaEinI[3] ;
   double omegaE[3];
   double ned_from_eci[3][3] ;
   double slai, clai, smui, cmui, slasmu, slacmu, clacmu, clasmu ;

   /* Compute Earth-Fixed position */
   MxV(S.output.ecef_pos, S.input.eci2ecef, S.input.inertial_pos);

   /* Calculate geodetic altitude, latitude, longitude */
   xyz_to_geodetic_e2( S.input.r_eq, S.input.e_ellip_sq, S.output.ecef_pos, &S.output.geod_alt, &S.output.geod_lat, &S.output.geod_lon);

   /* Calculate geocentric altitude, latitude, longitude */
   xyz_to_geocentric( S.output.ecef_pos, &S.output.geocen_alt, &S.output.geocen_lat, &S.output.geocen_lon );

   posmag = V_MAG(S.input.inertial_pos) ;
   S.output.geocen_alt = posmag;

   /* Calculate true altitude above earth using geocentric coordinates */
   posmag_xy = std::sqrt(S.input.inertial_pos[0]*S.input.inertial_pos[0] + S.input.inertial_pos[1]*S.input.inertial_pos[1] );
   geocentric_ellipsoid_alt( S.input.r_eq, S.input.r_pol, &S.output.geocen_alt, &S.output.geocen_lat, &S.output.geocen_alt_gram);
   S.work.inertial_lat = std::atan2(S.input.inertial_pos[2] , posmag_xy) ;
   S.work.inertial_lon = std::atan2(S.input.inertial_pos[1] , S.input.inertial_pos[0]) ;

   /* Compute State stuff */
   slai = std::sin(S.work.inertial_lat) ;
   clai = std::cos(S.work.inertial_lat) ;
   smui = std::sin(S.work.inertial_lon) ;
   cmui = std::cos(S.work.inertial_lon) ;
   slasmu = slai * smui ;
   slacmu = slai * cmui ;
   clacmu = clai * cmui ;
   clasmu = clai * smui ;

   /* Compute the Inertial to NED LOCAL frame */
   ned_from_eci[0][0] = -slacmu;
   ned_from_eci[0][1] = -slasmu;
   ned_from_eci[0][2] =  clai;

   ned_from_eci[1][0] = -smui;
   ned_from_eci[1][1] =  cmui;
   ned_from_eci[1][2] =  0.0;

   ned_from_eci[2][0] = -clacmu;
   ned_from_eci[2][1] = -clasmu;
   ned_from_eci[2][2] = -slai;

   /* Compute the inertial velocity of the LOCAL frame */
   omegaE[0] = S.input.pc_ang_vel0 ;
   omegaE[1] = S.input.pc_ang_vel1 ;
   omegaE[2] = S.input.pc_ang_vel2 ;
   MtxV(omegaEinI, S.input.eci2ecef, omegaE) ;

   V_CROSS( S.work.VLocalIDueToEarthOmega, omegaEinI, S.input.inertial_pos ) ;
   S.work.VOmega = V_MAG(S.work.VLocalIDueToEarthOmega) ;

   V_SUB(S.work.VRelECEFinInertial, S.input.inertial_vel, S.work.VLocalIDueToEarthOmega) ;
   MxV(S.work.ecef_vel, S.input.eci2ecef, S.work.VRelECEFinInertial) ;

   /* Compute Earth relative velocity expressed in the NED Local frame */
   MxV(S.work.Verl,ned_from_eci,S.work.VRelECEFinInertial);
   S.output.Vmag_earthrel = V_MAG(S.work.Verl);

   return(0);

}
