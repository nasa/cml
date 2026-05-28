/******************************* TRICK HEADER **********************************
PURPOSE:
    (Initialization of the high fidelity parachute model) 

PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (8-July-2009) (Initial Release))           
     ((Jeff Semrau) (Honeywell) (May 2010)    (Osiris CR 763))             
     ((Jeff Semrau) (Honeywell) (Nov 2011)    (lmbp1281, initialize structure_to_Inertial matrix))          
     ((Jeff Semrau) (Honeywell) (Oct 2012)    (lmbp2122, minor correction to computation of initial CF/chute position))          
     ((Jeff Semrau) (Honeywell) (Oct 2015)    (CR4609, updates for new integration method))          
     ((Jeff Semrau) (LM)        (Jan 2018)    (CR12242, fix to prevent bug in ITL from crashing the sim))          
*******************************************************************************/

/*
 * Include files for package
 */

#include "trick/trick_math.h"
#include "../include/parachute_exec.hh"

void parachute_exec_init (
                          const int              mortar_fired,
                          PARACHUTE_DATA&        chute)
{

   double chute_pos_rel_cg[3], chute_pos_body[3], chute_pos_inert[3];
   double veh_rate_inert[3];
   double chute_wxr_vel_inert[3];
   double pyld_Q_inertial_to_body[4];


   /* OrionSim needs to support individual pyro inputs.  To do this, we've added a
      indiv_mortar_fired array which will be driven by OrionSim directly.  For
      backwards compatibility, if the top-level mortar_fired signal gets set, we
      will set the lower level flags.  Supporting separate initiation also means
      we have to handle chute state initialization on an individual basis as well.
      so the parachute_init and init_complete flags have been converted to arrays */

   // Handle single input
   if ( mortar_fired == 1 ) {
      for (int i=0; i < chute.param.num_chutes ; i++) {
         chute.parachute_init[i] = true ;
         chute.mortar_fired[i] = true;
      }
   }

   // Handle individual inputs
   for (int i=0; i < chute.param.num_chutes ; i++) {
      if ( chute.parachute_init[i] ) {
         // If any one mortar has fired, init conflpnt
         chute.conflpnt_init = true ;
      }
   }


   /* Copy the quaternion scalar and vector into a 4 vector */
   pyld_Q_inertial_to_body[0] = chute.input.EI_pyld_Q_inertial_to_body_scalar;
   for(int i=0; i<3; i++) {
      pyld_Q_inertial_to_body[i+1] = chute.input.EI_pyld_Q_inertial_to_body_vector[i];
   }

   if ((chute.conflpnt_init == true) && (chute.conflpnt_init_complete == false)) {

      double cp_pos_rel_cg[3], cp_pos_body[3], cp_pos_inert[3];

      /* Compute Confluence Point (CP) Inertial Position */
      // First compute CP position relative to CG in SR frame
      V_SUB(cp_pos_rel_cg, chute.param.confluence_pt_pos_SR, chute.input.EI_cg);
       
      // Next compute position of CP relative to CG in inertial frame
      MtxV(cp_pos_body, chute.input.EI_pyld_T_body_to_SR, cp_pos_rel_cg);
      MtxV(cp_pos_inert, chute.input.EI_pyld_T_inertial_to_body, cp_pos_body);
   
      // Now we can compute CP inertial position 
      V_ADD(chute.eom_confpt.output.inertial_pos, chute.input.EI_pyld_inertial_pos, cp_pos_inert);

      /* Compute CP Inertial Velocity */
      // Compute rotational velocity of CP 
      double veh_wxr_vel_inert[3];
      MtxV(veh_rate_inert, chute.input.EI_pyld_T_inertial_to_body, chute.input.EI_pyld_inertial_rate);
      V_CROSS(veh_wxr_vel_inert, veh_rate_inert, cp_pos_inert);

      // Now we can compute CP inertial velocity 
      V_ADD(chute.eom_confpt.output.inertial_vel, veh_wxr_vel_inert, chute.input.EI_pyld_inertial_vel);
      M_TRANS(chute.eom_confpt.output.T_body_to_inertial, chute.input.EI_pyld_T_inertial_to_body);                   

      chute.conflpnt_init = false ;
      chute.conflpnt_init_complete = true ;

   } 

   //Now initialize each chute's inertial position, velcity, inertial to body transformation, etc. 
   for (int i = 0; i < chute.param.num_chutes; i++) {

      if ((chute.parachute_init[i] == true) && (chute.init_complete[i] == false)) {

         //Check if skipping a stage, if so populate inflation data structure with skip data
         if ((chute.skip_stage1 == true) || (chute.skip_stage2 == true)){
            if (i == chute.param.skipped_chute) {
               for (int j = 0; j < chute.main_inflation[i].param.num_reef_stages; j++) {
                  chute.main_inflation[i].param.CDS[j]                    = chute.main_inflation_skip_data[i].param.CDS[j];
                  chute.main_inflation[i].param.fill_constant[j]          = chute.main_inflation_skip_data[i].param.fill_constant[j];
                  chute.main_inflation[i].param.overinflation_factor[j]   = chute.main_inflation_skip_data[i].param.overinflation_factor[j];
                  if ( chute.main_inflation[i].param.overinflation_factor[j] < 1.0) chute.main_inflation[i].param.overinflation_factor[j] = 1.0;
                  chute.main_inflation[i].param.inflation_factor[j]       = chute.main_inflation_skip_data[i].param.inflation_factor[j];
                  chute.main_inflation[i].param.transition_time_factor[j] = chute.main_inflation_skip_data[i].param.transition_time_factor[j];
                  chute.main_inflation[i].param.deflation_time[j] = chute.main_inflation_skip_data[i].param.deflation_time[j];
               }
            }
            else {
               for (int j = 0; j < chute.main_inflation[i].param.num_reef_stages; j++) {
                  chute.main_inflation[i].param.CDS[j]                    = chute.main_inflation_lag_data[i].param.CDS[j];
                  chute.main_inflation[i].param.fill_constant[j]          = chute.main_inflation_lag_data[i].param.fill_constant[j];
                  chute.main_inflation[i].param.overinflation_factor[j]   = chute.main_inflation_lag_data[i].param.overinflation_factor[j];
                  if ( chute.main_inflation[i].param.overinflation_factor[j] < 1.0) chute.main_inflation[i].param.overinflation_factor[j] = 1.0;
                  chute.main_inflation[i].param.inflation_factor[j]       = chute.main_inflation_lag_data[i].param.inflation_factor[j];
                  chute.main_inflation[i].param.transition_time_factor[j] = chute.main_inflation_lag_data[i].param.transition_time_factor[j];
                  chute.main_inflation[i].param.deflation_time[j] = chute.main_inflation_lag_data[i].param.deflation_time[j];
               }
            }
         }

         /* Initialize the Main Chute rotation matrix to that of the payload */
         Q_COPY(chute.eom_main[i].work.quat_inertial_to_body, pyld_Q_inertial_to_body);
         M_TRANS(chute.eom_main[i].output.T_body_to_inertial, chute.input.EI_pyld_T_inertial_to_body);                   
         M_COPY(chute.eom_main[i].output.T_inertial_to_body, chute.input.EI_pyld_T_inertial_to_body);
         M_COPY(chute.eom_main[i].output.T_struct_to_inertial, chute.eom_main[i].output.T_body_to_inertial);
         mat_to_quat(chute.eom_main[i].work.quat_inertial_to_body, chute.eom_main[i].output.T_inertial_to_body);

         /* Initialize the Pilot Chute rotation matrix to that of the payload */
         Q_COPY(chute.eom_pilot[i].work.quat_inertial_to_body, pyld_Q_inertial_to_body);
         M_TRANS(chute.eom_pilot[i].output.T_body_to_inertial, chute.input.EI_pyld_T_inertial_to_body);                   
         M_COPY(chute.eom_pilot[i].output.T_inertial_to_body, chute.input.EI_pyld_T_inertial_to_body);
         M_COPY(chute.eom_pilot[i].output.T_struct_to_inertial, chute.eom_pilot[i].output.T_body_to_inertial);
         mat_to_quat(chute.eom_pilot[i].work.quat_inertial_to_body, chute.eom_main[i].output.T_inertial_to_body);

         /* Compute Chute Inertial Position */
         // First compute chute position relative to CG in SR frame
         V_SUB(chute_pos_rel_cg, chute.param.chute_pos_SR[i], chute.input.EI_cg);

         // Next compute position of Chute relative to CG in inertial frame
         MtxV(chute_pos_body, chute.input.EI_pyld_T_body_to_SR, chute_pos_rel_cg);
         MtxV(chute_pos_inert, chute.input.EI_pyld_T_inertial_to_body, chute_pos_body);

         // Now we can compute Chute inertial position 
         V_ADD(chute.eom_main[i].output.inertial_pos, chute.input.EI_pyld_inertial_pos, chute_pos_inert);

         // For simplicity, equating pilot inertial pos to main inertial pos
         V_COPY(chute.eom_pilot[i].output.inertial_pos, chute.eom_main[i].output.inertial_pos);

         /* Compute Chute Inertial Velocity */
         // Compute rotational velocity of Chute 
         MtxV(veh_rate_inert, chute.input.EI_pyld_T_inertial_to_body, chute.input.EI_pyld_inertial_rate);
         V_CROSS(chute_wxr_vel_inert, veh_rate_inert, chute_pos_inert);

         // Now we can compute Chute inertial velocity 
         V_ADD(chute.eom_main[i].output.inertial_vel, chute_wxr_vel_inert, chute.input.EI_pyld_inertial_vel);

         // For simplicity, equating pilot inertial vel to main inertial vel
         V_COPY(chute.eom_pilot[i].output.inertial_vel, chute.eom_main[i].output.inertial_vel);

         // Code to init main and pilot mass properties model, avoids initialization spike in mass props model. 
         chute.main_massprops[i].work.prev_parachute_mass_x    = chute.main_massprops[i].param.parachute_dry_mass;
         chute.main_massprops[i].work.prev_parachute_mass_y    = chute.main_massprops[i].param.parachute_dry_mass;
         chute.main_massprops[i].work.prev_parachute_mass_z    = chute.main_massprops[i].param.parachute_dry_mass;
         chute.main_massprops[i].work.prev_normalized_diameter = 0.0;

         chute.pilot_massprops[i].work.prev_parachute_mass_x    = chute.pilot_massprops[i].param.parachute_dry_mass;
         chute.pilot_massprops[i].work.prev_parachute_mass_y    = chute.pilot_massprops[i].param.parachute_dry_mass;
         chute.pilot_massprops[i].work.prev_parachute_mass_z    = chute.pilot_massprops[i].param.parachute_dry_mass;
         chute.pilot_massprops[i].work.prev_normalized_diameter = 0.0;

         // Init the chute mass output vector from mass props
         for (int j = 0; j < 3; j++) {
            chute.main_massprops[i].output.chute_mass[j]  = chute.main_massprops[i].param.parachute_dry_mass;
            chute.pilot_massprops[i].output.chute_mass[j] = chute.pilot_massprops[i].param.parachute_dry_mass;
         }

         /* Reset Reefing Times based on CPAS MMv14 'time_transition_factor' */
         for (int k = 0; k < chute.main_inflation[i].param.num_reef_stages; k++) {
            chute.main_inflation[i].param.transition_time[k] =
               chute.main_inflation[i].param.transition_time[k] * (1.0 + chute.main_inflation[i].param.transition_time_factor[k]);
         }

         chute.parachute_init[i] = false;
         chute.init_complete[i] = true;
         chute.chutes_deployed = true;
      }

   }

} 

