/********************************** TRICK HEADER **************************
PURPOSE:
    (Parachute inflation model data structure)

REFERENCE:
    (( -- ))

ASSUMPTIONS AND LIMITATIONS:
    (( -- ))

PROGRAMMERS:
     (((Jeff Semrau) (HW) (Apr 2009) (Initial Version)))
     (((Jeff Semrau) (HW) (May 2010) (CR 763)))           
     (((Jeff Semrau) (HW) (July 2011) (lmbp934)))           
     (((Jeff Semrau) (HW) (May 2012) (lmbp1650 - MMv9 Updates)))           
     (((Jeff Semrau) (HW) (Jan 2013) (lmbp2290 - MMv11 Updates)))           
**************************************************************************/
#include <stdbool.h>

#ifndef _PARACHUTE_INFLATION_
#define _PARACHUTE_INFLATION_

typedef struct Inflation_Inputs   
{
   double   deploy_velocity;                         /* (m/s) Deploy velocity of reef stage                      */
   double   nominal_diameter;                        /* (m)   Fully inflated diameter of chute                   */
   bool     PEC_fired;                               /* (--)  PEC command for reefing transition                 */

} Inflation_Inputs;

typedef struct Inflation_Outputs   
{
   double   CDS;                                     /* (m2)  Drag Area of Chute                                 */
   double   current_diameter;                        /* (m)   Current diameter of inflated chute                 */
   double   current_radius;                          /* (m)   Current radius of inflated chute                   */
   double   Chute_Area;                              /* (m2)  Area of Chute                                      */
   bool     inflation_complete;                      /* (--)  Inflation Complete flag                            */
   double   PercentInflated;                         /* (--)  Percentage Chute is Inflated                       */

} Inflation_Outputs; 

typedef struct Inflation_Params   
{
   double   CDS[3];                                  /* (m2)  Drag Area of each reef stage                       */
   double   overinflation_factor[3];                 /* (--)  Gain on reef area to compute overinflation         */
   double   vel_exponent[3];                         /* (--)  Exponent for velocity in fill time equation        */
   double   inflation_factor[3];                     /* (--)  Exponent for CDS equation                          */
   double   fill_constant[3];                        /* (--)  Fill constant for fill time equation               */
   double   deflation_time[3];                       /* (s)   Time to deflate from overinfated to nominal        */
   double   drag_coeff;                              /* (--)  Drag coeff of chute                                */
   double   transition_time[3];                      /* (s)   Targeted reef time of each reef stage              */
   double   transition_time_factor[3];               /* (--)  Gain on Nominal Reef Times, MM14 Update            */
   int      num_reef_stages;                         /* (--)  Number of planned reef stages                      */

   bool     full_inflation_commanded;                /* (--)  Command to model to skip all reefing               */
   bool     model_first_pass;                        /* (--)  Model first pass flag                              */
   bool     reef_stage_first_pass;                   /* (--)  Reef stage first pass flag                         */
   bool     use_DCLDYN_Inflation_Model;              /* (--)  Flag to use DCLDYN Inflation Model                 */

   double   CDS_Gain; /* (--) Gain on Chute CDS to account for Flyout angle cosine loss, FBCPs Only and MF Analysis */

   
} Inflation_Params;

typedef struct Inflation_Work 
{ 
   double   fill_time;                               /* (s)   Time to fill chute for current reef stage          */
   double   model_start_time;                        /* (s)   Time chute inflation started                       */
   double   reef_start_time;                         /* (s)   Time current reef stage started                    */
   double   deflation_start_time;                    /* (s)   Time deflation to steady state inflation started   */
   double   deflation_percent;                       /* (--)  Deflation percentage                               */
   double   total_elasped_time;                      /* (s)   Total elasped time in chute inflation model        */
   double   stage_elasped_time;                      /* (s)   Elasped time in current reef state                 */
   double   deflation_elasped_time;                  /* (s)   Deflation elasped time                             */
   double   overinflation_CDS[3];                    /* (m2)  CDS of each reef stage with overinflation factor   */
   double   reef_stage_CDS[3];                       /* (m2)  Steady State CDS of each reef stage                */
   double   inflation_CDS;                           /* (m2)  Target CDS Used in Fill Time Computation           */
   double   deflation_CDS;                           /* (m2)  CDS Used to decrement from Overinflation CDS       */
   double   initial_CDS;                             /* (m2)  Beginning CDS Used in Fill Time Computation        */
   double   delta_reef_ratio;                        /* (--)  Reef Ratio used in Fill Time Computation           */
   double   percent_filled;                          /* (--)  Precentage current reef stage is filled            */
   double   deflation_slope;                         /* (--)  Value to decrement CDS per cycle during deflation  */
   double   area_ratio[3];                           /* (--)  Area Ratio of each reef stage                      */

   enum fill_states { xreefed       = 0, 
                      xdisreef      = 1,
                      xfull_deploy  = 2 } fill_state;

   enum reef_stages { xreef_1       = 0, 
                      xreef_2       = 1, 
                      xfull         = 2 } reef_stage;

   //DCLDYN INFLATION MODEL VARIABLES
   double   tmp_scale;                               /* (--) Scale factor on inflation time                                 */
   double   total_fill_time;                         /* (--) Fill time, including nominal inflation and overinflation times */
   double   inflation_time;                          /* (--) Time to inflate chute reef stage                               */

   //Flag for Skipped Second Stage Modeling 
   bool  fill_time_computed[3];                      /* (--) Flag to indicate if reef stage fill time has been computed */

} Inflation_Work;

typedef struct 
{
   Inflation_Inputs      input;
   Inflation_Outputs     output;
   Inflation_Params      param;
   Inflation_Work        work;

} INFLATION_DATA;


/* ------------------- */
/* Function Prototypes */
/* ------------------- */
#ifdef __cplusplus
extern "C" {
#endif
void xcompute_chute_CDS ( INFLATION_DATA     *chute_inflation,
                         double              sim_local_time);

#ifdef __cplusplus
}
#endif

#endif


