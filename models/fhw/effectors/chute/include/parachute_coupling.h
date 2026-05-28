/********************************** TRICK HEADER **************************
PURPOSE:
    (Parachute coupling data structure)

REFERENCE:
    (( -- ))

ASSUMPTIONS AND LIMITATIONS:
    (( -- ))

PROGRAMMERS:
     (((Richard Burt) (LM) (Apr 2009) (Initial Version)))
     (((Jeff Semrau)  (HW) (Nov 2013) (New declaraions for inelastic load, LMBP #3148)))
**************************************************************************/

#include <stdbool.h>

#ifndef PARACHUTE_COUPLING_H_
#define PARACHUTE_COUPLING_H_

typedef struct COUPLING_INPUTS { 
   double     nominal_length;                 /* (m)   Nominal unstretched length of the riser line      */
   double     endpoint1_T_ECI_to_Body[3][3];  /* (--)  Endpoint 1 transformation from ECI to body        */
   double     endpoint2_T_ECI_to_Body[3][3];  /* (--)  Endpoint 2 transformation from ECI to body        */
   double     endpoint1_T_Body_to_SR[3][3];   /* (--)  Endpoint 1 transformation from body to structural */
   double     endpoint2_T_Body_to_SR[3][3];   /* (--)  Endpoint 2 transformation from body to structural */
   double     endpoint1_vec_wrt_cg[3];        /* (m)   Endpoint 1 vector to body cg                      */
   double     endpoint2_vec_wrt_cg[3];        /* (m)   Endpoint 2 vector to body cg                      */
   double     relstate_riser_distance;        /* (m)   Stretched length of riser line                    */
   double     relstate_riser_velocity;        /* (m/s) Relative velocity of riser ends                   */
   double     relstate_force_uvec[3];         /* (--)  Unit vector of riser load in ECI frame            */
   double     current_diameter;               /* (m)   Current Diameter of chute                         */
   double     nominal_diameter;               /* (m)   Nominal Diameter of chute                         */
   bool       use_inelastic_load;             /* (--)  Flag whether to apply inelastic load              */
   bool       use_mass_rate_load;             /* (--)  Flag whether to apply inelastic load              */

   double     canopy_mass;                    /* (kg)    mass of chute canopy                            */
   double     drag_mass;                      /* (kg)    chute mass                                      */
   double     riser_distance;                 /* (m)     Riser line distance                             */
   double     suspension_distance;            /* (m)     Suspension line distance                        */
   double     canopy_distance;                /* (m)     Canopy radius                                   */
   double     deploy_distance;                /* (m)     Deploy distance                                 */
   double     velocity;                       /* (m/s)   Velocity of chute relative to payload           */
   double     qbar;                           /* (N/m2)  Dynamic pressure of chute                       */
   double     VairB[3];                       /* (m/s)   Payload Velocity Vector                         */
   double     pyld_T_inertial_to_body[3][3];  /* (--)    Payload Transformation Matrix                   */

} COUPLING_INPUTS;

typedef struct COUPLING_OUTPUTS {
   double      deflection;                    /* (m)  Riser deflection                                    */
   double      load;                          /* (N)  Riser load                                          */
   double      max_riser_load;                /* (N)  Maximum riser load for simulation                   */
   double      endpoint1_force_body[3];       /* (N)  Force on endpoint 1 in the body frame               */
   double      endpoint2_force_body[3];       /* (N)  Force on endpoint 2 in the body frame               */
   double      endpoint1_force_SR[3];         /* (N)  Force on endpoint 1 in the structural frame         */
   double      endpoint2_force_SR[3];         /* (N)  Force on endpoint 2 in the structural frame         */
   double      endpoint1_moment_SR[3];        /* (N*m)  Moment on endpoint 1 in the structural frame      */
   double      endpoint2_moment_SR[3];        /* (N*m)  Moment on endpoint 2 in the structural frame      */
   double      endpoint1_max_force_SR[3];     /* (N)  Maximum force on endpoint 1 in the structural frame */
   double      endpoint2_max_force_SR[3];     /* (N)  Maximum force on endpoint 2 in the structural frame */
   double      deploy_load;                   /* (N)  Load computed using mass flow model during deploy   */
   double      deploy_impulse;                /* (N*s) Resultant impulse from deploy load model           */

} COUPLING_OUTPUTS;

typedef struct COUPLING_PARAMS {
   double      spring_constant;               /* (N/m) Spring Constant of the riser line                  */
   double      damping_constant;              /* (N*s/m) Damping Constant of the riser line               */
   bool        gain_moment;                   /* (--) Flag to apply gain to chute moment arm              */
   double      inelastic_load;                /* (N)  Inelastic riser load during deployment              */
   double      rs_cds_per_unit_length;        /* (m2/m)  CdS of riser/suspension line per meter           */
   double      canopy_cds_per_unit_length;    /* (m2/m)  CdS of canopy per meter                          */

   int          idex_mass;                    /* (--)  Size of cumulative mass table                      */
   double       dist_mass_table[5000];        /* (m)   Distance Table                                     */
   double       mass_table[5000];             /* (kg)  Cumulative Mass Table                              */

} COUPLING_PARAMS;

typedef struct COUPLING_WORK {
   double      endpoint1_force_eci[3];        /* (N)    Force on endpoint 1 in the ECI frame              */
   double      endpoint2_force_eci[3];        /* (N)    Force on endpoint 1 in the ECI frame              */
   double      moment_arm_gain;               /* (--)   Locally computed Moment Arm Gain                  */

   double       inertial_vel_diff[3];         /* (m/s)  Difference between main bag and payload inertial velocities */
   double       drag_area;                    /* (m2)   Total drag area during chute deploy               */
   double       canopy_drag_area;             /* (m2)   Drag area of canopy during deploy                 */
   double       drag;                         /* (N)    Drag force during deploy                          */
   double       deployed_mass;                /* (kg)   Deployed mass from chute bag                      */
   double       mass_per_unit_length;         /* (kg/m) Current mass per unit length                      */
   double       delta_mass;                   /* (kg)   Change in mass from last frame                    */
   double       delta_dist;                   /* (m)    Change in deploy distance from last frame         */
   double       mass_flow_load;               /* (N)    Load due to mass flow out of chute bag            */
   double       current_distance;             /* (m)    Current deployed chute distance                   */
   double       previous_deploy_distance;     /* (m)    Prior frame's deployed chute distance             */
   double       current_cumulative_mass;      /* (kg)   Current cumulative deployed chute mass            */
   double       previous_cumulative_mass;     /* (kg)   Prior frame's cumulative deployed chute mass      */
   double       VairInertial[3];              /* (m/s)  Payload velocity vector in Inertial Frame         */

   double       canopy_deploy_dist;           /* (m)    Distance canopy has been deployed                 */
   double       canopy_starting_mass;         /* (kg)   Chute deployed mass at start of canopy extraction */
   double       canopy_deployed_mass;         /* (kg)   Mass of canopy currently deployed                 */

} COUPLING_WORK;

typedef struct {
   COUPLING_INPUTS     input;                    /* (--)  */
   COUPLING_OUTPUTS    output;                   /* (--)  */
   COUPLING_PARAMS     param;                    /* (--)  */
   COUPLING_WORK       work;                     /* (--)  */   
}  CHUTE_COUPLING_DATA;

#ifdef __cplusplus
extern "C" {
#endif

int parachute_coupling ( CHUTE_COUPLING_DATA   *riser);

#ifdef __cplusplus
}
#endif

#endif
