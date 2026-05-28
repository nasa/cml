/******************************* TRICK HEADER **********************************
PURPOSE:
    (To determine whether chute lines recontact CM) 

ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))
 
PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (Jan 2011) (Initial Release))             
     ((Jeff Semrau) (Honeywell) (May 2011) (Added new monitoring of 'prime' forces for LRS))             
     ((Jeff Semrau) (Honeywell) (May 2012) (LMBP 1650 - Added new monitoring of extreme riser angles))             
     ((Jeff Semrau) (Honeywell) (Oct 2015) (Added new monitoring data for LRS))             
*******************************************************************************/

/*
 * Include files for package
 */

/* Model includes */
#include "../include/parachute_exec.hh"

/* Trick includes */
#include "trick/trick_math.h"
#include "trick/constant.h"              /* For unit conversions */
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/* Function prototypes */
void parachute_recontact_monitor (double             PyldState_alpha_total,
                                  double             PyldState_alpha,
                                  double             PyldState_beta,
                                  double             PyldState_qbar,
                                  PARACHUTE_DATA    *chute)
                        
{

   if (!chute) {
      CMLMessage::fail(__FILE__,__LINE__,"Information only: \n","pointer is null \n");
      return;
   }

   int i; 

   for (i=0; i < chute->param.num_chutes; i++) {

      if (chute->main_coupling[i].output.load > 0.0) { 
       
         // Rotate Chute Force from CM SR Frame to DSS Frame
         chute->recontact_force[i][0] = -chute->main_coupling[i].output.endpoint1_force_SR[0]; 
         chute->recontact_force[i][1] = chute->main_coupling[i].output.endpoint1_force_SR[1]; 
         chute->recontact_force[i][2] = -chute->main_coupling[i].output.endpoint1_force_SR[2]; 
         
         // Rotate Chute Force from DSS Frame to Fairlead Frame
         MxV(chute->chute_force_in_Fairlead_Frame[i], chute->T_DSS_SR_to_Fairlead, chute->recontact_force[i]);

         // Calculate angles in Fairlead xz and xy planes
         chute->xy_angle[i] = std::atan2(chute->chute_force_in_Fairlead_Frame[i][1], chute->chute_force_in_Fairlead_Frame[i][0]);
         chute->xz_angle[i] = std::atan2(chute->chute_force_in_Fairlead_Frame[i][2], chute->chute_force_in_Fairlead_Frame[i][0]);

         //Quick code to monitor for extreme angles
         if (chute->xy_angle[i] < chute->extreme_xy_angle_left[i]) {
            chute->extreme_xy_angle_left[i] = chute->xy_angle[i];
         }
         if (chute->xy_angle[i] > chute->extreme_xy_angle_right[i]) {
            chute->extreme_xy_angle_right[i] = chute->xy_angle[i];
         }
         if (chute->xz_angle[i] < chute->extreme_xz_angle_lower[i]) {
            chute->extreme_xz_angle_lower[i] = chute->xz_angle[i];
         }
         if (chute->xz_angle[i] > chute->extreme_xz_angle_upper[i]) {
            chute->extreme_xz_angle_upper[i] = chute->xz_angle[i];
         }

         //Store Angle Violations, indicates recontact, for futher review
         if (chute->xy_angle[i] < chute->xy_angle_left_limit) { 
            chute->xy_angle_left_violation[i] = 1;
         }
         else if (chute->xy_angle[i] > chute->xy_angle_right_limit) { 
            chute->xy_angle_right_violation[i] = 1;
         }

         if (chute->xz_angle[i] < chute->xz_angle_lower_limit) { 
            chute->xz_angle_lower_violation[i] = 1;
         }
         else if (chute->xz_angle[i] > chute->xz_angle_upper_limit) { 
            chute->xz_angle_upper_violation[i] = 1;
         }

      }

   }

   // 45 Deg Fairlead Monitoring
   //Setup Transformation Matrix from CM Frame to 45 Deg Fairlead 
   M_INIT(chute->T_45Deg_Fairlead);
   double cos_45_deg, sin_45_deg;  
   cos_45_deg = std::cos(45.0*DTR);
   sin_45_deg = std::sin(45.0*DTR);
   chute->T_45Deg_Fairlead[0][0] = 1.0;
   chute->T_45Deg_Fairlead[1][1] = cos_45_deg;
   chute->T_45Deg_Fairlead[1][2] = -sin_45_deg;
   chute->T_45Deg_Fairlead[2][1] = sin_45_deg;
   chute->T_45Deg_Fairlead[2][2] = cos_45_deg;

   MxV(chute->force_prime, chute->T_45Deg_Fairlead, chute->output.force_on_payload);

   if (chute->force_prime[1] > chute->max_y_force_prime[1]) {
      V_COPY(chute->max_y_force_prime, chute->force_prime);
   }
   else if (chute->force_prime[1] < chute->min_y_force_prime[1]) { 
      V_COPY(chute->min_y_force_prime, chute->force_prime);
   }

   if (chute->force_prime[2] > chute->max_z_force_prime[2]) {
      V_COPY(chute->max_z_force_prime, chute->force_prime);
   }
   else if (chute->force_prime[2] < chute->min_z_force_prime[2]) { 
      V_COPY(chute->min_z_force_prime, chute->force_prime);
   }

   MtxV(chute->max_y_fp_CM, chute->T_45Deg_Fairlead, chute->max_y_force_prime);
   MtxV(chute->min_y_fp_CM, chute->T_45Deg_Fairlead, chute->min_y_force_prime);
   MtxV(chute->max_z_fp_CM, chute->T_45Deg_Fairlead, chute->max_z_force_prime);
   MtxV(chute->min_z_fp_CM, chute->T_45Deg_Fairlead, chute->min_z_force_prime);

   M_INIT(chute->T_45Deg_xz_Fairlead); 
   chute->T_45Deg_xz_Fairlead[0][0] = std::cos(45.0*DTR);
   chute->T_45Deg_xz_Fairlead[0][2] = std::sin(45.0*DTR);
   chute->T_45Deg_xz_Fairlead[1][1] = 1.0;           
   chute->T_45Deg_xz_Fairlead[2][0] = -chute->T_45Deg_xz_Fairlead[0][2];  
   chute->T_45Deg_xz_Fairlead[2][2] = chute->T_45Deg_xz_Fairlead[0][0];

   MxV(chute->force_xz_prime, chute->T_45Deg_xz_Fairlead, chute->output.force_on_payload);

   if (chute->force_xz_prime[0] > chute->max_x_force_xz_prime[0]) {
      V_COPY(chute->max_x_force_xz_prime, chute->force_xz_prime);
   }
   else if (chute->force_xz_prime[0] < chute->min_x_force_xz_prime[0]) { 
      V_COPY(chute->min_x_force_xz_prime, chute->force_xz_prime);
   }

   if (chute->force_xz_prime[2] > chute->max_z_force_xz_prime[2]) {
      V_COPY(chute->max_z_force_xz_prime, chute->force_xz_prime);
   }
   else if (chute->force_xz_prime[2] < chute->min_z_force_xz_prime[2]) { 
      V_COPY(chute->min_z_force_xz_prime, chute->force_xz_prime);
   }

   MtxV(chute->max_x_fp_xz_CM, chute->T_45Deg_xz_Fairlead, chute->max_x_force_xz_prime);
   MtxV(chute->min_x_fp_xz_CM, chute->T_45Deg_xz_Fairlead, chute->min_x_force_xz_prime);
   MtxV(chute->max_z_fp_xz_CM, chute->T_45Deg_xz_Fairlead, chute->max_z_force_xz_prime);
   MtxV(chute->min_z_fp_xz_CM, chute->T_45Deg_xz_Fairlead, chute->min_z_force_xz_prime);


   // Cluster Load Monitoring
   chute->total_payload_force_mag = V_MAG(chute->output.force_on_payload);
   if (chute->total_payload_force_mag > chute->peak_cluster_load_mag) { 
       chute->peak_cluster_load_mag = chute->total_payload_force_mag;
       chute->qbar_at_peak_cluster_load = PyldState_qbar;
   }

   if ((chute->main_inflation[0].work.reef_stage == Inflation_Work::xreef_1) && (chute->relstate.chute[0].output.canopy_extracted==true)) {
      if (chute->total_payload_force_mag > chute->peak_reef1_cluster_load_mag) {
         chute->peak_reef1_cluster_load_mag = chute->total_payload_force_mag;
         chute->qbar_at_peak_reef1_cluster_load = PyldState_qbar;
         V_COPY(chute->peak_reef1_cluster_load_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[0] < chute->min_reef1_cluster_load_x) {
         chute->min_reef1_cluster_load_x = chute->output.force_on_payload[0];
         V_COPY(chute->min_reef1_cluster_load_x_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[0] > chute->max_reef1_cluster_load_x) {
         chute->max_reef1_cluster_load_x = chute->output.force_on_payload[0];
         V_COPY(chute->max_reef1_cluster_load_x_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[1] < chute->min_reef1_cluster_load_y) {
         chute->min_reef1_cluster_load_y = chute->output.force_on_payload[1];
         V_COPY(chute->min_reef1_cluster_load_y_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[1] > chute->max_reef1_cluster_load_y) {
         chute->max_reef1_cluster_load_y = chute->output.force_on_payload[1];
         V_COPY(chute->max_reef1_cluster_load_y_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[2] < chute->min_reef1_cluster_load_z) {
         chute->min_reef1_cluster_load_z = chute->output.force_on_payload[2];
         V_COPY(chute->min_reef1_cluster_load_z_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[2] > chute->max_reef1_cluster_load_z) {
         chute->max_reef1_cluster_load_z = chute->output.force_on_payload[2];
         V_COPY(chute->max_reef1_cluster_load_z_vec, chute->output.force_on_payload);
      }
      if (chute->force_prime[1] < chute->min_reef1_y_force_prime) {
         chute->min_reef1_y_force_prime = chute->force_prime[1];
         V_COPY(chute->min_reef1_y_force_prime_vec, chute->force_prime);  
      }
      if (chute->force_prime[1] > chute->max_reef1_y_force_prime) {
         chute->max_reef1_y_force_prime = chute->force_prime[1];
         V_COPY(chute->max_reef1_y_force_prime_vec, chute->force_prime);  
      }
      if (chute->force_prime[2] < chute->min_reef1_z_force_prime) {
         chute->min_reef1_z_force_prime = chute->force_prime[2];
         V_COPY(chute->min_reef1_z_force_prime_vec, chute->force_prime);  
      }
      if (chute->force_prime[2] > chute->max_reef1_z_force_prime) {
         chute->max_reef1_z_force_prime = chute->force_prime[2];
         V_COPY(chute->max_reef1_z_force_prime_vec, chute->force_prime);  
      }
   }
   else if (chute->main_inflation[0].work.reef_stage == Inflation_Work::xreef_2) {
      if (chute->total_payload_force_mag > chute->peak_reef2_cluster_load_mag) {
         chute->peak_reef2_cluster_load_mag = chute->total_payload_force_mag;
         chute->qbar_at_peak_reef2_cluster_load = PyldState_qbar;
         V_COPY(chute->peak_reef2_cluster_load_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[0] < chute->min_reef2_cluster_load_x) {
         chute->min_reef2_cluster_load_x = chute->output.force_on_payload[0];
         V_COPY(chute->min_reef2_cluster_load_x_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[0] > chute->max_reef2_cluster_load_x) {
         chute->max_reef2_cluster_load_x = chute->output.force_on_payload[0];
         V_COPY(chute->max_reef2_cluster_load_x_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[1] < chute->min_reef2_cluster_load_y) {
         chute->min_reef2_cluster_load_y = chute->output.force_on_payload[1];
         V_COPY(chute->min_reef2_cluster_load_y_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[1] > chute->max_reef2_cluster_load_y) {
         chute->max_reef2_cluster_load_y = chute->output.force_on_payload[1];
         V_COPY(chute->max_reef2_cluster_load_y_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[2] < chute->min_reef2_cluster_load_z) {
         chute->min_reef2_cluster_load_z = chute->output.force_on_payload[2];
         V_COPY(chute->min_reef2_cluster_load_z_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[2] > chute->max_reef2_cluster_load_z) {
         chute->max_reef2_cluster_load_z = chute->output.force_on_payload[2];
         V_COPY(chute->max_reef2_cluster_load_z_vec, chute->output.force_on_payload);
      }
      if (chute->force_prime[1] < chute->min_reef2_y_force_prime) {
         chute->min_reef2_y_force_prime = chute->force_prime[1];
         V_COPY(chute->min_reef2_y_force_prime_vec, chute->force_prime);  
      }
      if (chute->force_prime[1] > chute->max_reef2_y_force_prime) {
         chute->max_reef2_y_force_prime = chute->force_prime[1];
         V_COPY(chute->max_reef2_y_force_prime_vec, chute->force_prime);  
      }
      if (chute->force_prime[2] < chute->min_reef2_z_force_prime) {
         chute->min_reef2_z_force_prime = chute->force_prime[2];
         V_COPY(chute->min_reef2_z_force_prime_vec, chute->force_prime);  
      }
      if (chute->force_prime[2] > chute->max_reef2_z_force_prime) {
         chute->max_reef2_z_force_prime = chute->force_prime[2];
         V_COPY(chute->max_reef2_z_force_prime_vec, chute->force_prime);  
      }
   }
   else {
      if (chute->total_payload_force_mag > chute->peak_disreef_cluster_load_mag) {
         chute->peak_disreef_cluster_load_mag = chute->total_payload_force_mag;
         chute->qbar_at_peak_disreef_cluster_load = PyldState_qbar;
         V_COPY(chute->peak_disreef_cluster_load_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[0] < chute->min_disreef_cluster_load_x) {
         chute->min_disreef_cluster_load_x = chute->output.force_on_payload[0];
         V_COPY(chute->min_disreef_cluster_load_x_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[0] > chute->max_disreef_cluster_load_x) {
         chute->max_disreef_cluster_load_x = chute->output.force_on_payload[0];
         V_COPY(chute->max_disreef_cluster_load_x_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[1] < chute->min_disreef_cluster_load_y) {
         chute->min_disreef_cluster_load_y = chute->output.force_on_payload[1];
         V_COPY(chute->min_disreef_cluster_load_y_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[1] > chute->max_disreef_cluster_load_y) {
         chute->max_disreef_cluster_load_y = chute->output.force_on_payload[1];
         V_COPY(chute->max_disreef_cluster_load_y_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[2] < chute->min_disreef_cluster_load_z) {
         chute->min_disreef_cluster_load_z = chute->output.force_on_payload[2];
         V_COPY(chute->min_disreef_cluster_load_z_vec, chute->output.force_on_payload);
      }
      if (chute->output.force_on_payload[2] > chute->max_disreef_cluster_load_z) {
         chute->max_disreef_cluster_load_z = chute->output.force_on_payload[2];
         V_COPY(chute->max_disreef_cluster_load_z_vec, chute->output.force_on_payload);
      }
      if (chute->force_prime[1] < chute->min_disreef_y_force_prime) {
         chute->min_disreef_y_force_prime = chute->force_prime[1];
         V_COPY(chute->min_disreef_y_force_prime_vec, chute->force_prime);  
      }
      if (chute->force_prime[1] > chute->max_disreef_y_force_prime) {
         chute->max_disreef_y_force_prime = chute->force_prime[1];
         V_COPY(chute->max_disreef_y_force_prime_vec, chute->force_prime);  
      }
      if (chute->force_prime[2] < chute->min_disreef_z_force_prime) {
         chute->min_disreef_z_force_prime = chute->force_prime[2];
         V_COPY(chute->min_disreef_z_force_prime_vec, chute->force_prime);  
      }
      if (chute->force_prime[2] > chute->max_disreef_z_force_prime) {
         chute->max_disreef_z_force_prime = chute->force_prime[2];
         V_COPY(chute->max_disreef_z_force_prime_vec, chute->force_prime);  
      }
   }
   if (PyldState_alpha < 0.0) {
      chute->alpha_0to360 = PyldState_alpha + 360.0;
   }
   else {
      chute->alpha_0to360 = PyldState_alpha;
   }

   if (chute->alpha_0to360 < chute->min_alpha) {
      chute->min_alpha = chute->alpha_0to360;
      chute->min_alphas_beta = PyldState_beta;
   }
   if (chute->alpha_0to360 > chute->max_alpha) {
      chute->max_alpha = chute->alpha_0to360;
      chute->max_alphas_beta = PyldState_beta;
   }
   if (PyldState_beta < chute->min_beta) {
      chute->min_beta = PyldState_beta;
      chute->min_betas_alpha = PyldState_alpha;
   }
   if (chute->max_beta < PyldState_beta) {
      chute->max_beta = PyldState_beta;
      chute->max_betas_alpha = PyldState_alpha;
   }
   if (PyldState_alpha_total < chute->min_alpha_total) {
       chute->min_alpha_total = PyldState_alpha_total;
   }

   //Compute load sharing amongst chutes
   //Compute total load of all chutes in system
   chute->total_load = 0.0;
   for (i = 0; i < chute->param.num_chutes; i++) {
      chute->total_load = chute->total_load + chute->main_coupling[i].output.load;
   }
   if (chute->total_load > 0.0) { //Check to avoid divide by 0
      for (i = 0; i < chute->param.num_chutes; i++) {
         chute->load_share[i] = chute->main_coupling[i].output.load/chute->total_load;
      }
   }

   //Individual Chute Load Monitoring 
   for (i = 0; i < chute->param.num_chutes; i++) { 

      if ((chute->main_coupling[i].output.load > chute->peak_load_mag[i]) && (chute->relstate.chute[i].output.canopy_extracted==true)) { 
         chute->peak_load_mag[i] = chute->current_load_mag[i];
         chute->qbar_at_peak_load[i] = PyldState_qbar;
      }

      if ((chute->param.pilot_chute_active == true) && (chute->pilot_main_relstate.chute[i].output.canopy_extracted==true)) {
         if (chute->pilot_coupling[i].output.load > chute->pilot_peak_load[i]) {
            chute->pilot_peak_load[i] = chute->pilot_coupling[i].output.load;
            chute->qbar_at_pilot_peak_load[i] = PyldState_qbar;
         }
      }

      //Reef Stage Load Monitoring
      if ((chute->main_inflation[i].work.reef_stage == Inflation_Work::xreef_1) && (chute->relstate.chute[i].output.canopy_extracted==true)) {
         if (chute->main_coupling[i].output.load > chute->peak_reef1_max_load[i]) {
             chute->peak_reef1_max_load[i] = chute->main_coupling[i].output.load;
             chute->qbar_at_peak_reef1_max_load[i] = PyldState_qbar;
         }
      }
      else if (chute->main_inflation[i].work.reef_stage == Inflation_Work::xreef_2) {
         if (chute->main_coupling[i].output.load > chute->peak_reef2_max_load[i]) {
             chute->peak_reef2_max_load[i] = chute->main_coupling[i].output.load;
             chute->qbar_at_peak_reef2_max_load[i] = PyldState_qbar;
         }
      }
      else if (chute->main_inflation[i].work.reef_stage == Inflation_Work::xfull) {
         if (chute->main_coupling[i].output.load > chute->peak_full_max_load[i]) {
             chute->peak_full_max_load[i] = chute->main_coupling[i].output.load;
             V_COPY(chute->peak_full_load_vector[i], chute->main_coupling[i].output.endpoint1_force_SR);
             chute->qbar_at_full_max_load[i] = PyldState_qbar;
         }
      }
   }
}
