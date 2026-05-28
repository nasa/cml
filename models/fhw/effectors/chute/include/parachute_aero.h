/********************************** TRICK HEADER **************************
PURPOSE:
    (Parachute Aerodynamics data structure)

PROGRAMMERS:
    (
    ((Richard Burt) (LMCO) (Apr 2009) (Initial Version))
    ((Jeff Semrau)  (Hon)  (May 2010) (Osiris Release 10.06, CR 763 Updates per CPAS Model Memo;CEV-LRS-10-003))
    ((Gesting) (OSR) (05/2010) (791) (Update OIMU to Honeywell updates))
    ((Jeff Semrau)  (Hon)  (Jan 2011) (CR 859 Updates to simple wake effects model))
    ((Jeff Semrau) (HON) (02/2012) (lmbp1395) (Chute Pressure Recovery Fraction))
    ((Jeff Semrau) (HON) (05/2012) (lmbp1650) (Knacke Wake Model))
    ((Jeff Semrau) (HON) (01/2013) (lmbp2290) (MM11 Updates))
    )
**************************************************************************/

#ifndef PARACHUTE_AERO_H
#define PARACHUTE_AERO_H

#include <stdbool.h>

typedef struct Aero_Inputs { 
   // FIXME Ghan August 2020
   // The enum type aero_mode and the variable aero_mode shouldn't have the same name. It's causing build warnings in the unit sim.
   enum         aero_mode { ballistic_aero = 0, aero_coef      = 1} aero_mode; /* (--) Parameter to tell aero model what type of aero to use */
   double       mass;                  /* (kg)    Ballistic Body mass                                 */
   double       body_rates[3];         /* (rad/s)   Rotational rates relative to inertial frame, expressed in body frame */
   double       VairSR[3];             /* (m/s)   Chute Velocity in Structural Frame                  */ 
   double       dynamic_pressure;      /* (N/m2)  Chute dynamic pressure                              */
   double       mach;                  /* (--)    Chute Mach Number                                   */
   double       alpha_total;           /* (rad)     Chute Total Angle of Attack                         */
   double       sep_distance;          /* (m)     Separation distance between chute and payload       */
   double       CDS;                   /* (m2)    Chute Drag Area                                     */ 
   double       Chute_Area;            /* (m2)    Inflated Chute Area                                 */ 
   double       CBAR;                  /* (m)     Mean Aerodynamic Chord of chute                     */
   double       T_aero_to_body[3][3];  /* (--)    Rotation from Chute Aero Frame to Chute Body Frame  */
   double       T_body_to_SR[3][3];    /* (--)    Rotation from Chute Body Frame to Chute Structural Frame  */

   double       att_pnt_inertial_pos[3]; /* (m)   Chute Attach Point Inertial Position                */
   double       chute_inertial_pos[3]; /* (m)     Chute Inertial Position                             */

   double       min_drag;              /* (N)     Minimum drag to use on chute                        */
} Aero_Inputs;

typedef struct Aero_Output { 
   double       force_SR[3];            /* (N)   Parachute Forces in the Chute Structural Reference frame   */
   double       moment_SR[3];           /* (N*m) Parachute Moments in the Chute Structural Reference frame  */
   double       drag;                   /* (N)   Computed aerodynamic drag                                  */
} Aero_Outputs;

typedef struct Aero_Params { 
   /* Ballistic Coefficient Model */
   double       BC;                     /* (kg/m2)  Ballistic Coefficient       */

   /* 6-dof Coefficient Model */
   // Axial Force Coefficient Declarations
   double       alpha_ca_table[100];    /* (rad)    Alpha Table for CA            */
   double       mach_ca_table[100];     /* (--)   Mach Table for CA             */
   double       ca_table[100];          /* (--)   CA Table                      */
   int          idex_ca;                /* (--)   i index length  for CA        */
   int          jdex_ca;                /* (--)   j index length  for CA        */

   // Side Force Coefficient Declarations
   double       alpha_cy_table[100];    /* (rad)    Alpha Table for CY            */
   double       cy_table[100];          /* (--)   CY Table                      */
   int          idex_cy;                /* (--)   i index length  for CY        */

   // Pitch Moment Coefficient Declarations
   double       alpha_cm_table[100];    /* (rad)    Alpha Table for CM            */
   double       cm_table[100];          /* (--)   CM Table                      */
   int          idex_cm;                /* (--)   i index length  for CM        */

   // Pitch Damping Coefficient Declarations
   double       mach_cq_table[100];     /* (--)   Mach Table for CM             */
   double       cq_table[100];          /* (--)   CQ Table                      */
   int          idex_cq;                /* (--)   i index length  for CQ        */

   // Wake Degradation Declarations
   bool         Simple_Wake_Effects_Enabled; /* (--)  Simple Wake Effects Enable Flag     */
   double       decay_factor;                /* (--)  Decay factor in washout of wake effects */

   double       nominal_cd;             /* (--)  Nominal Cd                     */

   //PRF
   double       PRF;                    /* (--)  Pressure Recovery Factor       */

} Aero_Params;

typedef struct Aero_Work { 
   /* Ballistic Coefficient Model */
   double       vmag2;                  /* (m/s)  Magnitude of current airspeed, squared    */
   double       vmag;                   /* (m/s)  Magnitude of current airspeed             */
   double       v_dir_str[3];           /* (--)   Velocity unit vector in SR frame          */

   /* 6-dof Coefficient Model */  
   double       aero_ca;                /* (--)   Axial Force Coefficient                   */
   double       aero_cy;                /* (--)   Normal Force Coefficient                  */      
   double       aero_cm;                /* (--)   Pitching Moment Coefficient               */
   double       aero_cq;                /* (--)   Pitch Dampening Coefficient               */
   double       VairB[3];               /* (m/s)  Airspeed in Body Frame                    */
   double       QDSC;                   /* (N*m)   Drag Moment                               */
   double       QDS;                    /* (N)    Drag Force                                */
   double       roll_angle;             /* (rad)    Parachute Roll Angle                      */
   double       damping;                /* (--)   Damping term for aero moment              */
   double       force_chute_aero[3];    /* (N)    Parachute Forces in the chute aero frame  */
   double       moment_chute_aero[3];   /* (N*m)  Parachute Moments in the chute aero frame */
   double       force_chute_body[3];    /* (N)    Parachute Forces in the chute body frame  */
   double       moment_chute_body[3];   /* (N*m)  Parachute Moments in the chute body frame */

   /* Simple Wake Effects Model */
   double       sep_distance[3];        /* (m)    Separation distance vector of chute to payload             */
   double       sep_distance_mag;       /* (m)    Separation distance magnitude of chute to payload          */
   double       qbar_sf;                /* (--)   Scale factor on chute dynamic pressure due to wake effects */

} Aero_Work; 
   

typedef struct {
   Aero_Inputs     input;
   Aero_Outputs    output;
   Aero_Params     param;
   Aero_Work       work;
  
} AERO_DATA; 

#ifdef __cplusplus
extern "C" {
#endif

int parachute_aero (AERO_DATA   *A);

#ifdef __cplusplus
}
#endif

#endif
