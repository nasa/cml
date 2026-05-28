/******************************* TRICK HEADER **********************************
PURPOSE:
    (To determine current phase of parachute deployment sequence and
     call appropriate routines to compute aero/coupling forces.)
REFERENCE:
    (( -- ))
ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))
LIBRARY DEPENDENCY:
    (
     (parachute_phasing.o)
     (parachute_state_atmos.o)
     (parachute_attitude_reset.o)
     (orbutil_pfix_to_topo.o)
     (parachute_mass_properties_mark2.o)
     (mortar_recoil.o)
    )

PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (Dec 2009) (Initial Release))
     ((Jeff Semrau) (Honeywell) (May 2010) (Osiris CR 763))
     ((Jeff Semrau) (Honeywell) (June 2010) (Osiris DR 806) (Corrected bug in attach point indices sent to coupling model))
     ((Jeff Semrau) (Honeywell) (Aug 2010) (Osiris DR 824) (Various updates for pilot modeling, mass props and new analysis vars))
     ((Jeff Semrau) (Honeywell) (Jan 2011) (Osiris CR 859) (Various updates for mass props, line sail, etc))
     ((Jeff Semrau) (Honeywell) (May 2011) (lmbp683) (Various updates, using mass_props_mark2 for all chutes now, line sail for drgs, etc))
     ((Jeff Semrau) (Honeywell) (Jul 2011) (lmbp934) (Added new logic for modeling a skipped reef stage 2))
     ((Jeff Semrau) (Honeywell) (May 2012) (lmbp1650) (LRS MMv9 Updates))
     ((Jeff Semrau) (Honeywell) (Oct 2012) (lmbp2122) (Updated Skipped Stage1 Modeling))
     ((Jeff Semrau) (Honeywell) (Jan 2013) (lmbp2290) (LRS MMv11 Updates))
     ((Jeff Semrau) (Honeywell) (Aug 2013) (lmbp2720) (Correction to calculation of mortar velocity direction))
     ((Jeff Semrau) (Honeywell) (Nov 2013) (lmbp3148) (Updates to line sail, new attitude reset code, new confluence code))
     ((Jeff Semrau) (Honeywell) (Nov 2015) (CR4609) (Updates for new integration method))
     ((Jeff Semrau) (Honeywell) (Aug 2018) (CR11107) (Updates to more cleanly handle staggered mortar fire))
*******************************************************************************/

/*
 * Include files for package
 */

#include "../include/parachute_exec.hh"
#include "../include/parachute_pre_atmos.hh"
#include "../include/parachute_proto.hh"
#include "../include/orbitalutil_2.h"

#include "jeod/models/environment/planet/include/planet.hh"
#include "trick/trick_math.h"
#include "trick/exec_proto.h"            /* For get_job_cycle routine */
#include "trick/constant.h"              /* For unit conversions */

#include <cmath>

void parachute_phasing (PARACHUTE_DATA&    chute)

