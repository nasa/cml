
/********************************** TRICK HEADER **************************
PURPOSE:
    (Parachute mass properties data structure)

REFERENCE:
    (( -- ))

ASSUMPTIONS AND LIMITATIONS:
    (( -- ))

PROGRAMMERS:
    (
    ((Jeff Semrau) (HW) (Apr 2009) (Initial Version))
    ((Jeff Semrau) (HW) (May 2010) (Osiris CR 763))
    ((Gesting) (OSR) (05/2010) (791) (Update OIMU to Honeywell updates))
    ((Jeff Semrau) (HW) (Aug 2010) (Osiris DR 824 ))
    ((Jeff Semrau) (HW) (Jan 2011) (Osiris CR 859 ) (Mass Props Mark2 updates))
    ((Jeff Semrau) (HW) (May 2012) (LMBP 1650 ) (LRS MMv9 Updates))
    ((Jeff Semrau) (HW) (Aug 2013) (LMBP 2720 ) (Added filter of atm density to smooth out mass spikes due to low GRAM Rate))
    ((Jeff Semrau) (HW) (Nov 2013) (LMBP 3148 ) (New declarations to improve calculation of mass rate))
    )
**************************************************************************/

#include <stdbool.h>
#include "parachute_inflation.h"

#ifndef _PARACHUTE_MASS_PROPS
#define _PARACHUTE_MASS_PROPS

typedef struct Mass_Inputs   
{
   double   nominal_diameter;                        /* (m)     Nominal diameter of chute         */
   double   current_diameter;                        /* (m)     Current diameter of chute         */
   double   air_density;                             /* (kg/m3) Current air density               */
   double   dt;                                      /* (s)     Execution time step of code       */
   double   chute_velocity[3];                       /* (m/s)   Current chute velocity vector     */
   double   elasped_inflation_time;                  /* (s)     Elasped time since start of inflation */

   bool     first_pass;                              /* (--)    First Pass Flag                   */

} Mass_Inputs;

typedef struct Mass_Outputs  
{
   double   inertia[3][3];                           /* (kg*m2)  Chute inertia matrix               */
   double   mass_accel[3];                           /* (m/s2)  Acceleration due to apparent mass  */
   double   chute_mass[3];                           /* (kg)    Chute mass vector                  */

} Mass_Outputs;

typedef struct Mass_Params   
{
   double   canopy_mass;                             /* (kg)    Mass of parachute canopy             */
   double   parachute_dry_mass;                      /* (kg)    Mass of parachute system             */
   double   mass_below_skirt;                        /* (kg)    Mass of chute components below skirt */
   double   enclosed_mass_sf;                        /* (--)    Enclosed Mass scale factor           */
   double   app_mass_sf;                             /* (--)    Apparant Mass scale factor           */
   double   dlp;                                     /* (m)     Suspension Line Length               */
   double   volume_sf;                               /* (--)    Scale Factor used to compute chute enclosed mass */

   //NEW VARS FOR MASS PROPS MARK2
   double TNHFAC;                                    /* (--)    Added Mass Emphirical Parameter   */
   double XK1;                                       /* (--)    Added Mass Emphirical Parameter   */
   double XK2;                                       /* (--)    Added Mass Emphirical Parameter   */
   double XM1;                                       /* (--)    Added Mass Emphirical Parameter   */
   double XM2;                                       /* (--)    Added Mass Emphirical Parameter   */
   double B1;                                        /* (--)    Added Mass Emphirical Parameter   */
   double B2;                                        /* (--)    Added Mass Emphirical Parameter   */
   double A;                                         /* (--)    Added Mass Emphirical Parameter   */
   double B;                                         /* (--)    Added Mass Emphirical Parameter   */
   double C;                                         /* (--)    Added Mass Emphirical Parameter   */
   double D;                                         /* (--)    Added Mass Emphirical Parameter   */
   
} Mass_Params;

