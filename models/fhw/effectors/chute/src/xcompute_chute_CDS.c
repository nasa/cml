/******************************* TRICK HEADER **********************************
PURPOSE:
    (To compute the current Drag Area, CdS, of a chute as a function of elasped
     time since chute deployment, time required to inflate chute to required area,
     and chute characteristics.)
REFERENCE:
    ((
        "PA1_Study_Parameters_01042008,xls",
         Chris Madsen email Jan 04, 2008))

ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))
LIBRARY DEPENDENCY:
    ((DCLDYN_fill_time.o))

PROGRAMMERS:
    (((Jeff Semrau) (Honeywell) (09-Jan-2008) (OSIRIS V2.2.1 release))
     ((Jeff Semrau) (Honeywell) (25-Apr-2008) (OSIRIS V2.3.2 release))
     ((Jeff Semrau) (Honeywell) (15-Dec-2008) (Updated chute timer to be relative
                                               to time of chute line stretch))
     ((Jeff Semrau) (Honeywell) (Dec 2009)    (New version for HiFi Chute Model))
     ((Jeff Semrau) (Honeywell) (May 2010)    (Osiris CR 763)))
     ((Jeff Semrau) (Honeywell) (June 2010)   (Osiris DR 806) (Corrected bug in computing elasped stage time for final disreef case))
     ((Jeff Semrau) (Honeywell) (May 2011)    (lmbp683) (Updated code to handle skipped reefing stages))
     ((Jeff Semrau) (Honeywell) (July 2011)   (lmbp934) (Updated code for new skipped 2nd stage modeling paradigm))
     ((Jeff Semrau) (Honeywell) (May 2012)    (lmbp1650) (LRS MMv9 updates))
     ((Jeff Semrau) (Honeywell) (Oct 2012)    (lmbp2122) (Updates to handle case if dispersion file has smaller CDS for reef2 than reef1))
     ((Jeff Semrau) (Honeywell) (Jan 2013)    (lmbpR2290 (LRS MMv11 updates))
     ((Bingquan Wang) (OSR) (Apr 2017) (Antares) (Fixed the compilation warning of float-point number equality comparision)))
*******************************************************************************/


#include <math.h>
#include <stdio.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <fenv.h>  //fedisableexcept, feenableexcept

#include <assert.h>

#include "../include/parachute_inflation.h"
#include "../include/parachute_DCLDYN_prototypes.h"

#define _USE_MATH_DEFINES
#include <math.h>

