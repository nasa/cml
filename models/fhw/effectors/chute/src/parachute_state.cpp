/******************************* TRICK HEADER **********************************
PURPOSE:
    (To manage calls to parachute_relstate) 
REFERENCE:
    (( -- ))                                   
ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))
LIBRARY DEPENDENCY:
     (parachute_relstate.o)

PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (Oct 2015) (Initial Release))             
*******************************************************************************/

/*
 * Include files for package
 */

#include "../include/parachute_exec.hh"
#include "../include/parachute_proto.hh"

#include "trick/trick_math.h"

void parachute_state (PARACHUTE_DATA&    chute) 
                        
{

   int i; 

///////////////////////////////////////////////////////////////////////////////
//       RELSTATE                                                            //
///////////////////////////////////////////////////////////////////////////////

   if (chute.param.pilot_chute_active == true) {   //Will need to compute relstate of Pilot and Main Bag,
                                              //in this case main bag is the payload
      //Code to populate rel state input data structure for pilot/main system
      for (i = 0; i < chute.param.num_chutes; i++) {
         V_COPY(chute.pilot_main_relstate.pyld.input.pilot_pyld_pos_inert[i], chute.eom_main[i].output.inertial_pos);
         V_COPY(chute.pilot_main_relstate.pyld.input.pilot_pyld_vel_inert[i], chute.eom_main[i].output.inertial_vel);                 
         V_COPY(chute.pilot_main_relstate.pyld.input.pilot_pyld_inert_rate[i], chute.eom_main[i].output.body_rates);                  
         M_COPY(chute.pilot_main_relstate.pyld.input.pyld_T_body_from_inert, chute.eom_main[i].output.T_inertial_to_body);
         V_COPY(chute.pilot_main_relstate.pyld.input.pyld_cg_sr, chute.param.zero_vector);
         V_COPY(chute.pilot_main_relstate.chute[i].input.chute_pos_inert, chute.eom_pilot[i].output.inertial_pos);
         V_COPY(chute.pilot_main_relstate.chute[i].input.chute_vel_inert, chute.eom_pilot[i].output.inertial_vel);
         V_COPY(chute.pilot_main_relstate.chute[i].input.chute_inert_rate, chute.eom_pilot[i].output.body_rates);
         M_COPY(chute.pilot_main_relstate.chute[i].input.chute_T_body_from_inert, chute.eom_pilot[i].output.T_inertial_to_body);
         V_COPY(chute.pilot_main_relstate.chute[i].input.chute_wind_vel_inert, chute.wind_vel_inertial);

         chute.pilot_main_relstate.chute[i].input.chute_atm_density     = chute.input.atm_density;
         chute.pilot_main_relstate.chute[i].input.chute_speed_of_sound  = chute.input.atm_sos;
         chute.pilot_main_relstate.chute[i].input.chute_radius          = chute.param.pilot_diameter/2.0;
      }

      //Code to populate rel state confluence pt input data structure
      chute.param.has_confl_point        = false;
      chute.pilot_main_system            = true;
      chute.param.riser_line_length      = chute.param.pilot_riser_length;
      chute.param.suspension_line_length = chute.param.pilot_suspension_length;

      //Compute Relative State of each body: pilot/main
      parachute_relstate( &chute.pilot_main_relstate.pyld,
                          &chute );

      //Code to populate rel state input data structure for main/confluence pt/CM system
      for (i = 0; i < chute.param.num_chutes; i++) {
         V_COPY(chute.relstate.pyld.input.pyld_pos_inert, chute.input.pyld_inertial_pos);
         V_COPY(chute.relstate.pyld.input.pyld_vel_inert, chute.input.pyld_inertial_vel);
         V_COPY(chute.relstate.pyld.input.pyld_inert_rate, chute.input.pyld_inertial_rate);
         M_COPY(chute.relstate.pyld.input.pyld_T_body_from_inert, chute.input.pyld_T_inertial_to_body);
         V_COPY(chute.relstate.pyld.input.pyld_cg_sr, chute.input.cg);
         V_COPY(chute.relstate.chute[i].input.chute_pos_inert, chute.eom_main[i].output.inertial_pos);
         V_COPY(chute.relstate.chute[i].input.chute_vel_inert, chute.eom_main[i].output.inertial_vel);
         V_COPY(chute.relstate.chute[i].input.chute_inert_rate, chute.eom_main[i].output.body_rates);
         M_COPY(chute.relstate.chute[i].input.chute_T_body_from_inert, chute.eom_main[i].output.T_inertial_to_body);
         V_COPY(chute.relstate.chute[i].input.chute_wind_vel_inert, chute.wind_vel_inertial);
         chute.relstate.chute[i].input.chute_atm_density             = chute.input.atm_density;
         chute.relstate.chute[i].input.chute_speed_of_sound          = chute.input.atm_sos;
         chute.relstate.chute[i].input.chute_radius                  = chute.param.main_diameter/2.0;

      }
      //Code to populate rel state confluence pt input data structure
      if (chute.param.model_confl_point == true) {
         chute.param.has_confl_point = true;
      }
      else {
         chute.param.has_confl_point = false;
      }

      chute.pilot_main_system            = false;
      chute.param.riser_line_length      = chute.param.main_riser_length; ;
      chute.param.suspension_line_length = chute.param.main_suspension_length;

      V_COPY(chute.relstate.confl.input.confl_pnt_pos_inert, chute.eom_confpt.output.inertial_pos);
      V_COPY(chute.relstate.confl.input.confl_pnt_vel_inert, chute.eom_confpt.output.inertial_vel);

      //Compute Relative State of each body: main/confluence pt/CM
      parachute_relstate(&chute.relstate.pyld,
                         &chute );
   }

   else {

      //Code to populate rel state chute input data structure
      for (i = 0; i < chute.param.num_chutes; i++) {
         V_COPY(chute.relstate.pyld.input.pyld_pos_inert, chute.input.pyld_inertial_pos);
         V_COPY(chute.relstate.pyld.input.pyld_vel_inert, chute.input.pyld_inertial_vel);
         V_COPY(chute.relstate.pyld.input.pyld_inert_rate, chute.input.pyld_inertial_rate);
         M_COPY(chute.relstate.pyld.input.pyld_T_body_from_inert, chute.input.pyld_T_inertial_to_body);
         V_COPY(chute.relstate.pyld.input.pyld_cg_sr, chute.input.cg);
         V_COPY(chute.relstate.chute[i].input.chute_pos_inert, chute.eom_main[i].output.inertial_pos);
         V_COPY(chute.relstate.chute[i].input.chute_vel_inert, chute.eom_main[i].output.inertial_vel);
         V_COPY(chute.relstate.chute[i].input.chute_inert_rate, chute.eom_main[i].output.body_rates);
         M_COPY(chute.relstate.chute[i].input.chute_T_body_from_inert, chute.eom_main[i].output.T_inertial_to_body);
         V_COPY(chute.relstate.chute[i].input.chute_wind_vel_inert, chute.wind_vel_inertial);
         chute.relstate.chute[i].input.chute_atm_density             = chute.input.atm_density;
         chute.relstate.chute[i].input.chute_speed_of_sound          = chute.input.atm_sos;
         chute.relstate.chute[i].input.chute_radius                  = chute.param.main_diameter/2.0;
      }
      //Code to populate rel state confluence pt input data structure
      V_COPY(chute.relstate.confl.input.confl_pnt_pos_inert, chute.eom_confpt.output.inertial_pos);
      V_COPY(chute.relstate.confl.input.confl_pnt_vel_inert, chute.eom_confpt.output.inertial_vel);

      if (chute.param.model_confl_point == true) {
         chute.param.has_confl_point = true;
      }
      else {
         chute.param.has_confl_point = false;
      }

      chute.param.riser_line_length      = chute.param.main_riser_length; ;
      chute.param.suspension_line_length = chute.param.main_suspension_length;

      //Compute Relative State of each body:
      //   Chute Bag (Only needed during chute line stretch)
      //   Confluence Pt
      //   Chute Body
      parachute_relstate(&chute.relstate.pyld,
                         &chute );

   }

}

