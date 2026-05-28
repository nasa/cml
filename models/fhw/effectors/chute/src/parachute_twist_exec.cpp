/******************************* TRICK HEADER **********************************
PURPOSE:
    (To manage call to chute twist torque model.)
REFERENCE:
    (( -- ))                                   
ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))
LIBRARY DEPENDENCY:
    (
    (parachute_twist_torque.o)
    (parachute_twist_exec.o)
    )

PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (Oct 2015) (Initial Release))             
*******************************************************************************/

/*
 * Include files for package
 */

#include "../include/parachute_exec.hh"
#include "../include/parachute_proto.hh"

#include "trick/trick_math.h"
#include "trick/exec_proto.h"            /* For get_job_cycle routine */
#include "trick/constant.h"              /* For unit conversions */

#include <cmath>

void parachute_twist_exec (PARACHUTE_DATA&    chute) 
                        
{

   // Logic to turn on twist torque based on number of chutes being modeled 
   for (int j = 0; j < chute.param.num_chutes; j++) {
      if  (chute.chute_phase[j] >= PARACHUTE_DATA::main_inflating) {
          chute.turn_twist_on_check = chute.turn_twist_on_check + 1;
      }
      else {
        chute.turn_twist_on_check = 0;
      }
   }

   if (chute.turn_twist_on_check >= chute.param.num_chutes) {
       chute.turn_twist_on_check = chute.param.num_chutes;
   }

   // Compute Total Riser Force for Twist Torque Input 
   chute.twist.input.riser_load = 0.0;
   for (int k = 0; k < chute.param.num_chutes; k++) {
      if (chute.param.fail_random_mortar == true) {
         if (k == chute.param.failed_chute) {
            continue;
         }
      }
      chute.twist.input.riser_load = chute.twist.input.riser_load + chute.main_coupling[k].output.load;
   }
  
   // Compute Line Twist Torque 
   if (chute.param.line_twist_enabled == true && chute.turn_twist_on_check == chute.param.num_chutes) { 
   
      // Buffer inputs 
      //Calculate CM Rate about riser as roll rate input
      V_NORM(chute.twist.work.riser_uvec, chute.main_coupling[0].output.endpoint1_force_body);
      chute.twist.input.roll_rate = V_DOT(chute.input.pyld_inertial_rate, chute.twist.work.riser_uvec);

      chute.twist.input.dt         = exec_get_job_cycle(0);
      chute.twist.input.cev_weight = chute.input.pyld_mass * V_MAG(chute.input.gravaccel);

      if (chute.param.fail_random_chute == true) { 
        chute.twist.param.num_chutes = chute.param.num_chutes - 1;
      }
      else { 
        chute.twist.param.num_chutes = chute.param.num_chutes;
      }

      chute.twist.input.main_diameter = chute.param.main_diameter; 
      
      V_SCALE(chute.twist.input.inertial_vector, chute.relstate.chute[0].output.chute_ap_force_vec_inert, -1.0);
      M_COPY(chute.twist.input.T_ECI_to_Body, chute.input.pyld_T_inertial_to_body);
      M_COPY(chute.twist.input.T_Body_to_SR, chute.input.pyld_T_body_to_SR);
      
      // check to see if all parachutes have inflated 
      chute.twist.input.chutes_inflated = true;
      for(int i = 0; i < chute.twist.param.num_chutes; i++){
          if( chute.main_inflation[i].output.inflation_complete == 0 ){
                chute.twist.input.chutes_inflated = false;
          }
      }

      if (chute.twist.input.chutes_inflated == true) { // Only overwrite steady state Chute Area once all chutes have inflated

         chute.twist.work.elasped_time = chute.sim_time_local - chute.twist.work.start_time;
         if (chute.twist.work.elasped_time < chute.twist.param.S_transition_time) { 
         //Transition from current chute area to TCD chute area over pre-defined time delta
            chute.twist.work.cycle_count = chute.twist.param.S_transition_time / chute.twist.input.dt; 
            for (int i = 0; i < chute.param.num_chutes; i++){ 
               chute.twist.work.S_step[i] = chute.twist.work.delta_S[i] / chute.twist.work.cycle_count;
                   chute.main_inflation[i].output.Chute_Area = chute.main_inflation[i].output.Chute_Area - chute.twist.work.S_step[i];
            }
         }
         else { 
         // Call  function 
            parachute_twist_torque(&chute.twist);

            for (int i = 0; i < chute.param.num_chutes; i++) {
                    chute.main_inflation[i].output.Chute_Area = 
                   chute.twist.output.CdS / chute.main_inflation[i].param.drag_coeff;
            }
         }
      }
      else { //Calculate Sp at time = 0
         chute.twist.transient.work.t = 0.0; 
         parachute_waveform(&chute.twist.transient);
         chute.twist.work.flyout_angle = chute.twist.transient.work.flyout_angle;
         chute.twist.work.Sp           = chute.twist.transient.work.Sp;
         chute.twist.work.CdSF         = chute.twist.transient.param.Sp_CdSF;
         chute.twist.work.S            = M_PI * ((chute.param.main_diameter * chute.param.main_diameter)) / 4.0;
         chute.twist.output.CdS =
           (chute.param.num_chutes * chute.twist.work.Sp * std::cos(chute.twist.work.flyout_angle)) *
            chute.twist.work.CdSF * chute.twist.work.S;
         chute.twist.work.Chute_Area = chute.twist.output.CdS / chute.main_aero[0].param.nominal_cd;
         chute.twist.work.start_time = chute.sim_time_local; 
         for (int i = 0; i < chute.param.num_chutes; i++){ 
            chute.twist.work.delta_S[i] = chute.main_inflation[i].output.Chute_Area - chute.twist.work.Chute_Area;
         }
      }
   }
   else {
          V_INIT(chute.twist.output.twist_torque_SR);
   }   
}

