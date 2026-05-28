/******************************** TRICK HEADER ********************************
PURPOSE:
    (Compute the planet-fixed to topodetic transformation matrix)
REFERENCE:
    ((FOR MORE INFORMATION SEE PAGES 4-219 AND 5-132 OF THE 
       STS81-0004A NAV FSSR (RELEASE 19)))
ASSUMPTIONS AND LIMITATIONS:
    ((None))
PROGRAMMERS:
    (((Scott Piggott) (OSR) (05/07) (LM Orion) (Initial Implementation))
     ((Gesting) (OSR) (08/07) (CR 122) (set as own function))
     ((R. BORLAND) (ESCG) (04/13) (Added in math.h include and function prototype))
     ((G Turner) (OSR) (03/16) (Swapped prototype out for #include.)))
******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "../include/orbitalutil_2.h"

/*****************************************************************************
                    FUNCTION DEFINITION
******************************************************************************/
int pfix_to_topo(                      /* RETURN:  --  Always returns zero.     */
   double geocen_lat,                  /* IN:      --  geodetic latittude       */
   double geocen_lon,                  /* IN:      --  longitude of point       */
   double T_ECEF_topd[3][3])           /* OUT:     --  trans from pfix to topo  */
{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in orbutil_pfix_to_topo.c \n";
    if (!T_ECEF_topd) {
       printf(error_msg);
       return(0);
    }

   T_ECEF_topd[1][1] =  cos(geocen_lon);
   T_ECEF_topd[0][2] =  cos(geocen_lat);
   T_ECEF_topd[1][0] = -sin(geocen_lon);
   T_ECEF_topd[2][2] = -sin(geocen_lat);
   T_ECEF_topd[0][0] =  T_ECEF_topd[2][2]*T_ECEF_topd[1][1];
   T_ECEF_topd[2][0] = -T_ECEF_topd[0][2]*T_ECEF_topd[1][1];
   T_ECEF_topd[0][1] = -T_ECEF_topd[2][2]*T_ECEF_topd[1][0];
   T_ECEF_topd[2][1] =  T_ECEF_topd[0][2]*T_ECEF_topd[1][0];
   T_ECEF_topd[1][2] =  0.0;
   
   return(0);

}
