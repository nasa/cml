/********************************** TRICK HEADER **************************
PURPOSE:
    (Parachute Equations of Motion data structure)

REFERENCE:
    (( -- ))

ASSUMPTIONS AND LIMITATIONS:
    (( -- ))

PROGRAMMERS:
    (
    ((Jeff Semrau) (HW) (24-Mar-2009) (--) (Initial Version))
    ((Gesting) (OSR) (05/2010) (791) (Update OIMU to Honeywell updates))
    )
**************************************************************************/

#ifndef PARACHUTE_EOM_H
#define PARACHUTE_EOM_H

#include <stdbool.h>

typedef struct EOM_Inputs   

{
   bool         rotational_eom_enabled; /* (--)   Flag to determine whether or not to compute rotational EOMs  */ 
   double       aero_force_SR[3];       /* (N)    Chute aero force in SR frame                  */
   double       aero_moment_SR[3];      /* (N*m)  Chute aero moment in SR frame                 */
   double       riser_force_SR[3];      /* (N)    Chute aero force in SR frame                  */
   double       riser_moment_SR[3];     /* (N*m)  Chute aero moment in SR frame                 */
   double       mortar_force_SR[3];     /* (N)    Mortar Force on chute in SR frame             */
   double       pilot_force_SR[3];      /* (N)    Pilot Force on chute in SR frame              */
   double       pilot_moment_SR[3];     /* (N*m)  Pilot Moment on chute in SR frame             */
   double       apparent_mass_accel[3]; /* (m/s2) Acceleration due to apparent mass             */ 
   double       gravaccel[3];           /* (m/s2) Gravitational acceleration                    */
   double       inertia[3][3];          /* (kg*m2) Body inertia matrix                          */
   double       mass;                   /* (kg)   body mass                                     */
   double       chute_mass[3];          /* (kg)   Chute mass vector                             */
   double       chute_dry_mass;         /* (kg)   Chute dry mass                                */
   bool         modeling_chute;         /* (--)   Flag to tell EOM if modeling chute or confluence pt */
   double       T_SR_to_body[3][3];     /* (--)   SR to Body Transformation matrix              */

} EOM_Inputs; 

typedef struct EOM_Outputs   
{
   //Translational 
   double       inertial_pos[3];             /* (m)    Inertial position vector of chute        */
   double       inertial_vel[3];             /* (m/s)  Inertial velocity vector of chute        */
 
   //Rotational 
   double       body_rates[3];               /* (rad/s)  Rotational rates relative to inertial frame, expressed in body frame */
   double       T_inertial_to_body[3][3];    /* (--)   Transformation matrix for inertial to body frames                    */
   double       T_body_to_inertial[3][3];    /* (--)   Transformation matrix for body to inertial frames                    */
   double       T_struct_to_inertial[3][3];  /* (--)   Transformation matrix for structural to inertial frames              */
   
} EOM_Outputs;

typedef struct EOM_Work    
{
   /* Translational */
   double          trans_accel[3];            /* (m/s2)    Translational acceleration vector of chute  */
   double          total_accel[3];            /* (m/s2)    Total acceleration vector of chutes         */
   double          body_accel[3];             /* (m/s2)    Acceleration vector of chute in body axis   */
   double          inertial_accel[3];         /* (m/s2)    Acceleration vector of chute in earth centered inertial frame */
   double          gravaccel_mass[3];         /* (--)      Inertial Gravity times chute mass                             */
   double          gravaccelmass_SR[3];       /* (--)      Gravity times mass in the chute SR frame                      */
   double          gravSR[3];                 /* (m/s2)    Gravity Accel Vector on chute in chute SR frame               */
   double          gravaccel[3];              /* (m/s2)    Gravity Accel Vector on chute in inertial frame               */

   /* Rotational */  
   double          body_torque[3];                 /* (N*m)     torque in body frame                 */
   double          SR_torque[3];                   /* (N*m)     torque in SR frame                   */
   double          ang_mom_body[3];                /* (kg*m2/s)  Angular momentum                    */      
   double          inertial_torque[3];             /* (N*m)     Inertial torque                      */
   double          inverse_inertia[3][3];          /* (1/kg*m2)  Inverse chute inertia matrix        */
   double          rot_accel[3];                   /* (rad/s2)    Rotational acceleration vector of chute in body frame */
   double          quat_temp[3];                   /* (--)      Temporary quaternion                  */
   double          quat_inertial_to_body[4];       /* (--)      Quaternion for inertial to body       */
   double          temp_quat_inertial_to_body[4];  /* (--)      Temp Quaternion for inertial to body  */
   double          quat_deriv[4];                  /* (--)      Quaternion derviative                 */

} EOM_Work;
   

typedef struct {
   EOM_Inputs     input;
   EOM_Outputs    output;
   EOM_Work       work;
} EOM_DATA; 

#ifdef __cplusplus
extern "C" {
#endif
void parachute_eom (EOM_DATA    *E); 

#ifdef __cplusplus
}
#endif

#endif
