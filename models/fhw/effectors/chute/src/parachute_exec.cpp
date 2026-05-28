/******************************* TRICK HEADER **********************************

PURPOSE:
    (The main executive of the high fidelity parachute model) 
REFERENCE:
    ((  -- ))                                          
ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))
LIBRARY DEPENDENCY:
   ((parachute_exec.o)
    (parachute_eom.o)
    (parachute_phasing.o)
    (parachute_twist_exec.o)
    (parachute_loads.o)
    (parachute_att_pnt_damping.o)
    (parachute_state.o)   
    (parachute_recontact_monitor.o)
   )


PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (May 2010) (Osiris Release 763))          
     ((Jeff Semrau) (Honeywell) (June 2010) (DR 806) (Add check of new flag to compute debug data))
     ((Jeff Semrau) (Honeywell) (Aug 2010) (DR 824) (Only calling CF EOM if a CF is being modeled))
     ((Jeff Semrau) (Honeywell) (Jan 2011) (CR 859) (New monitoring code, GRAM I/F updates))
     ((Jeff Semrau) (Honeywell) (May 2011) (lmbp683) (New Twist Torque model updates))
     ((Jeff Semrau) (Honeywell) (Nov 2011) (lmbp1281)(Correction to apply gravity on chute during deploy))
     ((Jeff Semrau) (Honeywell) (02/2012)  (lmbp1395) (Chute Instantaneous Cd (ICD)))
     ((Jeff Semrau) (Honeywell) (05/2012)  (lmbp1650) (LRS MMv9 Updates))
     ((Jeff Semrau) (Honeywell) (10/2012)  (lmbp2122) (Updated I/F to take in PEC to cut chutes))
     ((Jeff Semrau) (Honeywell) (01/2013)  (lmbp2290) (LRS MMv11 Updates))
     ((Jeff Semrau) (Honeywell) (01/2013)  (lmbp2720) (Implemented Time Varying Cd (TCD) ))
     ((Jeff Semrau) (Honeywell) (11/2013)  (lmbp3148) (New code to randomly fail a chute and improved CF modeling))
     ((Jeff Semrau) (Honeywell) (04/2015)  (CR2673) (Update twist torque for textile risers))
     ((Jeff Semrau) (Honeywell) (11/2015)  (CR4609) (Updates for new integration method))
     ((Jeff Semrau) (Honeywell) (08/2017)  (CR11107) (Updates to accommodate sims with multiple mortar PECs))

*******************************************************************************/
/*
 * Include files for package
 */

#include "trick/constant.h"              /* For unit conversions */
#include "trick/exec_proto.h"            /* For get_job_cycle routine */
#include "trick/trick_math.h"
#include "../include/parachute_exec.hh"