typedef struct Mass_Work    
{ 
   double   normalized_diameter;                     /* (--)    Normalized diameter of current chute diameter  */
   double   prev_normalized_diameter;                /* (--)    Normalized diameter of previous cycle diameter */
   double   normalized_diameter_delta;               /* (--)    Delta of current and prev normalized diameters */
   double   diameter_sqrd;                           /* (m2)    Current chute diameter squared                 */
   double   hemi_volume;                             /* (m3)    Current chute Hemisphere volume                */
   double   cyl_volume;                              /* (m3)    Current chute Cylinder volume                  */
   double   enclosed_hemisphere_mass;                /* (kg)    Current enclosed air mass in chute hemisphere  */
   double   enclosed_cylinder_mass;                  /* (kg)    Current enclosed air mass in chute cylinder    */
   double   enclosed_mass;                           /* (kg)    Total enclosed air mass in chute               */
   double   apparent_mass;                           /* (kg)    Current apparent air mass of chute             */
   double   parachute_mass_x;                        /* (kg)    Parachute mass in X body axis                  */
   double   parachute_mass_y;                        /* (kg)    Parachute mass in Y body axis                  */
   double   parachute_mass_z;                        /* (kg)    Parachute mass in Z body axis                  */
   double   prev_parachute_mass_x;                   /* (kg)    Prior cycle parachute mass in X body axis      */
   double   prev_parachute_mass_y;                   /* (kg)    Prior cycle parachute mass in Y body axis      */
   double   prev_parachute_mass_z;                   /* (kg)    Prior cycle parachute mass in Z body axis      */
   double   mass_x_rate;                             /* (kg/s)  Mass rate of chute in X Body axis              */
   double   mass_y_rate;                             /* (kg/s)  Mass rate of chute in Y Body axis              */
   double   mass_z_rate;                             /* (kg)    Mass rate of chute in Z Body axis              */
   double   canopy_inertia;                          /* (kg*m2) Inertia of chute canopy                        */

   //NEW VARS FOR MASS PROPS MARK2
   double   dnorm_sf;                                /* (--)   Scale factor for diameter ratio                 */
   double   dnorm0;                                  /* (--)   Diameter ratio at reef stage start              */
   double   dnorm1;                                  /* (--)   Diameter ratio at reef stage end                */
   double   dnorm;                                   /* (--)   Current diameter ratio                          */
   double   current_diameter;                        /* (m)    Current diameter of chute                       */
   double   rr_prime;                                /* (--)   Filtered reefing ratio                          */
   double   X;                                       /* (--)   Total slope of added mass equation              */
   double   B;                                       /* (--)   Y Intercept of added mass gain                  */
   double   DCLF;                                    /* (--)   Added mass gain                                 */
   double   prelim_mass_x_rate;                      /* (kg/s) Preliminary mass rate in X direction            */
   double   prev_time;                               /* (s)    Stage relative previous time                    */
   double   corrected_time;                          /* (s)    Corrected stage relative time                   */
   double   R1;                                      /* (--)   Added mass rate rise rate scale factor          */
   double   max_mass_x_rate;                         /* (kg/s) Added mass rate at inflation complete           */

   //Declarations to filter atm density, to smooth out density spikes
   //from slow rate GRAM is called in the sim. 
   double prev_density;                             /* (kg/m3) Prevous air density  */
   double new_density;                              /* (kg/m3) New air density      */
   double rc;                                       /* (--)    Air density filter coefficient   */
   double alp;                                      /* (--)    Air density filter coefficient   */

   double tdone;                                    /* (s)  Sim time when stage inflation is complete */
   double t2;                                       /* (s)  Elasped time since stage inflation complete */

} Mass_Work;

typedef struct MASS_DATA   
{
   Mass_Inputs      input;
   Mass_Outputs     output;
   Mass_Params      param;
   Mass_Work        work;

} MASS_DATA;


/* ------------------- */
/* Function Prototypes */
/* ------------------- */

#ifdef __cplusplus
extern "C" {
#endif

int parachute_mass_properties_mark2 ( double sim_time_local, 
                                      INFLATION_DATA   *inflation,
                                      MASS_DATA        *mass);

#ifdef __cplusplus
}
#endif

#endif
