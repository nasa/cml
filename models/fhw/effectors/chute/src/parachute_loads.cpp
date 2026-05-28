/******************************* TRICK HEADER **********************************
PURPOSE:
    (To determine current phase of parachute deployment sequence and 
     call appropriate routines to compute aero/coupling forces.)
REFERENCE:
    (( -- ))                                   
ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))
LIBRARY DEPENDENCY:
     (parachute_aero.o)
     (parachute_coupling.o)
     (parachute_mass_properties_mark2.o)
     (parachute_line_sail.o))
     (xcompute_chute_CDS.o)

PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (Oct 2015) (Initial Release))             
*******************************************************************************/

/*
 * Include files for package
 */

#include <cmath>

#include "../include/parachute_exec.hh"
#include "../include/parachute_pre_atmos.hh"
#include "../include/parachute_proto.hh"

#include "jeod/models/environment/planet/include/planet.hh"
#include "trick/trick_math.h"

/* Function prototypes */

void parachute_loads (PARACHUTE_DATA&    chute) 
                        
{

   int i; 

   //Populate aero input structure 
   chute.confpt_aero.input.aero_mode         = Aero_Inputs::ballistic_aero;
   chute.confpt_aero.input.mass              = chute.param.confpt_mass; 
   chute.confpt_aero.input.dynamic_pressure  = chute.relstate.chute[0].output.chute_dynamic_pressure;
   V_COPY(chute.confpt_aero.input.VairSR, chute.relstate.chute[0].output.chute_wrel_vel_sr);      
 
   //Compute Confluence Point Aero
   parachute_aero(&chute.confpt_aero);

   // Compute the confluence point damping
   V_SCALE(chute.conf_damping.damping_force_inert, chute.relstate.confl.output.confl_pnt_vel_inert_cg_rel, -chute.conf_damping.damping_coefficient);

   //Model deployment, inflation and steady state for each chute being modeled
   for (i=0; i < chute.param.num_chutes; i++) {

      PARACHUTE_DATA::chute_phases current_chute_phase = chute.chute_phase[i];

      switch (chute.chute_phase[i]) {

         case PARACHUTE_DATA::mortar_fire:

            //Do nothing for mortar fire phase, that's handled in parachute_phasing now.
            break;

         case PARACHUTE_DATA::pilot_free: 

            chute.pilot_aero[i].input.aero_mode         = Aero_Inputs::aero_coef;
            chute.pilot_aero[i].input.dynamic_pressure  = chute.pilot_main_relstate.chute[i].output.chute_dynamic_pressure;
            V_COPY(chute.pilot_aero[i].input.att_pnt_inertial_pos, chute.pilot_main_relstate.pyld.output.pyld_att_pnt_pos_inert[i]);
            V_COPY(chute.pilot_aero[i].input.chute_inertial_pos, chute.eom_pilot[i].output.inertial_pos);
            chute.pilot_aero[i].input.mach              = chute.pilot_main_relstate.chute[i].output.chute_mach;
            chute.pilot_aero[i].input.alpha_total       = chute.pilot_main_relstate.chute[i].output.chute_alpha_total;
            chute.pilot_aero[i].input.sep_distance      = chute.pilot_main_relstate.chute[i].output.chute_sep_distance_mag;
            chute.pilot_aero[i].input.CDS               = chute.param.min_pilot_area;
            chute.pilot_aero[i].input.Chute_Area        = chute.param.min_pilot_area;
            chute.pilot_aero[i].input.CBAR              = chute.param.min_pilot_CBAR;
            V_COPY(chute.pilot_aero[i].input.body_rates, chute.eom_pilot[i].output.body_rates);
            V_COPY(chute.pilot_aero[i].input.VairSR, chute.pilot_main_relstate.chute[i].output.chute_wrel_vel_sr);
            M_COPY(chute.pilot_aero[i].input.T_body_to_SR, chute.param.T_body_to_SR);
            M_COPY(chute.pilot_aero[i].input.T_aero_to_body, chute.param.T_aero_to_body);

            //Compute pilot ballistic aero
            parachute_aero(&chute.pilot_aero[i]);

            //Populate and call line sail drag model
            V_COPY(chute.line_sail[i].input.VairB, chute.input.pyld_VairB);
            V_COPY(chute.line_sail[i].input.chute_inertial_pos, chute.eom_pilot[i].output.inertial_pos);
            V_COPY(chute.line_sail[i].input.pyld_inertial_pos, chute.input.pyld_inertial_pos);
            M_COPY(chute.line_sail[i].input.pyld_T_inertial_to_body, chute.input.pyld_T_inertial_to_body);
            M_COPY(chute.line_sail[i].input.chute_T_struct_to_inertial, chute.eom_pilot[i].output.T_struct_to_inertial);
            chute.line_sail[i].input.chute_qbar = chute.pilot_main_relstate.chute[i].output.chute_dynamic_pressure;

            parachute_line_sail(&chute.line_sail[i]);

            //Add Line sail drag to current aero drag
            double new_drag2[3];
            V_INIT(new_drag2);
            V_ADD(new_drag2, chute.pilot_aero[i].output.force_SR, chute.line_sail[i].output.drag_SR);
            V_COPY(chute.pilot_aero[i].output.force_SR, new_drag2);

            break;

         case PARACHUTE_DATA::main_deployment:

            if (chute.param.pilot_chute_active == false) {

               //Populate aero input structure
               chute.main_aero[i].input.aero_mode        = Aero_Inputs::aero_coef;
               chute.main_aero[i].input.dynamic_pressure = chute.relstate.chute[i].output.chute_dynamic_pressure;
               chute.main_aero[i].input.mach             = chute.relstate.chute[i].output.chute_mach;
               chute.main_aero[i].input.alpha_total      = chute.relstate.chute[i].output.chute_alpha_total;
               chute.main_aero[i].input.sep_distance     = chute.relstate.chute[i].output.chute_sep_distance_mag;
               chute.main_aero[i].input.CDS              = chute.param.min_chute_area; 
               chute.main_aero[i].input.Chute_Area       = chute.param.min_chute_area; 
               chute.main_aero[i].input.CBAR             = chute.param.main_diameter; 
               V_COPY(chute.main_aero[i].input.body_rates, chute.eom_main[i].output.body_rates);
               V_COPY(chute.main_aero[i].input.VairSR, chute.relstate.chute[i].output.chute_wrel_vel_sr);
               M_COPY(chute.main_aero[i].input.T_body_to_SR, chute.param.T_body_to_SR);
               M_COPY(chute.main_aero[i].input.T_aero_to_body, chute.param.T_aero_to_body);
               V_COPY(chute.main_aero[i].input.att_pnt_inertial_pos, chute.relstate.pyld.output.pyld_att_pnt_pos_inert[i]);
               V_COPY(chute.main_aero[i].input.chute_inertial_pos, chute.eom_main[i].output.inertial_pos);

               //Compute aero  
               parachute_aero(&chute.main_aero[i]);

               //Populate line sail input structure
               V_COPY(chute.line_sail[i].input.VairB, chute.input.pyld_VairB);
               V_COPY(chute.line_sail[i].input.chute_inertial_pos, chute.eom_main[i].output.inertial_pos);
               V_COPY(chute.line_sail[i].input.pyld_inertial_pos, chute.input.parent_inertial_pos);
               M_COPY(chute.line_sail[i].input.pyld_T_inertial_to_body, chute.input.parent_T_inertial_to_body);
               M_COPY(chute.line_sail[i].input.chute_T_struct_to_inertial, chute.eom_main[i].output.T_body_to_inertial);

               chute.line_sail[i].input.chute_qbar = chute.relstate.chute[i].output.chute_dynamic_pressure;

               //Compute line sail 
               parachute_line_sail(&chute.line_sail[i]);

               //Add Line sail drag to current aero drag
               double new_drag[3];
               V_INIT(new_drag);
               V_ADD(new_drag, chute.main_aero[i].output.force_SR, chute.line_sail[i].output.drag_SR);
               V_COPY(chute.main_aero[i].output.force_SR, new_drag);
            }
            else {
   
               switch (chute.main_deploy_subphase[i]) {

                  case PARACHUTE_DATA::pilot_inflating:

                     //Populate inflation model for pilot
                     chute.pilot_inflation[i].input.deploy_velocity = chute.pilot_main_relstate.chute[i].output.chute_wrel_vel_mag;
                     chute.pilot_inflation[i].input.nominal_diameter = chute.param.pilot_diameter;
                     chute.pilot_inflation[i].param.drag_coeff       = chute.pilot_aero[i].param.nominal_cd;

                     xcompute_chute_CDS ( &chute.pilot_inflation[i],
                                          chute.sim_time_local);

                     //Populate aero model input structure                
                     chute.pilot_aero[i].input.aero_mode         = Aero_Inputs::aero_coef;
                     chute.pilot_aero[i].input.dynamic_pressure  = chute.pilot_main_relstate.chute[i].output.chute_dynamic_pressure;
                     V_COPY(chute.pilot_aero[i].input.att_pnt_inertial_pos, chute.pilot_main_relstate.pyld.output.pyld_att_pnt_pos_inert[i]);
                     V_COPY(chute.pilot_aero[i].input.chute_inertial_pos, chute.eom_pilot[i].output.inertial_pos);
                     chute.pilot_aero[i].input.mach              = chute.pilot_main_relstate.chute[i].output.chute_mach;
                     chute.pilot_aero[i].input.alpha_total       = chute.pilot_main_relstate.chute[i].output.chute_alpha_total;    
                     chute.pilot_aero[i].input.sep_distance      = chute.pilot_main_relstate.chute[i].output.chute_sep_distance_mag; 
                     chute.pilot_aero[i].input.CDS               = chute.pilot_inflation[i].output.CDS;
                     chute.pilot_aero[i].input.Chute_Area        = chute.pilot_inflation[i].output.Chute_Area;
                     chute.pilot_aero[i].input.CBAR              = chute.param.pilot_diameter;               
                     V_COPY(chute.pilot_aero[i].input.body_rates, chute.eom_pilot[i].output.body_rates);
                     V_COPY(chute.pilot_aero[i].input.VairSR, chute.pilot_main_relstate.chute[i].output.chute_wrel_vel_sr);      
                     M_COPY(chute.pilot_aero[i].input.T_body_to_SR, chute.param.T_body_to_SR);
                     M_COPY(chute.pilot_aero[i].input.T_aero_to_body, chute.param.T_aero_to_body);
   
                     if (chute.pilot_inflation[i].output.Chute_Area < chute.param.min_pilot_area) { 
                        chute.pilot_aero[i].input.CDS        = chute.param.min_pilot_area;
                        chute.pilot_aero[i].input.Chute_Area = chute.param.min_pilot_area;
                     }

                     if (chute.pilot_inflation[i].output.current_diameter < chute.param.min_pilot_CBAR) {
                        chute.pilot_aero[i].input.CBAR              = chute.param.min_pilot_CBAR;
                     }

                     //Compute pilot chute aero
                     parachute_aero( &chute.pilot_aero[i] ); 

                  case PARACHUTE_DATA::main_free:

                     //Populate aero input structure 
                     chute.main_aero[i].input.aero_mode         = Aero_Inputs::ballistic_aero;
                     chute.main_aero[i].input.mass              = chute.main_massprops[i].param.parachute_dry_mass;
                     chute.main_aero[i].input.dynamic_pressure  = chute.relstate.chute[i].output.chute_dynamic_pressure;  
                     V_COPY(chute.main_aero[i].input.VairSR, chute.relstate.chute[i].output.chute_wrel_vel_sr);      
                     V_COPY(chute.main_aero[i].input.att_pnt_inertial_pos, chute.relstate.pyld.output.pyld_att_pnt_pos_inert[i]);
                     V_COPY(chute.main_aero[i].input.chute_inertial_pos, chute.eom_main[i].output.inertial_pos);
 
                     //Compute main ballistic aero
                     parachute_aero(&chute.main_aero[i]); 

                     //Determine if main line stretch has occurred  
                     if (chute.relstate.chute[i].output.riser_susp_line_stretch == false) {
                        break; 
                     }

                  case PARACHUTE_DATA::bag_strip:

                    //Populate aero input structure
                    chute.main_aero[i].input.aero_mode         = Aero_Inputs::ballistic_aero;
                    chute.main_aero[i].input.mass              = chute.main_massprops[i].param.parachute_dry_mass;
                    chute.main_aero[i].input.dynamic_pressure  = chute.relstate.chute[i].output.chute_dynamic_pressure;
                    V_COPY(chute.main_aero[i].input.VairSR, chute.relstate.chute[i].output.chute_wrel_vel_sr);
                    V_COPY(chute.main_aero[i].input.att_pnt_inertial_pos, chute.relstate.pyld.output.pyld_att_pnt_pos_inert[i]);
                    V_COPY(chute.main_aero[i].input.chute_inertial_pos, chute.eom_main[i].output.inertial_pos);

                    //Compute main ballistic aero
                    parachute_aero(&chute.main_aero[i]);

               } //Close bracket for main_deploy_subphase switch statement

            } //Close bracket for else statement

            break;

         case PARACHUTE_DATA::main_inflating:

           chute.main_inflation[i].input.deploy_velocity  = chute.relstate.chute[i].output.chute_wrel_vel_mag;
           chute.main_inflation[i].input.nominal_diameter = chute.param.main_diameter;
           chute.main_inflation[i].param.drag_coeff       = chute.main_aero[i].param.nominal_cd;

           xcompute_chute_CDS ( &chute.main_inflation[i],
                                 chute.sim_time_local);

            //Populate aero input structure 
            chute.main_aero[i].input.aero_mode        = Aero_Inputs::aero_coef;
            chute.main_aero[i].input.dynamic_pressure = chute.relstate.chute[i].output.chute_dynamic_pressure;
            chute.main_aero[i].input.mach             = chute.relstate.chute[i].output.chute_mach;
            chute.main_aero[i].input.alpha_total      = chute.relstate.chute[i].output.chute_alpha_total;                           
            chute.main_aero[i].input.sep_distance     = chute.relstate.chute[i].output.chute_sep_distance_mag; 
            chute.main_aero[i].input.CDS              = chute.main_inflation[i].output.CDS;
            chute.main_aero[i].input.Chute_Area       = chute.main_inflation[i].output.Chute_Area;
            chute.main_aero[i].input.CBAR             = chute.param.main_diameter;             
            chute.main_aero[i].input.min_drag = chute.main_coupling[i].work.drag;       
            V_COPY(chute.main_aero[i].input.body_rates, chute.eom_main[i].output.body_rates);
            V_COPY(chute.main_aero[i].input.VairSR, chute.relstate.chute[i].output.chute_wrel_vel_sr);      
            M_COPY(chute.main_aero[i].input.T_body_to_SR, chute.param.T_body_to_SR);
            M_COPY(chute.main_aero[i].input.T_aero_to_body, chute.param.T_aero_to_body);
            V_COPY(chute.main_aero[i].input.att_pnt_inertial_pos, chute.relstate.pyld.output.pyld_att_pnt_pos_inert[i]);
            V_COPY(chute.main_aero[i].input.chute_inertial_pos, chute.eom_main[i].output.inertial_pos);

            //To prevent aero F&M going to zero at start of inflation chute area
            //is zero at this point, hold this parameter to min value.
            if (chute.main_coupling[i].input.use_mass_rate_load == false) { 
               if (chute.main_inflation[i].output.Chute_Area < chute.param.min_chute_area) {
                  chute.main_aero[i].input.CDS              = chute.param.min_chute_area;
                  chute.main_aero[i].input.Chute_Area       = chute.param.min_chute_area;
               }
            }
            else { 
               if (chute.main_inflation[i].output.Chute_Area < chute.main_coupling[i].work.canopy_drag_area) {
                  chute.main_aero[i].input.CDS              = chute.main_coupling[i].work.canopy_drag_area; 
                  chute.main_aero[i].input.Chute_Area       = chute.main_coupling[i].work.canopy_drag_area; 
                  chute.main_aero[i].input.CBAR = std::sqrt((4.0 * chute.main_coupling[i].work.canopy_drag_area)/(chute.main_inflation[i].param.drag_coeff * M_PI));
               }
            }
   
            //Compute chute aero
            parachute_aero(&chute.main_aero[i]); 

            break;
 
         case PARACHUTE_DATA::main_steady_state: 

            //Populate aero input structure 
            chute.main_aero[i].input.aero_mode        = Aero_Inputs::aero_coef;
            chute.main_aero[i].input.dynamic_pressure = chute.relstate.chute[i].output.chute_dynamic_pressure;
            chute.main_aero[i].input.mach             = chute.relstate.chute[i].output.chute_mach;
            chute.main_aero[i].input.alpha_total      = chute.relstate.chute[i].output.chute_alpha_total;                           
            chute.main_aero[i].input.sep_distance     = chute.relstate.chute[i].output.chute_sep_distance_mag; 
            chute.main_aero[i].input.CDS              = chute.main_inflation[i].output.CDS;
            chute.main_aero[i].input.Chute_Area       = chute.main_inflation[i].output.Chute_Area;
            chute.main_aero[i].input.CBAR             = chute.param.main_diameter;             
            V_COPY(chute.main_aero[i].input.body_rates, chute.eom_main[i].output.body_rates);
            V_COPY(chute.main_aero[i].input.VairSR, chute.relstate.chute[i].output.chute_wrel_vel_sr);      
            M_COPY(chute.main_aero[i].input.T_body_to_SR, chute.param.T_body_to_SR);
            M_COPY(chute.main_aero[i].input.T_aero_to_body, chute.param.T_aero_to_body);
            V_COPY(chute.main_aero[i].input.att_pnt_inertial_pos, chute.relstate.pyld.output.pyld_att_pnt_pos_inert[i]);
            V_COPY(chute.main_aero[i].input.chute_inertial_pos, chute.eom_main[i].output.inertial_pos);
   
            //Compute chute aero
            parachute_aero(&chute.main_aero[i]);

      } //Close bracket associated with phase switch statement

      //Populate data structures to compute riser line forces
      if ((chute.chute_phase[i] == PARACHUTE_DATA::main_deployment) &&
         (chute.param.pilot_chute_active == true)) {   //Compute coupling between pilot and main bag

         M_COPY(chute.pilot_coupling[i].input.endpoint2_T_ECI_to_Body, chute.eom_pilot[i].output.T_inertial_to_body);
         M_COPY(chute.pilot_coupling[i].input.endpoint1_T_ECI_to_Body, chute.eom_main[i].output.T_inertial_to_body);  
         M_COPY(chute.pilot_coupling[i].input.endpoint2_T_Body_to_SR, chute.param.T_body_to_SR); 
         M_COPY(chute.pilot_coupling[i].input.endpoint1_T_Body_to_SR, chute.param.T_body_to_SR);
         V_COPY(chute.pilot_coupling[i].input.endpoint2_vec_wrt_cg, chute.pilot_main_relstate.chute[i].output.chute_moment_arm);
         V_COPY(chute.pilot_coupling[i].input.endpoint1_vec_wrt_cg, chute.param.zero_vector);
         V_COPY(chute.pilot_coupling[i].input.relstate_force_uvec, chute.pilot_main_relstate.chute[i].output.chute_cg_force_vec_inert);

         chute.pilot_coupling[i].input.current_diameter        = chute.pilot_inflation[i].output.current_diameter;
         chute.pilot_coupling[i].input.nominal_diameter        = chute.param.pilot_diameter;
         chute.pilot_coupling[i].input.nominal_length          = chute.param.pilot_riser_length + chute.param.pilot_suspension_length;
         chute.pilot_coupling[i].input.relstate_riser_distance = chute.pilot_main_relstate.chute[i].output.riser_suspension_distance;
         chute.pilot_coupling[i].input.relstate_riser_velocity = chute.pilot_main_relstate.chute[i].output.riser_suspension_stretch_rate;
         chute.pilot_coupling[i].input.use_inelastic_load      = false;

         //Code to hold pilot riser stretched distance and by extension pilot riser load steady after main line stretch
         if (chute.relstate.chute[i].output.riser_susp_line_stretch == true) {
            chute.pilot_coupling[i].input.relstate_riser_distance = chute.temp_distance[i];
            chute.pilot_coupling[i].input.relstate_riser_velocity = 0.0;
         }
         else {
            chute.temp_distance[i] = chute.pilot_coupling[i].input.relstate_riser_distance;
         }

         //Call Coupling Routine to compute pilot riser line forces
         if (current_chute_phase >= PARACHUTE_DATA::main_deployment) {
            parachute_coupling( &chute.pilot_coupling[i] );
         }

      }

      else {

         chute.pilot_coupling[i].input.relstate_riser_distance = 0.0;
         chute.pilot_coupling[i].input.relstate_riser_velocity = 0.0;
         chute.pilot_coupling[i].input.nominal_length          = chute.param.pilot_riser_length;
         V_INIT(chute.pilot_coupling[i].output.endpoint2_force_SR);
         V_INIT(chute.pilot_coupling[i].output.endpoint2_moment_SR);

      }

      //Code to switch from using distance from CF to Chute CG, or distance from CF to Suspension Line Confluence Attach Point
      //For now doing this for mains only.
      if (chute.param.pilot_chute_active == true) {   //Compute coupling between pilot and main bag

         chute.main_coupling[i].input.nominal_length          = chute.param.main_riser_length + chute.param.main_suspension_length;
         if (chute.relstate.chute[i].output.riser_susp_line_stretch == true) {

            chute.main_coupling[i].input.relstate_riser_distance = chute.relstate.chute[i].output.riser_suspension_distance;
            chute.main_coupling[i].input.relstate_riser_velocity = chute.relstate.chute[i].output.riser_suspension_stretch_rate;
         }
         else {
            //Set riser distance/velocity inputs to coupling model to 0, there is no deflection or
            //movement of the main riser line at this time.
            chute.main_coupling[i].input.relstate_riser_distance = 0.0;
            chute.main_coupling[i].input.relstate_riser_velocity = 0.0;
         }
      }
      else {
         chute.main_coupling[i].input.nominal_length          = chute.param.main_riser_length + chute.param.main_suspension_length;
         if (chute.relstate.chute[i].output.riser_susp_line_stretch == true) {

            chute.main_coupling[i].input.relstate_riser_distance = chute.relstate.chute[i].output.riser_suspension_distance;
            chute.main_coupling[i].input.relstate_riser_velocity = chute.relstate.chute[i].output.riser_suspension_stretch_rate;
         }
         else {
            //Set riser distance/velocity inputs to coupling model to 0, there is no deflection or
            //movement of the main riser line at this time.
            chute.main_coupling[i].input.relstate_riser_distance = 0.0;
            chute.main_coupling[i].input.relstate_riser_velocity = 0.0;
         }
      }

      M_COPY(chute.main_coupling[i].input.endpoint2_T_ECI_to_Body, chute.eom_main[i].output.T_inertial_to_body);
      M_COPY(chute.main_coupling[i].input.endpoint1_T_ECI_to_Body, chute.input.pyld_T_inertial_to_body); 

      M_COPY(chute.main_coupling[i].input.endpoint2_T_Body_to_SR, chute.param.T_body_to_SR); 
      M_COPY(chute.main_coupling[i].input.endpoint1_T_Body_to_SR, chute.param.T_body_to_SR);
      V_COPY(chute.main_coupling[i].input.endpoint2_vec_wrt_cg, chute.relstate.chute[i].output.chute_moment_arm);

      //Need to switch coupling input uvec after reef stage 1, otherwise get high chute and CM rates under mains. 
      if (chute.main_inflation[i].work.reef_stage > Inflation_Work::xreef_1) {
         V_COPY(chute.main_coupling[i].input.relstate_force_uvec, chute.relstate.chute[i].output.chute_ap_force_vec_inert);
      }
      else {
         V_COPY(chute.main_coupling[i].input.relstate_force_uvec, chute.relstate.chute[i].output.chute_cg_force_vec_inert);
      }

      if (chute.param.has_confl_point == true) { 
         V_COPY(chute.main_coupling[i].input.endpoint1_vec_wrt_cg, chute.param.zero_vector);
      }
      else {
         V_COPY(chute.main_coupling[i].input.endpoint1_vec_wrt_cg, chute.relstate.pyld.output.pyld_att_pnt_moment_arm[i]);
         M_COPY(chute.main_coupling[i].input.endpoint1_T_Body_to_SR,  chute.input.pyld_T_body_to_SR); 
      }
      chute.main_coupling[i].input.current_diameter        = chute.main_inflation[i].output.current_diameter; 
      chute.main_coupling[i].input.nominal_diameter        = chute.param.main_diameter;

      if ( current_chute_phase == PARACHUTE_DATA::main_deployment) {
         chute.main_coupling[i].input.use_inelastic_load = true;
      } else {
         chute.main_coupling[i].input.use_inelastic_load = false;
      }

      //Call Coupling Routine to compute main riser line forces
      if ( current_chute_phase > PARACHUTE_DATA::mortar_fire )  {

         if ((chute.main_coupling[i].input.use_mass_rate_load == true) && 
            (chute.main_inflation[i].output.Chute_Area < chute.main_coupling[i].work.canopy_drag_area))  { 

           chute.main_coupling[i].input.current_diameter  = std::sqrt((4.0 * chute.main_coupling[i].work.canopy_drag_area)/(chute.main_inflation[i].param.drag_coeff * M_PI));

         }

         // Load inputs for main deploy load modeling 
         V_SUB(chute.main_coupling[i].work.inertial_vel_diff,  chute.eom_main[i].output.inertial_vel, chute.input.pyld_inertial_vel);
         chute.main_coupling[i].input.velocity                 = V_MAG(chute.main_coupling[i].work.inertial_vel_diff);
         chute.main_coupling[i].input.drag_mass                = chute.main_massprops[i].param.parachute_dry_mass;
         chute.main_coupling[i].input.riser_distance           = chute.param.riser_line_length;
         chute.main_coupling[i].input.suspension_distance      = chute.param.suspension_line_length;
         chute.main_coupling[i].input.canopy_distance          = chute.param.main_diameter/2.0;
         chute.main_coupling[i].input.deploy_distance          = chute.relstate.chute[i].output.riser_suspension_distance;
         chute.main_coupling[i].input.qbar                     = chute.input.pyld_qbar;
         chute.main_coupling[i].input.canopy_mass              = chute.main_massprops[i].param.canopy_mass; 
         V_COPY(chute.main_coupling[i].input.VairB, chute.input.pyld_VairB);
         M_COPY(chute.main_coupling[i].input.pyld_T_inertial_to_body, chute.input.pyld_T_inertial_to_body);

         parachute_coupling( &chute.main_coupling[i] );
      }
  
   } //Close bracket associated with for loop

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   for (i = 0; i < chute.param.num_pyld_attach_points; i++) {
      //Compute Harness Line Forces/Moments
      //Populate Coupling Model Structure with Confluence Point Data and Attach Point Data
      M_COPY(chute.harness_coupling[i].input.endpoint2_T_ECI_to_Body, chute.input.pyld_T_inertial_to_body); 
      M_COPY(chute.harness_coupling[i].input.endpoint1_T_ECI_to_Body, chute.input.pyld_T_inertial_to_body);  
      M_COPY(chute.harness_coupling[i].input.endpoint1_T_Body_to_SR,  chute.input.pyld_T_body_to_SR); 
      M_COPY(chute.harness_coupling[i].input.endpoint2_T_Body_to_SR,  chute.param.T_body_to_SR); 
      V_COPY(chute.harness_coupling[i].input.endpoint1_vec_wrt_cg, chute.relstate.pyld.output.pyld_att_pnt_moment_arm[i]);
      V_COPY(chute.harness_coupling[i].input.endpoint2_vec_wrt_cg, chute.param.zero_vector);
      V_COPY(chute.harness_coupling[i].input.relstate_force_uvec, chute.relstate.pyld.output.pyld_ap_force_vec_inert[i]);
      chute.harness_coupling[i].input.relstate_riser_distance = chute.relstate.pyld.output.harness_line_distance[i];
      chute.harness_coupling[i].input.relstate_riser_velocity = chute.relstate.pyld.output.harness_line_stretch_rate[i];
      chute.harness_coupling[i].input.nominal_length          = chute.param.harness_line_length[i];                                 
      chute.harness_coupling[i].input.use_inelastic_load      = false;

      //Call Coupling Routine to compute Harness Line Forces/Moments
      parachute_coupling( &chute.harness_coupling[i] );
   }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