{


///////////////////////////////////////////////////////////////////////////////
//        STATE ATMOSPHERE                                                   //
///////////////////////////////////////////////////////////////////////////////

   //Code to populate state atmosphere input data structure.
   //To simplify things we are just using one chute's pos/vel as the input to calculate
   //GRAM inputs. We are just using GRAM results from one chute to use on all chutes.
   // Have to use first active chute since they might not all deploy
   for (int i=0; i < chute.param.num_chutes ; i++) {
      if ( chute.init_complete[i] ) {
         V_COPY(chute.gram.input.inertial_pos, chute.eom_main[i].output.inertial_pos);
         V_COPY(chute.gram.input.inertial_vel, chute.eom_main[i].output.inertial_vel);
         M_COPY(chute.gram.input.eci2ecef, chute.input.eci2ecef);
         break ;
      }
   }

   //Calculate state parameters need by Chute GRAM Model
   parachute_state_atmos(chute.gram);

   //Transform Chute Winds from NED to Inertial Frame
   double wind_vel_inertial[3];
   pfix_to_topo(chute.gram.output.geocen_lat,
                chute.gram.output.geocen_lon,
                chute.T_ECEF_topd);
   MtxV(chute.atm_wind_vel_pfix, chute.T_ECEF_topd, chute.input.atm_wind_vel_NED);
   MtxV(wind_vel_inertial,chute.input.eci2ecef, chute.atm_wind_vel_pfix);
   V_ADD(chute.wind_vel_inertial, wind_vel_inertial, chute.gram.work.VLocalIDueToEarthOmega);


   //Model deployment, inflation and steady state for each chute being modeled
   for (int i=0; i < chute.param.num_chutes; i++) {

      //Skip this iteration of loop if current chute has not finished initialization
      if (chute.init_complete[i] == false) {
         continue;
      }

   /////////////////////////////////////////////////
   /* NEW CODE TO COMPUTE CG TO CF LINE STRETCH */

      if (chute.relstate.chute[i].output.riser_suspension_distance >
      (chute.param.suspension_line_length + chute.param.riser_line_length)) {
         if (!chute.relstate.chute[i].output.riser_susp_line_stretch) {
            chute.relstate.chute[i].output.riser_susp_line_stretch = true;
         }
      }
      if (chute.pilot_main_relstate.chute[i].output.riser_suspension_distance >
      (chute.param.pilot_suspension_length + chute.param.pilot_riser_length)) {
         if (!chute.pilot_main_relstate.chute[i].output.riser_susp_line_stretch) {
            chute.pilot_main_relstate.chute[i].output.riser_susp_line_stretch = true;
         }
      }
      if (chute.param.pilot_chute_active == true) {
      // Determine if the canopy has been fully extracted from the bag
         if ( chute.pilot_main_relstate.chute[i].output.riser_suspension_distance >
         ( chute.param.pilot_suspension_length + chute.param.pilot_riser_length + chute.param.pilot_diameter/2.0 ) ) {
            if (!chute.pilot_main_relstate.chute[i].output.canopy_extracted ) {
               chute.pilot_main_relstate.chute[i].output.canopy_extracted = true;
            }
         }

         if ( chute.relstate.chute[i].output.riser_suspension_distance >
         (chute.param.suspension_line_length + chute.param.riser_line_length + ((M_PI * chute.param.main_diameter)/6.0)) ) {
            if ( !chute.relstate.chute[i].output.canopy_extracted ) {
               chute.relstate.chute[i].output.canopy_extracted = true;
            }
         }

      }
      else {
      // Determine if the canopy has been fully extracted from the bag
         if ( chute.relstate.chute[i].output.riser_suspension_distance >
         ( chute.param.suspension_line_length + chute.param.riser_line_length + chute.param.main_diameter/2.0 ) ) {
            if (!chute.relstate.chute[i].output.canopy_extracted ) {
               chute.relstate.chute[i].output.canopy_extracted = true;
            }
         }
      }
   }


   for (int i=0; i < chute.param.num_chutes; i++) {

      //Skip this iteration of loop if current chute has not finished initialization
      if (chute.init_complete[i] == false) {
         continue;
      }

      switch (chute.chute_phase[i]) {

         case PARACHUTE_DATA::mortar_fire:

            // Populate Mortar Input Data Structure
            V_COPY(chute.mortar[i].input.CG_position, chute.input.cg);
            chute.mortar[i].input.sim_time_local = chute.sim_time_local;
            chute.mortar[i].input.mortar_fired   = chute.mortar_fired[i];

            mortar_recoil( &chute.param.LRS_origin_in_SR_frame[0],
                           &chute.param.LRS_to_SR_rotation[0],
                           &(chute.mortar[i]) );

            if (chute.param.drogue_deployed_mains == true) { // 1 = Mains
               //During Bag Strip hold main transformations to last known good drogue chute values
               M_COPY(chute.eom_main[i].output.T_body_to_inertial, chute.temp_T_b_to_i);
               M_COPY(chute.eom_main[i].output.T_inertial_to_body, chute.temp_T_i_to_b);
               mat_to_quat(chute.eom_main[i].work.quat_inertial_to_body, chute.temp_T_i_to_b);
            }

            //Determine if Phase update should happen
            if (chute.mortar[i].output.firing_complete == true ) {
               if (chute.param.pilot_chute_active == true) {
                  chute.chute_phase[i] = PARACHUTE_DATA::pilot_free;
               }
               else {
                  chute.chute_phase[i] = PARACHUTE_DATA::main_deployment;

               }
            }

            //Apply Mortar Exit Velocity to chute in inertial frame.
            //This is for proper deploy time of the chute.
            //Since mortar_direction_SR is the direction the mortar force in
            //pyld SR frame, am using negative of mortar vel to get correct direction of
            //mortar exit velocity, the direction of mortar vel is equal and opposite
            //to direction of mortar force.
            if (chute.mortar[i].work.vel_first_pass == true) {
               V_SCALE(chute.mortar[i].work.mortar_vel_pyld_SR,
                       chute.mortar[i].work.mortar_direction_SR,
                       -chute.mortar[i].param.mortar_vel);

               //Now rotate mortar vel in Pyld SR frame to Chute SR Frame
               MxV(chute.mortar[i].work.mortar_vel_chute_SR,
                   chute.param.T_pyldSR_to_chuteSR,
                   chute.mortar[i].work.mortar_vel_pyld_SR);

               //Determine which chute is being mortared, pilot or drogue, then rotate mortar exit velocity
               //to inertial frame, and add it to current chute inertial velocity to update chute velocity.
               if (chute.param.pilot_chute_active == true) {
                  MxV(chute.mortar[i].work.mortar_vel_inertial,
                      chute.eom_pilot[i].output.T_body_to_inertial,
                      chute.mortar[i].work.mortar_vel_chute_SR);
                  V_INCR( chute.eom_pilot[i].output.inertial_vel,
                          chute.mortar[i].work.mortar_vel_inertial);
               }
               else {
                  MxV(chute.mortar[i].work.mortar_vel_inertial,
                      chute.eom_main[i].output.T_body_to_inertial,
                      chute.mortar[i].work.mortar_vel_chute_SR);
                  V_INCR( chute.eom_main[i].output.inertial_vel,
                          chute.mortar[i].work.mortar_vel_inertial);
               }
                  chute.mortar[i].work.vel_first_pass = false;

                 //Capture Start Time
                 chute.deploy_start_time[i] = chute.sim_time_local;
            }

            break;

         case PARACHUTE_DATA::pilot_free:

            //Determine if Phase update should happen
            if ((chute.pilot_main_relstate.chute[i].output.riser_susp_line_stretch == true ) &&
                (chute.line_sail[i].work.theta < chute.line_sail[i].param.min_theta)) {

               // Once the canopy is fully extracted, deployment is complete
               if ( chute.pilot_main_relstate.chute[i].output.canopy_extracted == true ) {
                  double magnitude_position_shift;
                  double vector_position_shift[3];

                  chute.chute_phase[i] = PARACHUTE_DATA::main_deployment;
                  chute.deploy_end_time[i] = chute.sim_time_local;
                  chute.deploy_time[i] = chute.deploy_end_time[i] - chute.deploy_start_time[i];

                  // Set the pilot vel to pyld vel
                  V_COPY(chute.eom_pilot[i].output.inertial_vel, chute.input.pyld_inertial_vel);

                  // Initialize the main chute to an unstretched state
                  magnitude_position_shift = chute.pilot_main_relstate.chute[i].output.riser_suspension_distance -
                                             ( chute.param.pilot_riser_length + chute.param.pilot_suspension_length );
                  V_SCALE( vector_position_shift,
                           chute.pilot_main_relstate.chute[i].output.chute_cg_force_vec_inert,
                           magnitude_position_shift );
                  V_ADD( chute.eom_pilot[i].output.inertial_pos,
                         chute.eom_pilot[i].output.inertial_pos,
                         vector_position_shift );

                  parachute_attitude_reset(chute, chute.eom_pilot[i]);

               }

            }

            break;

         case PARACHUTE_DATA::main_deployment:

            if (chute.param.pilot_chute_active == false) {

               if (chute.param.drogue_deployed_mains == true) {
                  //During Bag Strip hold main transformations to last known good drogue chute values
                  M_COPY(chute.eom_main[i].output.T_body_to_inertial, chute.temp_T_b_to_i);
                  M_COPY(chute.eom_main[i].output.T_inertial_to_body, chute.temp_T_i_to_b);
                  mat_to_quat(chute.eom_main[i].work.quat_inertial_to_body, chute.temp_T_i_to_b);

                  if (chute.relstate.chute[i].output.chute_line_stretch == true) {
                     V_COPY(chute.eom_main[i].output.inertial_vel, chute.input.pyld_inertial_vel);
                  }
               }

               if ((chute.relstate.chute[i].output.riser_susp_line_stretch == true) &&
                   (chute.line_sail[i].work.theta < chute.line_sail[i].param.min_theta))  {

                  // Once the canopy is fully extracted, deployment is complete
                  if ( chute.relstate.chute[i].output.canopy_extracted == true ) {
                     double magnitude_position_shift;
                     double vector_position_shift[3];

                     chute.chute_phase[i] = PARACHUTE_DATA::main_inflating;
                     chute.deploy_end_time[i] = chute.sim_time_local;
                     chute.deploy_time[i] = chute.deploy_end_time[i] - chute.deploy_start_time[i];

                     // Set the main bag vel to pyld vel
                     V_COPY(chute.eom_main[i].output.inertial_vel, chute.input.pyld_inertial_vel);

                     // Initialize the main chute to an unstretched state
                     magnitude_position_shift = chute.relstate.chute[i].output.riser_suspension_distance -
                                                ( chute.param.main_riser_length + chute.param.main_suspension_length );
                     V_SCALE( vector_position_shift,
                              chute.relstate.chute[i].output.chute_cg_force_vec_inert,
                              magnitude_position_shift );
                     V_ADD( chute.eom_main[i].output.inertial_pos,
                            chute.eom_main[i].output.inertial_pos,
                            vector_position_shift );

                     parachute_attitude_reset(chute, chute.eom_main[i]);

                  }
               }

            }
            else {

               //Copy over current deploy load data for use next frame
               chute.main_coupling[i].work.previous_cumulative_mass = chute.main_coupling[i].work.current_cumulative_mass;
               chute.main_coupling[i].work.previous_deploy_distance = chute.main_coupling[i].input.deploy_distance;
               //For analysis use, compute deploy load impulse
               chute.main_coupling[i].output.deploy_impulse =
                  chute.main_coupling[i].output.deploy_load * exec_get_job_cycle(0) + chute.main_coupling[i].output.deploy_impulse;

               switch (chute.main_deploy_subphase[i]) {

                  case PARACHUTE_DATA::pilot_inflating:

                     //Populate mass model for pilot chute inputs
                     chute.pilot_massprops[i].input.nominal_diameter = chute.param.pilot_diameter;
                     chute.pilot_massprops[i].input.current_diameter = chute.pilot_inflation[i].output.current_diameter;
                     chute.pilot_massprops[i].input.air_density      = chute.input.atm_density;
                     chute.pilot_massprops[i].input.dt               = exec_get_job_cycle(0);
                     chute.pilot_massprops[i].param.dlp              = chute.param.pilot_suspension_length;
                     chute.pilot_massprops[i].input.elasped_inflation_time = chute.pilot_inflation[i].work.total_elasped_time;
                     V_COPY(chute.pilot_massprops[i].input.chute_velocity, chute.pilot_main_relstate.chute[i].output.chute_wrel_vel_sr);

                     if (chute.pilot_inflation[i].output.Chute_Area < chute.param.min_pilot_area) {
                        chute.pilot_massprops[i].input.current_diameter = std::sqrt((chute.param.min_pilot_area * 4.0)/M_PI);
                        chute.pilot_massprops[i].input.first_pass       = true;
                     }

                     parachute_mass_properties_mark2(chute.sim_time_local,
                                                     &chute.pilot_inflation[i],
                                                     &chute.pilot_massprops[i]);

                     //Determine if main bag release has occurred
                     if  (chute.bag_strap_counter[i] > chute.param.strap_counter) {
                        chute.main_bag_release[i] = true;
                     }

                     if (chute.main_bag_release[i] == false) {
                        if ((chute.pilot_coupling[i].output.load > chute.param.bag_strap_strength)) {
                            chute.bag_strap_counter[i] = chute.bag_strap_counter[i] + 1;
                            break;
                        }
                        else {
                           chute.bag_strap_counter[i] = 0;
                           break;
                        }
                     }

                  case PARACHUTE_DATA::main_free:

                     //Determine if main line stretch has occurred
                     if (chute.relstate.chute[i].output.riser_susp_line_stretch == false) {
                        break;
                     }

                  case PARACHUTE_DATA::bag_strip:

                    // Once the canopy is fully extracted, deployment is complete
                    if ( chute.relstate.chute[i].output.canopy_extracted == true ) {
                       double magnitude_position_shift;
                       double vector_position_shift[3];

                       chute.chute_phase[i] = PARACHUTE_DATA::main_inflating;
                       chute.deploy_end_time[i] = chute.sim_time_local;
                       chute.deploy_time[i] = chute.deploy_end_time[i] - chute.deploy_start_time[i];

                       // Set the main bag vel to pyld vel
                       V_COPY(chute.eom_main[i].output.inertial_vel, chute.input.pyld_inertial_vel);

                       // Initialize the main chute to an unstretched state
                       magnitude_position_shift = chute.relstate.chute[i].output.riser_suspension_distance -
                                                  (chute.param.main_riser_length + chute.param.main_suspension_length) -
                                                  (chute.main_coupling[i].work.drag/chute.main_coupling[i].param.spring_constant);

                       V_SCALE( vector_position_shift,
                                chute.relstate.chute[i].output.chute_cg_force_vec_inert,
                                magnitude_position_shift );
                       V_ADD( chute.eom_main[i].output.inertial_pos,
                              chute.eom_main[i].output.inertial_pos,
                              vector_position_shift );

                       parachute_attitude_reset(chute, chute.eom_main[i]);
                       quat_to_mat(chute.eom_main[i].output.T_inertial_to_body, chute.eom_main[i].work.quat_inertial_to_body);
                       M_TRANS(chute.eom_main[i].output.T_body_to_inertial, chute.eom_main[i].output.T_inertial_to_body);

                    }

               } //Close bracket for main_deploy_subphase switch statement

            } //Close bracket for else statement

            break;

         case PARACHUTE_DATA::main_inflating:

            //Populate mass model for main chute inputs
            chute.main_massprops[i].input.nominal_diameter = chute.param.main_diameter;
            chute.main_massprops[i].input.air_density      = chute.input.atm_density;
            chute.main_massprops[i].input.dt               = exec_get_job_cycle(0);
            chute.main_massprops[i].param.dlp              = chute.param.main_suspension_length;
            chute.main_massprops[i].input.elasped_inflation_time = chute.main_inflation[i].work.total_elasped_time;
            V_COPY(chute.main_massprops[i].input.chute_velocity, chute.relstate.chute[i].output.chute_wrel_vel_sr);

            if (chute.main_inflation[i].output.Chute_Area < chute.param.min_chute_area) {
                  chute.main_massprops[i].input.first_pass       = true;
            }


            parachute_mass_properties_mark2(chute.sim_time_local,
                                               &chute.main_inflation[i],
                                               &chute.main_massprops[i]);

            int skip_index;
            skip_index = chute.param.skipped_chute;

            if (i == chute.param.skipped_chute) {
               if ((chute.skip_stage1 == true) &&
                  (chute.main_inflation[chute.param.skipped_chute].work.reef_stage == Inflation_Work::xreef_1) &&
                  (chute.skip_stage1_reset == false)) {

                  if (chute.main_inflation[skip_index].work.stage_elasped_time >=
                     (chute.main_inflation[skip_index].work.inflation_time +
                      chute.main_inflation[skip_index].param.deflation_time[chute.main_inflation[skip_index].work.reef_stage])) {
                     chute.main_inflation[chute.param.skipped_chute].param.transition_time[Inflation_Work::xreef_1] =
                        chute.main_inflation[chute.param.skipped_chute].work.inflation_time +
                        chute.main_inflation[skip_index].param.deflation_time[chute.main_inflation[skip_index].work.reef_stage];
                     chute.skip_stage1_reset = true;
                  }
               }


               //Code to reset stage 2 transition time if skipping stage 2
               //This prevents a discontinuity in chute inflation if fill time is
               //greater than nominal disreef time
               if (chute.skip_stage2 == true) {
                  chute.main_inflation[skip_index].param.transition_time[1] = 999.0;
                  if (chute.main_inflation[skip_index].work.reef_stage == Inflation_Work::xreef_2) {
                     if (chute.main_inflation[skip_index].work.stage_elasped_time >
                        (chute.main_inflation[skip_index].work.inflation_time +
                         chute.main_inflation[skip_index].param.deflation_time[chute.main_inflation[skip_index].work.reef_stage])) {
                        chute.chute_phase[skip_index] = PARACHUTE_DATA::main_steady_state;
                        chute.main_inflation[skip_index].work.reef_stage = Inflation_Work::xfull;
                        chute.main_inflation[skip_index].output.inflation_complete = true;
                     }
                  }
               }

               if ((chute.skip_stage2 == true) &&
                  (chute.main_inflation[skip_index].work.reef_stage == Inflation_Work::xreef_2) &&
                  (chute.skip_stage2_reset == false)) {

                  chute.main_inflation[skip_index].param.transition_time[Inflation_Work::xreef_2] =
                     chute.main_inflation[skip_index].work.inflation_time + chute.main_inflation[skip_index].param.transition_time[Inflation_Work::xreef_1];
                  chute.skip_stage2_reset = true;
               }
            }

            if (chute.main_inflation[i].output.inflation_complete == true) {
               chute.chute_phase[i] = PARACHUTE_DATA::main_steady_state;
            }

            //Check if all chutes are fully disreefed
            chute.all_chutes_disreefed = true;
            for (int j=0; j < chute.param.num_chutes; j++) {
               if (chute.main_inflation[j].output.inflation_complete == false) {
                  chute.all_chutes_disreefed = false;
               }
            }

            break;

         case PARACHUTE_DATA::main_steady_state:

            //Populate mass model for main chute inputs
            chute.main_massprops[i].input.nominal_diameter = chute.param.main_diameter;
            chute.main_massprops[i].input.air_density      = chute.input.atm_density;
            chute.main_massprops[i].input.dt               = exec_get_job_cycle(0);
            chute.main_massprops[i].param.dlp              = chute.param.main_suspension_length;
            chute.main_massprops[i].input.elasped_inflation_time = chute.main_inflation[i].work.total_elasped_time;
            V_COPY(chute.main_massprops[i].input.chute_velocity, chute.relstate.chute[i].output.chute_wrel_vel_sr);

            parachute_mass_properties_mark2(chute.sim_time_local,
                                            &chute.main_inflation[i],
                                            &chute.main_massprops[i]);

      } //Close bracket associated with phase switch statement

   } //Close bracket associated with for loop

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Compute Total Load Data for Analysis
   chute.total_load = 0.0;
   for (int i = 0; i < chute.param.num_chutes; i++) {
      chute.total_load = chute.total_load + chute.main_coupling[i].output.load;
   }

   //Compute load sharing amongst chutes
   if (chute.total_load > 0.0) { //Check to avoid divide by 0
      for (int i = 0; i < chute.param.num_chutes; i++) {
         chute.load_share[i] = chute.main_coupling[i].output.load/chute.total_load;
      }
   }

}

