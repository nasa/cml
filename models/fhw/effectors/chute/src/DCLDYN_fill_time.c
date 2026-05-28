/******************************* TRICK HEADER **********************************
PURPOSE:
    (This routine is called from the routine, compute_chute_CDS, to calculate the 
     time required to fill the current chute as a function of CM velocity and chute
     characteristics using the DCLDYN inflation model from Airborne Systems)
REFERENCE:
    ((   "PA1_Study_Parameters_01042008,xls", 
         Chris Madsen email Jan 04, 2008))

ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))

PROGRAMMERS:
    ((Jeff Semrau) (Honeywell) (Dec 2009) (Initial Release)))
    ((Jeff Semrau) (Honeywell) (May 2011) (Added check to account for skipped stage modeling)))
    ((Jeff Semrau) (Honeywell) (May 2012) (LRS MMv9 Updates)))
    ((Jeff Semrau) (Honeywell) (Oct 2012) (Updates to handle if CPAS gives 2nd stage CDS smaller than 1st stage, LMBP Task #2122)))
*******************************************************************************/

#include "../include/parachute_inflation.h"
#include "../include/parachute_DCLDYN_prototypes.h"
#include <math.h>
#include <stdio.h>

void DCLDYN_fill_time (INFLATION_DATA   *inflation)
{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in DCLDYN_fill_time.c \n";
    if (!inflation) {
       printf(error_msg);
       return;
    }

   //Compute Reef Area Ratio 
   if (inflation->work.fill_state == xreefed) {
      inflation->work.delta_reef_ratio = inflation->work.area_ratio[inflation->work.reef_stage];
   }
   else if (inflation->work.fill_state == xdisreef) {
      inflation->work.delta_reef_ratio = 
         inflation->work.area_ratio[inflation->work.reef_stage] - inflation->work.area_ratio[inflation->work.reef_stage - 1];
   }
   else if (inflation->work.fill_state == xfull_deploy) {
      inflation->work.delta_reef_ratio = 1.0;
   }

   if (inflation->work.delta_reef_ratio < 0.0) { 
      inflation->work.delta_reef_ratio = 0.0;
   }

   //Compute Fill Time
   inflation->work.fill_time = 
      (inflation->param.fill_constant[inflation->work.reef_stage] * inflation->input.nominal_diameter * sqrt(inflation->work.delta_reef_ratio)) / 
       pow(inflation->input.deploy_velocity, inflation->param.vel_exponent[inflation->work.reef_stage]); 
  
   if (inflation->work.reef_stage == xreef_1) {
      inflation->work.tmp_scale = inflation->param.overinflation_factor[inflation->work.reef_stage];
   }
   else {

      //Check for skipped stage to prevent divide by 0 error
      if ( inflation->work.area_ratio[inflation->work.reef_stage] == inflation->work.area_ratio[inflation->work.reef_stage - 1]) { 
         inflation->work.tmp_scale = 0.0;
      }
      else { 
         
         inflation->work.tmp_scale = 
            (inflation->param.overinflation_factor[inflation->work.reef_stage] * 
             inflation->work.area_ratio[inflation->work.reef_stage] - inflation->work.area_ratio[inflation->work.reef_stage - 1]) / 
            (inflation->work.area_ratio[inflation->work.reef_stage] - inflation->work.area_ratio[inflation->work.reef_stage - 1]);
      }

      if (inflation->work.tmp_scale < 0.0) { 
         inflation->work.tmp_scale = 0.0;
      }
   }

   inflation->work.total_fill_time = 
      inflation->work.fill_time * pow(inflation->work.tmp_scale, (1.0/inflation->param.inflation_factor[inflation->work.reef_stage]));

}