void xcompute_chute_CDS(INFLATION_DATA   *inflation,
                       double sim_local_time )
{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in xcompute_chute_CDS \n";
    if (!inflation) {
       printf(error_msg);
       return;
    }

   // First Pass Check to set needed parameters, only executed once per chute model; drogues or mains
   if(inflation->param.model_first_pass) {
      inflation->work.model_start_time   = sim_local_time;

      for (int i = 0; i < 3; i++) { // 3 is the number of reef stages, need to make a parameter

         inflation->work.area_ratio[i] = inflation->param.CDS[i]/inflation->param.CDS[2];

         inflation->work.overinflation_CDS[i] =
            inflation->param.CDS[i] * inflation->param.overinflation_factor[i];

      }

      inflation->param.model_first_pass = false;
   }

   // Determine if it is time to transition to next reef stage

   if (inflation->work.reef_stage < xfull) {
      if ((inflation->work.total_elasped_time > inflation->param.transition_time[inflation->work.reef_stage]) || (inflation->input.PEC_fired == true)) {
         inflation->param.reef_stage_first_pass = true;
         inflation->work.reef_stage             = inflation->work.reef_stage + 1;
         inflation->work.stage_elasped_time     = 0.0;
      }
   }

   // First Pass Logic to set up model for current reef stage of current chute
   // Executed for every reef stage
   if (inflation->param.reef_stage_first_pass) {
       if (inflation->param.full_inflation_commanded) {
          inflation->work.fill_state  = xfull_deploy;
          inflation->work.reef_stage  = xfull;
          inflation->work.initial_CDS = 0.0;
       }
       else if (inflation->work.reef_stage == xreef_1) {
          inflation->work.fill_state  = xreefed;
          inflation->work.initial_CDS = 0.0;
       }
       else {
          inflation->work.fill_state  = xdisreef;
          inflation->work.initial_CDS = inflation->output.CDS;

          if (inflation->work.reef_stage == xreef_2) {
             inflation->work.area_ratio[0] = inflation->work.initial_CDS/inflation->param.CDS[2];
          }
          if (inflation->work.reef_stage == xfull) {
             inflation->work.area_ratio[1] = inflation->work.initial_CDS/inflation->param.CDS[2];
          }
       }

       inflation->work.inflation_CDS  = inflation->param.CDS[inflation->work.reef_stage] - inflation->work.initial_CDS;
       DCLDYN_fill_time(inflation);

       inflation->work.deflation_elasped_time = 0.0;

       if (inflation->param.deflation_time[inflation->work.reef_stage] > 0.0) {
          inflation->work.deflation_slope =
             (inflation->work.overinflation_CDS[inflation->work.reef_stage] - inflation->param.CDS[inflation->work.reef_stage]) /
             inflation->param.deflation_time[inflation->work.reef_stage];
       }

       inflation->work.reef_start_time          = sim_local_time;
       inflation->param.reef_stage_first_pass  = false;
   }
   // END FIRST PASS LOGIC

   inflation->work.total_elasped_time = sim_local_time - inflation->work.model_start_time;
   inflation->work.stage_elasped_time = sim_local_time - inflation->work.reef_start_time;
   if (inflation->work.stage_elasped_time < 0.0) inflation->work.stage_elasped_time = 0.0;

   //disable the FP exception to allow isnan()/isinf() to work
   int fe_prev = fedisableexcept(FE_ALL_EXCEPT);
   assert(-1 != fe_prev);

   inflation->work.percent_filled     = inflation->work.stage_elasped_time / inflation->work.fill_time;

   //handle the cases of div-0, overflow or invalid-ops for the calculation above
   if (isnan(inflation->work.percent_filled) || isinf(inflation->work.percent_filled)) {
        inflation->work.percent_filled = 1.0;
   }

   //recover the previous settings of FP exceptions
   feenableexcept(fe_prev);

   // Compute Chute Drag Area during inflation
   if (inflation->param.use_DCLDYN_Inflation_Model == true) {
      inflation->work.inflation_time = inflation->work.total_fill_time;
      inflation->work.fill_time_computed[inflation->work.reef_stage] = true; //Flag used in modeling skipped 2nd stage
   }
   else {
      inflation->work.inflation_time = inflation->work.fill_time;
      inflation->work.fill_time_computed[inflation->work.reef_stage] = true; //Flag used in modeling skipped 2nd stage
   }

   if (inflation->work.stage_elasped_time < inflation->work.inflation_time) {

      inflation->output.CDS =
         inflation->work.initial_CDS + inflation->work.inflation_CDS *
         pow (inflation->work.percent_filled, inflation->param.inflation_factor[inflation->work.reef_stage]);

      inflation->work.deflation_start_time = sim_local_time;
   }
   // Compute Chute Drag Area during deflation from overinflated state
   else if (inflation->work.stage_elasped_time < (inflation->work.inflation_time + inflation->param.deflation_time[inflation->work.reef_stage])) {

      inflation->work.deflation_elasped_time =
         sim_local_time - inflation->work.deflation_start_time;

      inflation->work.deflation_CDS =
         inflation->work.deflation_slope * inflation->work.deflation_elasped_time;

      inflation->output.CDS =
         inflation->work.overinflation_CDS[inflation->work.reef_stage] - inflation->work.deflation_CDS;

         if (inflation->param.use_DCLDYN_Inflation_Model == true) {

            if (inflation->param.overinflation_factor[inflation->work.reef_stage] > 1.0) {

               inflation->work.deflation_percent = -inflation->work.deflation_elasped_time/inflation->param.deflation_time[inflation->work.reef_stage];
               inflation->output.CDS =
               inflation->work.overinflation_CDS[inflation->work.reef_stage] * pow(inflation->param.overinflation_factor[inflation->work.reef_stage], inflation->work.deflation_percent);

            }
            else {
               inflation->output.CDS = inflation->param.CDS[inflation->work.reef_stage];
            }
         }
   }
   // Chute Drag Area is steady state, = drag area of current reef stage
   else {
      inflation->output.CDS = inflation->param.CDS[inflation->work.reef_stage];
   }

   //Check for zero fill time (skipped stage) and set CDS to current reef stage value
   if (fabs(inflation->work.total_fill_time) <= 0.0) { //to avoid the float-equality compilation warning, use "fabs(a)<=0" test "a==0"
      inflation->output.CDS = inflation->param.CDS[inflation->work.reef_stage];

      //This is to handle scenario where CDS from GEV function is lower from 2nd stage than 1st stage
      if (inflation->param.CDS[inflation->work.reef_stage] < inflation->param.CDS[inflation->work.reef_stage-1]) {
         inflation->param.CDS[inflation->work.reef_stage] = inflation->param.CDS[inflation->work.reef_stage-1];
      }
   }

   if (inflation->work.reef_stage == xfull) {
      if (inflation->work.stage_elasped_time >= (inflation->work.inflation_time + inflation->param.deflation_time[inflation->work.reef_stage])) {
         inflation->output.inflation_complete = true;
      }
   }

   //Compute Current Chute Diameter & Radius
   inflation->output.current_diameter = sqrt((4.0 * inflation->output.CDS)/(inflation->param.drag_coeff * M_PI));
   inflation->output.current_radius   = inflation->output.current_diameter/2.0;

   //Compute Chute Area
   double temp_cds;
   temp_cds = inflation->output.CDS * inflation->param.CDS_Gain;
   inflation->output.CDS = temp_cds;

   inflation->output.Chute_Area = (inflation->output.CDS / inflation->param.drag_coeff);


   //Compute percentage inflated
   inflation->output.PercentInflated = inflation->output.CDS/inflation->param.CDS[xfull];

}

