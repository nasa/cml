/******************************* TRICK HEADER **********************************
PURPOSE:
    (This routine is called from the routine, parachute_twist_torque, to calculate 
     the parachute cluster flyout angle waveform.) 
REFERENCE:
    (( ))

ASSUMPTIONS AND LIMITATIONS:
    ((None))
LIBRARY DEPENDENCY:
   (
      (parachute_waveform.o)
   )

PROGRAMMERS:
    (
    ((Jeff Semrau)  (Honeywell) (Mar 2013) (Initial Release)) 
    )

*******************************************************************************/

#include "../include/parachute_twist_torque.h"
#include "trick/constant.h"              /* For unit conversions */
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

void parachute_waveform  (WAVEFORM_DATA   *waveform) 
{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in parachute_waveform.c \n";
    if (!waveform) {
       printf(error_msg);
       return;
    }

   int i;

   /* Compute flyout angle */
   waveform->work.flyout_angle = waveform->param.flyout_bias; 
   for(i = 0; i < waveform->param.flyout_range; i++){
      waveform->work.flyout_angle = 
         waveform->work.flyout_angle + 
         waveform->param.flyout_A[i] * sin((2.0 * M_PI * (waveform->work.t - waveform->param.flyout_delta_t[i])) / waveform->param.flyout_T[i]) +
         waveform->param.flyout_Anought[i];
   }

   waveform->work.Sp = waveform->param.Sp_bias; 
   for(i = 0; i < waveform->param.Sp_range; i++){
      waveform->work.Sp_delta = 
         waveform->param.Sp_A[i] * sin((2.0 * M_PI * (waveform->work.t - waveform->param.Sp_delta_t[i]))/waveform->param.Sp_T[i]) + 
         waveform->param.Sp_Anought[i];

      waveform->work.mod_check = fmod(((waveform->work.t - waveform->param.Sp_delta_t[0]) / (waveform->param.Sp_T[0]/2)),4);

      if(waveform->work.mod_check < 0.0) { 
         waveform->work.mod_check = waveform->work.mod_check + 4.0;
      }

      if ((i == 0) && (waveform->work.mod_check <= 3)) { 
         waveform->work.Sp_delta = 
            (waveform->param.Sp_Anought[i] + (waveform->work.Sp_delta - waveform->param.Sp_Anought[i])) / waveform->param.Sp_K[i];
      }

      waveform->work.Sp = waveform->work.Sp + waveform->work.Sp_delta;
   }

}
