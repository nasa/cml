/* Trick Header

PURPOSE:

     (This header file contains all include files, definitions
     and structure definitions needed internally only by the
     mortar model used in the HiFi Chute Model.)

PROGRAMMERS:

    (((Jeff Semrau) (Honeywell) (03/2009) ))
    (((Jeff Semrau) (Honeywell) (08/2013) (LMBP 2720) (Added vars to correct error in computing mortar velocity Y direction)))

*/

#include <stdbool.h>

#ifndef _MORTAR_H_
#define _MORTAR_H_

typedef struct Mortar_Inputs {
   double     CG_position[3];                             /* (m)  CG of CM                   */
   double     sim_time_local;                             /* (s)  Elasped sim time           */
   int        mortar_fired;                               /* (--) Mortar Fired flag          */
} Mortar_Inputs;

typedef struct Mortar_Outputs {
   double      mortar_forces[3];                          /* (N)   Force vector from mortar                   */
   double      mortar_moments[3];                         /* (N*m) Moment vector from mortar                  */
   double      total_impulse;                             /* (N*s) Total impulse from mortar                  */
   bool        firing_complete;                           /* (--)  Flag indicating mortar impulse is complete */
} Mortar_Outputs;

typedef struct Mortar_Params {
   double      mortar_position_LRS_frame[3];              /* (m)   Position of mortar in LRS frame         */
   double      mortar_elevation;                          /* (degree)   Elevation angle of mortar               */
   double      mortar_azimuth;                            /* (degree)   Azimuth (aka toe) angle of mortar       */
   double      force_uncertainty;                         /* (--)  Uncertainty factor for mortar force     */
   double      impulse;                                   /* (N*s) Mortar Impulse                          */
   double      delay_time;                                /* (s)   Mortar Delay time                       */
   bool        delay_mortar;                              /* (--)  Mortar Delay flag                       */

   double      mortar_vel;                                /* (m/s) Mortar Exit Velocity                    */
} Mortar_Params;

typedef struct Mortar_Work {
   double      mortar_position_SR_frame[3];               /* (m)   Position of mortar in SR frame         */
   double      mortar_position_SR_frame_temp[3];          /* (m)   Temp position of mortar in SR frame    */
   double      mortar_moment_arms[3];                     /* (m)   Mortar moment arms, from mortar to CG  */
   double      mortar_direction_LRS[3];                   /* (--)  Direction of mortar force in LRS frame */
   double      mortar_direction_SR[3];                    /* (--)  Direction of mortar force in SR frame  */
   double      start_time;                                /* (s)   Start time of mortar recoil model      */
   double      elasped_time;                              /* (s)   Elasped time mortar recoil model       */
   double      force;                                     /* (N)   Current mortar force                   */
   double      prev_total_impulse;                        /* (N*s) Previous total impulse value           */

   bool        mortar_deployed;                           /* (--)  Mortar Fired flag          */
   bool        impulse_finished;                          /* (--)  Mortar impulse complete flag           */
   bool        first_pass;                                /* (--)  Mortar recoil model first pass flag    */
   bool        start_impulse;                             /* (--)  Mortar impulse start flag              */

   double mortar_vel_pyld_SR[3];                          /* (m/s) Exit Velocity of Mortar in Payload SR Frame */
   double mortar_vel_chute_SR[3];                         /* (m/s) Exit Velocity of Mortar in Chute SR Frame */
   double mortar_vel_inertial[3];                         /* (m/s) Exit Velocity of Mortar in Inertial Frame */
   bool   vel_first_pass;                                 /* (--)  First Pass Flag for Exit Velocity Computation */
} Mortar_Work;

typedef struct {
   Mortar_Inputs     input;                                 /* (--) */
   Mortar_Outputs    output;                                /* (--) */
   Mortar_Params     param;                                 /* (--) */
   Mortar_Work       work;                                  /* (--) */
}  MORTAR_DATA;

#ifdef __cplusplus
extern "C" {
#endif
void mortar_recoil (double          LRS_origin_in_SR_Frame[3],
                    double          LRS_to_SR_rotation[3][3],
                    MORTAR_DATA    *chute_mortar);

#ifdef __cplusplus
}
#endif
#endif