int  parachute_exec (const int                  chute_cut, 
                     PARACHUTE_DATA&            chute, 
                     PARACHUTE_DATA&            droguechute)
{
   int    max_index_value;
   double cp_T_SR_to_inertial[3][3];
   double cp_T_inertial_to_SR[3][3];
   double confpt_total_force_inertial[3];

   double pyld_angle_of_attack, pyld_angle_of_attack_total, pyld_sideslip;
   pyld_angle_of_attack         =  chute.input.pyld_angle_of_attack_in * RTD;
   pyld_angle_of_attack_total   =  chute.input.pyld_angle_of_attack_total_in * RTD;
   pyld_sideslip                =  chute.input.pyld_sideslip_in * RTD;

/* Check Model Active flag then proceed with model */
   if(!chute.param.hifi_model_active_flag ) return(0); 

   if (chute.param.fail_random_mortar == true) {
      chute.mortar[chute.param.failed_chute].param.force_uncertainty = 0.0;
      chute.param.fail_random_chute = true;
   }

   if (chute.param.drogue_deployed_mains == true) {
      M_COPY(chute.temp_T_b_to_i, droguechute.eom_main[0].output.T_body_to_inertial);
      M_COPY(chute.temp_T_i_to_b, droguechute.eom_main[0].output.T_inertial_to_body);
   }

   // Check if chutes have been deployed, if not exit routine. 
   if (chute.chutes_deployed == false) {
      return(0);
   }

   //Code to accommodate sims with multiple PECS
   for (int i = 0 ; i < chute.param.num_chutes ; i++) {
      if ( chute.parachute_init[i] == 1 ) { //chute.mortar_fired = 1;
         chute.mortar_fired[i] = true;
      }
   }

   // Handle gobal chute_cut flag for single signal interface
   // The actual vehicle has separate cutters for each riser, 
   // need this for OrionSim
   if ( chute_cut == 1 ) {
      for (int i = 0 ; i < chute.param.num_chutes ; i++) {
         chute.chute_released[i] = true ;
      }
   }

   // Zero outputs from model if all chutes have been released
   chute.all_chutes_released = true;
   for (int i = 0 ; i < chute.param.num_chutes ; i++) {
      if ( chute.chute_released[i] == false ) {
         // At least one chute not released
         chute.all_chutes_released = false ;
         break ;
      }
   }
   if (chute.all_chutes_released) {
      V_INIT(chute.output.force_on_payload);
      V_INIT(chute.output.moment_on_payload);
      for (int i=0; i < chute.param.num_chutes; i++) {
          chute.main_coupling[i].output.load = 0.0;
      }
      chute.total_payload_force_mag = 0.0;
      return(0);
   }

   //Use get_job_cycle to get model cycle time
   chute.dt = exec_get_job_cycle(0);

   if (chute.param.fail_random_mortar == true) {
     chute.mortar[chute.param.failed_chute].param.force_uncertainty = 0.0;
     chute.param.fail_random_chute = true;
   }

   if ((chute.param.fail_random_chute == true) && (chute.param.reset_att_pnts_enabled == true)) {
      for (int i = 0; i < chute.param.num_chutes; i++) {
         V_COPY(chute.relstate.pyld.param.pyld_att_pnt_sr[i], chute.param.new_pyld_att_pnt_sr[i]); 
      }
   }

///////////////////////////////////////////////////////////////////////////////
//        RELSTATE                                                           //
///////////////////////////////////////////////////////////////////////////////

   parachute_state ( chute );        

///////////////////////////////////////////////////////////////////////////////
//        LOADS                                                              //
///////////////////////////////////////////////////////////////////////////////

   //Now calculate forces/moments acting on payload 
   //First init the output vectors to zero 
   V_INIT(chute.output.force_on_payload);
   V_INIT(chute.output.moment_on_payload);
   V_INIT(chute.output.total_mortar_force);
   V_INIT(chute.output.total_mortar_moment);

   parachute_loads(chute);

///////////////////////////////////////////////////////////////////////////////
//       EOM OF CONFLUENCE POINTS                                            //
///////////////////////////////////////////////////////////////////////////////

   //Code to populate eom_confpt input data. 
   chute.eom_confpt.input.rotational_eom_enabled = false; 
   chute.eom_confpt.input.modeling_chute         = false; 
   chute.eom_confpt.input.mass                   = chute.param.confpt_mass;
   V_COPY(chute.eom_confpt.input.aero_force_SR, chute.confpt_aero.output.force_SR);
   V_COPY(chute.eom_confpt.input.gravaccel, chute.input.gravaccel);  
   M_TRANS(chute.eom_confpt.output.T_body_to_inertial, chute.input.pyld_T_inertial_to_body);           

   //Check to see if any of the harness lines are taut
   for (int i = 0; i < chute.param.num_harness_lines; i++) {
      if (chute.relstate.pyld.output.harness_line_stretch[i]) {
         chute.harness_line_tight = true;
      }
   }

   //If any harness line is taut, reset confpt velocity to payload velocity
   if ((chute.confpt_vel_reset == false) && (chute.harness_line_tight)) {
      V_COPY(chute.eom_confpt.output.inertial_vel, chute.input.pyld_inertial_vel);
      chute.confpt_vel_reset = true;
   }

   //Need to Compute transformation matrix from each chute SR frame to Inertial Frame
   MxM(cp_T_SR_to_inertial, chute.eom_confpt.output.T_body_to_inertial, chute.param.T_SR_to_body);
   M_TRANS(cp_T_inertial_to_SR, cp_T_SR_to_inertial);

   //Loop code to sum up chute riser and harness forces acting on confluence pt into one overall force, 
   //this is then input to the confluence pt EOM as the 'riser' force. This is keep the input structure 
   //the same for all bodies. 

   if(chute.param.model_confl_point == true) {

      //Initialize to 0 the vector used to compute total harness/riser line forces acting on confluence pt
      V_INIT(confpt_total_force_inertial);

     //Different force summation code for cases where number of confluence pts is > 1
      
      /* Sum Up Riser Line Forces acting on Confluence Point */
      for (int i = 0; i < chute.param.num_chutes; i++) {
         for (int j = 0; j < 3; j++) {
            confpt_total_force_inertial[j] =  
               confpt_total_force_inertial[j] + chute.main_coupling[i].work.endpoint1_force_eci[j];                      
         }
      }

      /* Sum Up Harness Line Forces acting on Confluence Point */
      for (int i = 0; i < chute.param.num_harness_lines; i++) {
         for (int j = 0; j < 3; j++) {
            confpt_total_force_inertial[j] = 
               confpt_total_force_inertial[j] + chute.harness_coupling[i].work.endpoint2_force_eci[j];
         }
      }

      // Add the confluence damping force, but not before the confluence is fully deployed
      if (chute.harness_line_tight) {
          V_INCR(confpt_total_force_inertial, chute.conf_damping.damping_force_inert);
      }

      /* Rotate total riser/harness line force from Inertial frame to confluence point SR frame, 
         and pass as input to confluence pt EOM */
      MxV(chute.eom_confpt.input.riser_force_SR, cp_T_inertial_to_SR, confpt_total_force_inertial);

      /* For now, just apply aero, riser and gravity to confluence pt. 
       May want to in the future apply a mortar force to confluence point. 
       Code is here, but commented out for future reference */
      //V_COPY(chute.eom_confpt.input.mortar_force_SR, chute.mortar[0].output.mortar_forces);

      M_COPY(chute.eom_confpt.input.T_SR_to_body, chute.param.T_SR_to_body);

      /* Compute EOM of confluence pt, the confluence pt is 3DOF */
      parachute_eom ( &chute.eom_confpt); 

   } //End of if state for number of confluence points

///////////////////////////////////////////////////////////////////////////////
//       EOM OF CHUTES                                                       //
///////////////////////////////////////////////////////////////////////////////

   if (chute.param.pilot_chute_active == true) {   //Will need to compute EOMs of Pilots & Mains
       
      //Compute EOM for Pilots
      for (int i = 0; i < chute.param.num_chutes; i++) {

         //Skip this iteration if current chute has not finished initialization
         if (chute.init_complete[i] == false) {
            continue;
         }

         //Code to populate eom_chute input data for pilots. 
         chute.eom_pilot[i].input.modeling_chute         = true;
         V_COPY(chute.eom_pilot[i].input.chute_mass, chute.pilot_massprops[i].output.chute_mass);
         V_COPY(chute.eom_pilot[i].input.aero_force_SR, chute.pilot_aero[i].output.force_SR);                     
         V_COPY(chute.eom_pilot[i].input.aero_moment_SR, chute.pilot_aero[i].output.moment_SR);                     
         V_COPY(chute.eom_pilot[i].input.riser_force_SR, chute.pilot_coupling[i].output.endpoint2_force_SR);                     
         V_COPY(chute.eom_pilot[i].input.riser_moment_SR, chute.pilot_coupling[i].output.endpoint2_moment_SR);                     
         V_COPY(chute.eom_pilot[i].input.apparent_mass_accel, chute.pilot_massprops[i].output.mass_accel);                         
         V_COPY(chute.eom_pilot[i].input.gravaccel, chute.input.gravaccel);                         

         M_COPY(chute.eom_pilot[i].input.inertia, chute.pilot_massprops[i].output.inertia);
         M_COPY(chute.eom_pilot[i].input.T_SR_to_body, chute.param.T_SR_to_body);

         chute.eom_pilot[i].input.chute_dry_mass = chute.pilot_massprops[i].param.parachute_dry_mass;

         // Always running pilots as 6-DOF bodies
         chute.eom_pilot[i].input.rotational_eom_enabled = true;  

         //Turn off rotational EOM of pilot after main line stretch, aviods potential FPE in sim
         if (chute.chute_phase[i] >= PARACHUTE_DATA::main_inflating) {
            chute.eom_pilot[i].input.rotational_eom_enabled = false; 
         }

         //Compute EOMs of chutes, each chute is handled internally in the model
         parachute_eom ( &chute.eom_pilot[i]); 

      }
    
      //Compute EOM for Mains
      for (int i = 0; i < chute.param.num_chutes; i++) {

         //If past main deployment, zero out pilot F&M so it won't effect main canopy
         if (chute.chute_phase[i] >= PARACHUTE_DATA::main_inflating) { 
            V_INIT(chute.pilot_coupling[i].output.endpoint1_force_SR);
            V_INIT(chute.pilot_coupling[i].output.endpoint1_moment_SR);
         }

         //Code to populate eom_chute input data for pilots. 
         chute.eom_main[i].input.modeling_chute         = true;
         V_COPY(chute.eom_main[i].input.chute_mass, chute.main_massprops[i].output.chute_mass);
         V_COPY(chute.eom_main[i].input.aero_force_SR, chute.main_aero[i].output.force_SR);                     
         V_COPY(chute.eom_main[i].input.aero_moment_SR, chute.main_aero[i].output.moment_SR);                     
         V_COPY(chute.eom_main[i].input.riser_force_SR, chute.main_coupling[i].output.endpoint2_force_SR);                     
         V_COPY(chute.eom_main[i].input.riser_moment_SR, chute.main_coupling[i].output.endpoint2_moment_SR);                     
         V_COPY(chute.eom_main[i].input.pilot_force_SR, chute.pilot_coupling[i].output.endpoint1_force_SR);                     
         V_COPY(chute.eom_main[i].input.pilot_moment_SR, chute.pilot_coupling[i].output.endpoint1_moment_SR);                     
         V_COPY(chute.eom_main[i].input.apparent_mass_accel, chute.main_massprops[i].output.mass_accel);                         
         V_COPY(chute.eom_main[i].input.gravaccel, chute.input.gravaccel);                         
         M_COPY(chute.eom_main[i].input.inertia, chute.main_massprops[i].output.inertia);
         M_COPY(chute.eom_main[i].input.T_SR_to_body, chute.param.T_SR_to_body);
         chute.eom_main[i].input.chute_dry_mass = chute.main_massprops[i].param.parachute_dry_mass;

         if (chute.relstate.chute[i].output.riser_susp_line_stretch == true) { 
            chute.eom_main[i].input.rotational_eom_enabled = true;  
         }
         else {
            chute.eom_main[i].input.rotational_eom_enabled = false; 
         }

         //Only apply gravity to chute bag after it is deployed
         if (chute.chute_phase[i] <  PARACHUTE_DATA::main_deployment) { 
            V_INIT(chute.eom_main[i].input.gravaccel);
         }
         //Compute EOMs of chutes, each chute is handled internally in the model
         parachute_eom ( &chute.eom_main[i]); 

         // If main bag has not be extracted, hold main bag at its stowed location until extraction
         if (chute.main_bag_release[i] == false) {
            double chute_pos_rel_cg[3], chute_pos_body[3], chute_pos_inert[3];

            /* Compute Chute Inertial Position */
            // First compute chute position relative to CG in SR frame
            V_SUB(chute_pos_rel_cg, chute.param.chute_pos_SR[i], chute.input.cg);

            // Next compute position of Chute relative to CG in inertial frame
            MtxV(chute_pos_body, chute.input.pyld_T_body_to_SR, chute_pos_rel_cg);
            MtxV(chute_pos_inert, chute.input.pyld_T_inertial_to_body, chute_pos_body);

            // Now we can compute Chute inertial position
            V_ADD(chute.eom_main[i].output.inertial_pos, chute.input.pyld_inertial_pos, chute_pos_inert);

            //Equate Main Bag inertial velocity to Pyld inertial velocity
            V_COPY(chute.eom_main[i].output.inertial_vel, chute.input.pyld_inertial_vel);
         }
      }
   }
   else { // Just need to compute EOM of mains
      //Compute EOM for Mains
      for (int i = 0; i < chute.param.num_chutes; i++) {

         //Skip this iteration if current chute has not finished initialization
         if (chute.init_complete[i] == false) {
            continue; 
         }

         if ((chute.param.drogue_deployed_mains == true) && (chute.relstate.chute[i].output.chute_line_stretch == 0)) {
            //Drogue force is in Pyld SR frame, needs to be rotated to Chute SR frame
            MxV(chute.drogue_force, chute.param.T_pyldSR_to_chuteSR, droguechute.main_coupling[i].output.endpoint1_force_SR);
         }
         else {
            V_INIT(chute.drogue_force);
         }

         //Code to populate eom_chute input data for mains. 
         chute.eom_main[i].input.modeling_chute         = true;
         V_COPY(chute.eom_main[i].input.pilot_force_SR, chute.drogue_force);                     
         V_COPY(chute.eom_main[i].input.chute_mass, chute.main_massprops[i].output.chute_mass);
         V_COPY(chute.eom_main[i].input.aero_force_SR, chute.main_aero[i].output.force_SR);                     
         V_COPY(chute.eom_main[i].input.aero_moment_SR, chute.main_aero[i].output.moment_SR);                     
         V_COPY(chute.eom_main[i].input.riser_force_SR, chute.main_coupling[i].output.endpoint2_force_SR);                     
         V_COPY(chute.eom_main[i].input.riser_moment_SR, chute.main_coupling[i].output.endpoint2_moment_SR);                     
         V_COPY(chute.eom_main[i].input.apparent_mass_accel, chute.main_massprops[i].output.mass_accel);                         
         V_COPY(chute.eom_main[i].input.gravaccel, chute.input.gravaccel);                         
         M_COPY(chute.eom_main[i].input.inertia, chute.main_massprops[i].output.inertia);
         M_COPY(chute.eom_main[i].input.T_SR_to_body, chute.param.T_SR_to_body);
         chute.eom_main[i].input.chute_dry_mass = chute.main_massprops[i].param.parachute_dry_mass;

         // Always running drogues as 6-DOF bodies
         chute.eom_main[i].input.rotational_eom_enabled = true; 

         //Only apply gravity to chute bag after it is deployed
         if (chute.chute_phase[i] <  PARACHUTE_DATA::main_deployment) { 
            V_INIT(chute.eom_main[i].input.gravaccel);
         }

         //Compute EOMs of chutes, each chute is handled internally in the model
         parachute_eom ( &chute.eom_main[i]); 
      }
   }

   // Compute Effective Position and Area of cluster as a single chute and compute PRF
   chute.cluster.chute_area = 0.0;
   V_INIT(chute.cluster.inertial_pos);
   V_INIT(chute.cluster.inertial_vel);

   for (int i=0; i < chute.param.num_chutes; i++) {

      if (chute.param.pilot_chute_active == true) {   
         // Sum up each chute area to compute cluster area
         chute.cluster.chute_area = chute.cluster.chute_area + chute.pilot_inflation[i].output.Chute_Area;

         //Compute the sum of inertial postitions 
         V_ADD(chute.cluster.inertial_pos, chute.cluster.inertial_pos, chute.eom_pilot[i].output.inertial_pos);

         //Compute the sum of inertial velocities 
         V_ADD(chute.cluster.inertial_vel, chute.cluster.inertial_vel, chute.eom_pilot[i].output.inertial_vel);
      } 
      else { 
         // Sum up each chute area to compute cluster area
         chute.cluster.chute_area = chute.cluster.chute_area + chute.main_inflation[i].output.Chute_Area;

         //Compute the sum of inertial postitions 
         V_ADD(chute.cluster.inertial_pos, chute.cluster.inertial_pos, chute.eom_main[i].output.inertial_pos);

         //Compute the sum of inertial velocities 
         V_ADD(chute.cluster.inertial_vel, chute.cluster.inertial_vel, chute.eom_main[i].output.inertial_vel);
      }
   }

   // Compute average inertial position of chute
   V_SCALE(chute.cluster.inertial_pos, chute.cluster.inertial_pos, 1/chute.param.num_chutes);

   // Compute average inertial velocity of chute
   V_SCALE(chute.cluster.inertial_vel, chute.cluster.inertial_vel, 1/chute.param.num_chutes);

   // Set PRF Values for aero model
   for (int i=0; i < chute.param.num_chutes; i++) {
      if (chute.param.pilot_chute_active == true) {   
         if (chute.chute_phase[i] == PARACHUTE_DATA::pilot_free) { 
             chute.pilot_aero[i].param.PRF = 1.0; 
         }
         else {
         chute.pilot_aero[i].param.PRF = chute.wake_effects.prf_model.PRF;
         }
         chute.main_aero[i].param.PRF = 1.0;                                 
      }
      else { 
         chute.main_aero[i].param.PRF = chute.wake_effects.prf_model.PRF;
      } 
   }

///////////////////////////////////////////////////////////////////////////////
// Attach Point Damping                                                      //
///////////////////////////////////////////////////////////////////////////////

   if (chute.param.apd_damping == true) { 

      for (int i=0; i < chute.param.num_chutes; i++) {
         V_COPY(chute.apd[i].input.pyld_att_pnt_moment_arm, chute.relstate.pyld.output.pyld_att_pnt_moment_arm[i]);
         V_COPY(chute.apd[i].input.pyld_body_rates, chute.input.pyld_inertial_rate);
         V_COPY(chute.apd[i].input.VairB, chute.input.pyld_VairB);
         chute.apd[i].input.load_mag =  chute.main_coupling[i].output.load;
         M_COPY(chute.apd[i].input.pyld_T_body_from_SR,  chute.relstate.pyld.param.pyld_T_body_from_sr);
      
         parachute_att_pnt_damping(chute.apd[i]);          

         //If Att Pnt Damping enabled, overwrite coupling force and moment outputs with
         //forces and moments from attach point damping model
         V_COPY(chute.main_coupling[i].output.endpoint1_force_SR, chute.apd[i].output.att_pnt_force_SR);
         V_COPY(chute.main_coupling[i].output.endpoint1_moment_SR, chute.apd[i].output.att_pnt_moment_SR);
      }
   }

///////////////////////////////////////////////////////////////////////////////
//        FORCES/MOMENTS ACTING ON PAYLOAD                                   //
///////////////////////////////////////////////////////////////////////////////

   // Determine max index value for summing up forces/moments to output
   // It is dependent on whether system has confluence point or not.
   if (chute.param.has_confl_point == true) { 
      max_index_value = chute.param.num_harness_lines;
   }  
   else {
      max_index_value = chute.param.num_chutes; 
   }

   //Summarize chute forces and moments acting on CM 
   for (int j = 0; j < max_index_value; j++) {
      if ( ! chute.chute_released[j] ) {
         for (int i = 0; i < 3; i++) {
            if (chute.param.has_confl_point == true) {
               chute.output.force_on_payload[i] = 
                  chute.output.force_on_payload[i] + chute.harness_coupling[j].output.endpoint1_force_SR[i];
               chute.output.moment_on_payload[i] = 
                  chute.output.moment_on_payload[i] + chute.harness_coupling[j].output.endpoint1_moment_SR[i];
            }
            else { 
               if (chute.param.fail_random_chute == true) {
                  if (j == chute.param.failed_chute) {
                     chute.main_coupling[j].output.load = 0.0;
                     continue;
                  }
               }
               chute.output.force_on_payload[i] = 
                  chute.output.force_on_payload[i] + chute.main_coupling[j].output.endpoint1_force_SR[i];
               chute.output.moment_on_payload[i] = 
                  chute.output.moment_on_payload[i] + chute.main_coupling[j].output.endpoint1_moment_SR[i];
            }
         }
      }
   }

   //Summarize mortar forces and moments acting on CM 
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < chute.param.num_chutes; j++) {
         if ( ! chute.chute_released[j] ) {
            chute.output.total_mortar_force[i] = 
               chute.output.total_mortar_force[i] + chute.mortar[j].output.mortar_forces[i]; 
            chute.output.total_mortar_moment[i] = 
               chute.output.total_mortar_moment[i] + chute.mortar[j].output.mortar_moments[i]; 
         }
      }
   }

///////////////////////////////////////////////////////////////////////////////
// MONITORING CODE                                                           //
///////////////////////////////////////////////////////////////////////////////

   parachute_recontact_monitor ( pyld_angle_of_attack_total,
                                 pyld_angle_of_attack,
                                 pyld_sideslip,
                                 chute.input.pyld_qbar,
                                 &chute );

///////////////////////////////////////////////////////////////////////////////

   return(0);

}

