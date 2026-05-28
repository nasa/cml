/******************************* TRICK HEADER **********************************
PURPOSE:
    (Parachute relative state function)

REFERENCE:
    (())

ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))

LIBRARY DEPENDENCY:
     (())
    
PROGRAMMERS:
    (((C. Holtz, A. Barth) (LMSSS) (04-01-09) (Initial Version for hi-fi chute model)))
    (((J. Semrau)          (HON)   (May 2010) (Osiris Release 763)))                    
    (((J. Semrau)          (HON)   (Aug 2010) (Osiris Release 824-corrected relstate computation for pilot chutes)))                    
    (((J. Semrau)          (HON)   (Jan 2011) (Release 11.02, CR 859, added code to determine canopy extraction)))               
    (((J. Semrau)          (HON)   (Jan 2013) (LMBP#2290, Correction canopy extraction of pilot chute)))               
    (((J. Semrau)          (HON)   (Nov 2015) (RCR4609, Updates for new integration method)))               
*******************************************************************************/

#include <cmath>

#include "trick/trick_math.h"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/parachute_exec.hh"
#include "../include/parachute_proto.hh"

int parachute_relstate (                             /* RETURN: -- Return non-zero on error */
                        PYLD_RELSTATE   *RS_PYLD, 
                        PARACHUTE_DATA    *C)           /* INOUT:  -- Parachute structure */

{

   if (!RS_PYLD || !C) {
      CMLMessage::fail(__FILE__,__LINE__,"Information only: \n","pointer is null \n");
      return(0);
   }

   /* Relstate chute data structure pointer */
   CHUTE_RELSTATE      * RS_CHUTE[MAX_ITEMS::MAX_CHUTES];
   CONFL_RELSTATE      * RS_CONFL;

   int    i, ret ;
   double zerodvec[3] = {0.0, 0.0, 0.0} ;
   double tol, x_axis[3];
   tol = 1e-9;

   if (C->pilot_main_system == true) {
      RS_CONFL = &(C->pilot_main_relstate.confl);
   }
   else {
      RS_CONFL = &(C->relstate.confl);
   }
   
   /************************************************************/
   /**************** Process chute states  *********************/
   /************************************************************/
   
   /* Loop on each chute in the system */
   for ( i = 0 ; i < C->param.num_chutes ; i++ ) {

       /* Assign pointer for chute structure */
      if (C->pilot_main_system == true) {
         RS_CHUTE[i] = &(C->pilot_main_relstate.chute[i]);
      }  
      else {
         RS_CHUTE[i] = &(C->relstate.chute[i]);
      }
   
      /* Compute the chute CG location in SR frame.  This changes due to inflation spreading
         the chute.  The CG is assumed to be at the bottom of the chute skirt which is assumed
         to be the location of the suspension lines.  Based on the length of the suspension
         lines and the current radius of the chute (from the inflation model), we compute
         the distance from the chute attach point to the bottom of the skirt.  The chute
         is assumed axisymmetric, so Y/Z=0 */
      RS_CHUTE[i]->output.chute_cg_sr[1] = RS_CHUTE[i]->output.chute_cg_sr[2] = 0.0 ;
      RS_CHUTE[i]->output.chute_cg_sr[0] = 
         std::sqrt(C->param.suspension_line_length * C->param.suspension_line_length -
                   RS_CHUTE[i]->input.chute_radius * RS_CHUTE[i]->input.chute_radius) ;
      
      if (RS_CHUTE[i]->work.init_flag == 0) {
          M_COPY(RS_CHUTE[i]->input.chute_T_body_from_inert,  RS_PYLD->input.pyld_T_body_from_inert);
          RS_CHUTE[i]->work.init_flag = 1;
      }
      
      /* Compute chute attach point states in inertial frame */
      /* Call the generic attach point state function (each chute has 1 defined 
         attach point at the suspension line confluence point */
      ret = ap_inertial_states(RS_CHUTE[i]->output.chute_cg_sr, 
                               RS_CHUTE[i]->input.chute_pos_inert,
                               RS_CHUTE[i]->input.chute_vel_inert, 
                               RS_CHUTE[i]->input.chute_inert_rate,
                               RS_CHUTE[i]->input.chute_T_body_from_inert, 
                               RS_CHUTE[i]->param.chute_T_body_from_sr,
                               zerodvec,
                               RS_CHUTE[i]->output.chute_att_pnt_pos_inert, 
                               RS_CHUTE[i]->output.chute_att_pnt_vel_inert,
                               RS_CHUTE[i]->output.chute_wxr_vel_inert, 
                               RS_CHUTE[i]->output.chute_moment_arm) ;

      /* Check the return value */
      if ( ret != 0 ) return (ret) ;

      /************************************************************/
      /************ Process chute aerodynamic data  ***************/
      /************************************************************/
      /* Compute wind relative velocity in inertial, body, SR, and aero frames */
      V_SUB(RS_CHUTE[i]->output.chute_wrel_vel_inert, 
            RS_CHUTE[i]->input.chute_vel_inert, 
            RS_CHUTE[i]->input.chute_wind_vel_inert);
      MxV(RS_CHUTE[i]->output.chute_wrel_vel_body, 
          RS_CHUTE[i]->input.chute_T_body_from_inert, 
          RS_CHUTE[i]->output.chute_wrel_vel_inert);
      MtxV(RS_CHUTE[i]->output.chute_wrel_vel_sr, 
           RS_CHUTE[i]->param.chute_T_body_from_sr, 
           RS_CHUTE[i]->output.chute_wrel_vel_body);
      MtxV(RS_CHUTE[i]->output.chute_wrel_vel_aero, 
           RS_CHUTE[i]->param.chute_T_aero_from_sr, 
           RS_CHUTE[i]->output.chute_wrel_vel_sr);

      /* Compute wind relative velocity magnitude and unit vector in SR frame */
      RS_CHUTE[i]->output.chute_wrel_vel_mag = V_MAG(RS_CHUTE[i]->output.chute_wrel_vel_sr);
      V_SCALE(RS_CHUTE[i]->output.chute_wrel_vel_uvec_sr, 
              RS_CHUTE[i]->output.chute_wrel_vel_sr, 
              (1.0/RS_CHUTE[i]->output.chute_wrel_vel_mag)) ;

      /* Compute dynamic pressure */
      RS_CHUTE[i]->output.chute_dynamic_pressure = 
         0.5 * RS_CHUTE[i]->input.chute_atm_density * 
         RS_CHUTE[i]->output.chute_wrel_vel_mag * RS_CHUTE[i]->output.chute_wrel_vel_mag;

      /* Separation distance from payload to chute */
      V_SUB(RS_CHUTE[i]->work.chute_sep_distance, 
            RS_PYLD->input.pyld_pos_inert, 
            RS_CHUTE[i]->input.chute_pos_inert); 
      RS_CHUTE[i]->output.chute_sep_distance_mag = V_MAG(RS_CHUTE[i]->work.chute_sep_distance);

      /* Compute total angle of attack */
      x_axis[0] = 1;
      x_axis[1] = 0;
      x_axis[2] = 0;

      double alpha_dp_result;

      alpha_dp_result = V_DOT(RS_CHUTE[i]->output.chute_wrel_vel_aero, x_axis) / V_MAG(RS_CHUTE[i]->output.chute_wrel_vel_aero);
      if (alpha_dp_result >  1.0) alpha_dp_result =  1.0 - tol;
      if (alpha_dp_result < -1.0) alpha_dp_result = -1.0 + tol;
      RS_CHUTE[i]->output.chute_alpha_total =  std::acos(alpha_dp_result);

      /* Compute Mach number */
      if (RS_CHUTE[i]->input.chute_speed_of_sound > tol) {
          RS_CHUTE[i]->output.chute_mach = RS_CHUTE[i]->output.chute_wrel_vel_mag / RS_CHUTE[i]->input.chute_speed_of_sound;
      } else {
          RS_CHUTE[i]->output.chute_mach = 0.0;
      }

   }
   
   /************************************************************/
   /********** Process payload attach point states  ************/
   /************************************************************/
   
   if (C->pilot_main_system == false) {
   for ( i = 0 ; i < C->param.num_pyld_attach_points ; i++ ) {
   
      /* Call the generic attach point state function */
      ret = ap_inertial_states(RS_PYLD->input.pyld_cg_sr, 
                               RS_PYLD->input.pyld_pos_inert, 
                               RS_PYLD->input.pyld_vel_inert, 
                               RS_PYLD->input.pyld_inert_rate,
                               RS_PYLD->input.pyld_T_body_from_inert, 
                               RS_PYLD->param.pyld_T_body_from_sr,
                               RS_PYLD->param.pyld_att_pnt_sr[i],
                               RS_PYLD->output.pyld_att_pnt_pos_inert[i],
                               RS_PYLD->output.pyld_att_pnt_vel_inert[i],
                               RS_PYLD->work.pyld_att_pnt_wxr_vel_inert[i], 
                               RS_PYLD->output.pyld_att_pnt_moment_arm[i]) ;
                               
      /* Check the return value */
      if ( ret != 0 ) return (ret) ;
   }

   }
   else {
      for ( i = 0 ; i < C->param.num_pyld_attach_points ; i++ ) {

         /* Call the generic attach point state function */
         ret = ap_inertial_states(RS_PYLD->input.pyld_cg_sr,
                                  RS_PYLD->input.pilot_pyld_pos_inert[i],
                                  RS_PYLD->input.pilot_pyld_vel_inert[i],
                                  RS_PYLD->input.pilot_pyld_inert_rate[i],
                                  RS_PYLD->input.pyld_T_body_from_inert,
                                  RS_PYLD->param.pyld_T_body_from_sr,
                                  RS_PYLD->param.pyld_att_pnt_sr[i],
                                  RS_PYLD->output.pyld_att_pnt_pos_inert[i],
                                  RS_PYLD->output.pyld_att_pnt_vel_inert[i],
                                  RS_PYLD->work.pyld_att_pnt_wxr_vel_inert[i],
                                  RS_PYLD->output.pyld_att_pnt_moment_arm[i]) ;

         /* Check the return value */
         if ( ret != 0 ) return (ret) ;
      }
  }

   /************************************************************/
   /****************** Process line states  ********************/
   /************************************************************/
   
   /* If we have a confluence point, then we need to handle both harness and riser lines,
      otherwise we just the riser lines connected directly to the payload.  Because
      we will always apply line forces to the payload and chutes (whether there is a
      confluence point or not), we will compute the line vectors from the payload
      and chutes and will need to negate the vectors to apply forces to the
      confluence point. Stretch rates are computed as delta velocity along the
      force vector direction and positive for increasing distance. */
   if ( C->param.has_confl_point ) {
   
      /* Compute harness line states from payload attach points to the confluence point (assumed
         to be a single point in space) */
      for ( i = 0 ; i < C->param.num_harness_lines ; i++ ) {
      
         /* Line length */
         /* Inertial vector from payload attach point to confluence point */
         V_SUB(RS_PYLD->output.pyld_ap_force_vec_inert[i], 
               RS_PYLD->output.pyld_att_pnt_pos_inert[i], 
               RS_CONFL->input.confl_pnt_pos_inert) ;
         /* Line distance is the magnitude of the vector */
         RS_PYLD->output.harness_line_distance[i] = V_MAG(RS_PYLD->output.pyld_ap_force_vec_inert[i]) ;
         /* Make the force vector a unit vector */
         V_SCALE(RS_PYLD->output.pyld_ap_force_vec_inert[i], 
                 RS_PYLD->output.pyld_ap_force_vec_inert[i], 
                 (1.0/RS_PYLD->output.harness_line_distance[i])) ;
   
         /* Line stretch rate */
         /* Relative inertial velocity between payload attach point and confluence point */
         V_SUB(RS_PYLD->work.harness_line_inertial_dv[i], 
               RS_PYLD->output.pyld_att_pnt_vel_inert[i], 
               RS_CONFL->input.confl_pnt_vel_inert) ;
         /* Stretch rate is the component of the velocity in the direction of the line force vector */
         RS_PYLD->output.harness_line_stretch_rate[i] = 
            V_DOT(RS_PYLD->output.pyld_ap_force_vec_inert[i], RS_PYLD->work.harness_line_inertial_dv[i]) ;

            /* Set flag when the harness has achieved line stretch */
         if (!RS_PYLD->output.harness_line_stretch[i] && 
             RS_PYLD->output.harness_line_distance[i] >= C->param.harness_line_length[i]) {
             RS_PYLD->output.harness_line_stretch[i] = true;
         }
      } 
   
      /* Compute riser line states from chute attach points to the confluence point (assumed
         to be a single point in space) */

      //Code to copy over confluence point pos/vel to temp variable, 
      //done to handle cases where there are multitple confluence points

      for ( i = 0 ; i < C->param.num_chutes ; i++ ) {

         double temp_inertial_pos[3];
         double temp_inertial_vel[3];

         V_COPY(temp_inertial_pos, RS_CONFL->input.confl_pnt_pos_inert);
         V_COPY(temp_inertial_vel, RS_CONFL->input.confl_pnt_vel_inert);
      
         /* Line length */
         
         /* Inertial vector from chute attach point to confluence point */
         V_SUB(RS_CHUTE[i]->output.chute_ap_force_vec_inert, 
               temp_inertial_pos,                       
               RS_CHUTE[i]->output.chute_att_pnt_pos_inert) ;
         /* Line distance is the magnitude of the vector */
         RS_CHUTE[i]->output.riser_line_distance = V_MAG(RS_CHUTE[i]->output.chute_ap_force_vec_inert) ;
         /* Make the force vector a unit vector */
         V_SCALE(RS_CHUTE[i]->output.chute_ap_force_vec_inert, 
                 RS_CHUTE[i]->output.chute_ap_force_vec_inert, 
                 (1.0/RS_CHUTE[i]->output.riser_line_distance)) ;

         /////////////////////////////////////////////////

         /* NEW CODE TO COMPUTE DISTANCE FROM CG TO CF */
         /* Inertial vector from chute attach point to confluence point */
         V_SUB(RS_CHUTE[i]->output.chute_cg_force_vec_inert,
               temp_inertial_pos,                    
               RS_CHUTE[i]->input.chute_pos_inert) ;
         /* Line distance is the magnitude of the vector */
         RS_CHUTE[i]->output.riser_suspension_distance = V_MAG(RS_CHUTE[i]->output.chute_cg_force_vec_inert) ;
         /* Make the force vector a unit vector */
         V_SCALE(RS_CHUTE[i]->output.chute_cg_force_vec_inert,
                 RS_CHUTE[i]->output.chute_cg_force_vec_inert,
                 (1.0/RS_CHUTE[i]->output.riser_suspension_distance)) ;

         /////////////////////////////////////////////////

         /* Line stretch rate */
         /* Relative inertial velocity between chute attach point and confluence point */
         V_SUB(RS_CHUTE[i]->work.riser_line_inertial_dv, 
               temp_inertial_vel,                        
               RS_CHUTE[i]->output.chute_att_pnt_vel_inert) ;
         /* Stretch rate is the component of the velocity in the direction of the line force vector */
         RS_CHUTE[i]->output.riser_line_stretch_rate = 
            V_DOT(RS_CHUTE[i]->output.chute_ap_force_vec_inert, RS_CHUTE[i]->work.riser_line_inertial_dv) ;

         /////////////////////////////////////////////////

         /* NEW CODE TO COMPUTE VEL FROM CG TO CF */

         /* Line stretch rate */
         /* Relative inertial velocity between chute attach point and confluence point */
         V_SUB(RS_CHUTE[i]->work.riser_suspension_inertial_dv,
              temp_inertial_vel,                     
              RS_CHUTE[i]->input.chute_vel_inert) ;
         /* Stretch rate is the component of the velocity in the direction of the line force vector */
         RS_CHUTE[i]->output.riser_suspension_stretch_rate =
           V_DOT(RS_CHUTE[i]->output.chute_cg_force_vec_inert, RS_CHUTE[i]->work.riser_suspension_inertial_dv) ;

         /////////////////////////////////////////////////

         V_SUB(RS_CHUTE[i]->work.chute_att_pnt_clearance, 
               temp_inertial_pos,                        
               RS_CHUTE[i]->input.chute_pos_inert);
         RS_CHUTE[i]->work.chute_att_pnt_clearance_mag = V_MAG(RS_CHUTE[i]->work.chute_att_pnt_clearance);

         /* Set flag when the chute has achieved line stretch */
         if (RS_CHUTE[i]->work.chute_att_pnt_clearance_mag > C->param.suspension_line_length) {
            if (!RS_CHUTE[i]->output.chute_line_stretch && 
               RS_CHUTE[i]->output.riser_line_distance >= C->param.riser_line_length) { 
               RS_CHUTE[i]->output.chute_line_stretch = true;
               RS_CHUTE[i]->output.riser_line_stretch_rate = 0.0; 
            }
         }
      } 
   }
   else {   /* No confluence point */

      int    attPntIndex;

      /* Compute riser line states from chute attach points to the payload points */
      for ( i = 0 ; i < C->param.num_chutes ; i++ ) {

         //Code to set attach point index to correct value
         if (C->param.num_pyld_attach_points == 1) {
            attPntIndex = 0;
         }
         else {
            attPntIndex = i;
         }

         /* Line length */
         /* Inertial vector from chute attach point to payload attach point */
         V_SUB(RS_CHUTE[i]->output.chute_ap_force_vec_inert, 
               RS_PYLD->output.pyld_att_pnt_pos_inert[attPntIndex], 
               RS_CHUTE[i]->output.chute_att_pnt_pos_inert) ;
         /* Line distance is the magnitude of the vector */
         RS_CHUTE[i]->output.riser_line_distance = V_MAG(RS_CHUTE[i]->output.chute_ap_force_vec_inert) ;
         /* Make the force vector a unit vector */
         V_SCALE(RS_CHUTE[i]->output.chute_ap_force_vec_inert, 
                 RS_CHUTE[i]->output.chute_ap_force_vec_inert, 
                 (1.0/RS_CHUTE[i]->output.riser_line_distance)) ;

         /////////////////////////////////////////////////

         /* NEW CODE TO COMPUTE DISTANCE FROM CHUTE CG TO ATTACH POINT */
         /* Inertial vector from chute attach point to confluence point */
         V_SUB(RS_CHUTE[i]->output.chute_cg_force_vec_inert,
               RS_PYLD->output.pyld_att_pnt_pos_inert[attPntIndex], 
               RS_CHUTE[i]->input.chute_pos_inert) ;
         /* Line distance is the magnitude of the vector */
         RS_CHUTE[i]->output.riser_suspension_distance = V_MAG(RS_CHUTE[i]->output.chute_cg_force_vec_inert) ;
         /* Make the force vector a unit vector */
         if (RS_CHUTE[i]->output.riser_suspension_distance > 0) { 
         
         V_SCALE(RS_CHUTE[i]->output.chute_cg_force_vec_inert,
                 RS_CHUTE[i]->output.chute_cg_force_vec_inert,
                 (1.0/RS_CHUTE[i]->output.riser_suspension_distance)) ;

          }

         /////////////////////////////////////////////////


         /* Relative inertial velocity between chute attach point and payload attach point */
         V_SUB(RS_CHUTE[i]->work.riser_line_inertial_dv, 
               RS_PYLD->output.pyld_att_pnt_vel_inert[attPntIndex],  
               RS_CHUTE[i]->output.chute_att_pnt_vel_inert) ;
         /* Line stretch rate */
         /* Stretch rate is the component of the velocity in the direction of the line force vector */
         RS_CHUTE[i]->output.riser_line_stretch_rate = 
            V_DOT(RS_CHUTE[i]->output.chute_ap_force_vec_inert, RS_CHUTE[i]->work.riser_line_inertial_dv) ;

         /////////////////////////////////////////////////

         /* NEW CODE TO COMPUTE VEL FROM CHUTE CG TO ATTACH POINT */

         /* Line stretch rate */
         /* Relative inertial velocity between chute attach point and confluence point */
         V_SUB(RS_CHUTE[i]->work.riser_suspension_inertial_dv,
               RS_PYLD->output.pyld_att_pnt_vel_inert[attPntIndex],  
               RS_CHUTE[i]->input.chute_vel_inert) ;
         /* Stretch rate is the component of the velocity in the direction of the line force vector */
         RS_CHUTE[i]->output.riser_suspension_stretch_rate =
           V_DOT(RS_CHUTE[i]->output.chute_cg_force_vec_inert, RS_CHUTE[i]->work.riser_suspension_inertial_dv) ;

         /////////////////////////////////////////////////


         //Following computation fixes chute deploy issue where suspension line length > riser line length
         //Compute Distance from chute pos to att pnt pos
         double delta_pos[3];
         V_SUB(delta_pos, RS_PYLD->output.pyld_att_pnt_pos_inert[attPntIndex], RS_CHUTE[i]->input.chute_pos_inert);
         double delta_pos_mag = V_MAG(delta_pos);

         if (delta_pos_mag > C->param.suspension_line_length) {
         /* Set flag when the chute has achieved line stretch */
            if (!RS_CHUTE[i]->output.chute_line_stretch && 
               RS_CHUTE[i]->output.riser_line_distance >= C->param.riser_line_length) {
               RS_CHUTE[i]->output.chute_line_stretch = true;
            }
         }
      }
   }

   return(0) ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int ap_inertial_states(
   double veh_cg_sr[3],                /* Veh CG in SR frame */
   double veh_pos[3],                  /* Inertial position of veh CG */
   double veh_vel[3],                  /* Inertial velocity of veh CG */
   double veh_omega[3],                /* Inertial rates of veh in body frame relative to inertial */
   double veh_T_body_from_inert[3][3], /* Transform from inertial to vehicle body */
   double veh_T_body_from_sr[3][3],    /* Transform from veh SR to veh body */
   double ap_sr[3],                    /* Attach point location in veh SR */
   double ap_pos_inert[3],               /* OUT: Attach point inertial position */
   double ap_vel_inert[3],               /* OUT: Attach point inertial velocity */
   double veh_wxr_vel_inert[3],          /* OUT:  Rotational velocity component (w X r) in inertial coordinates */
   double ap_rel_pos_sr[3])              /* OUT: Position of attach point relative to veh CG in SR coordinates */
{

   if (!veh_cg_sr || !veh_pos || !veh_vel || !veh_omega || !veh_T_body_from_inert || !veh_T_body_from_sr || 
       !ap_sr || !ap_pos_inert || !ap_vel_inert || !veh_wxr_vel_inert || !ap_rel_pos_sr) {
      CMLMessage::fail(__FILE__,__LINE__,"Information only: \n","pointer is null \n");
      return(0);
   }

   double ap_rel_pos_body[3], ap_rel_pos_inert[3];
   double veh_omega_inert[3];

   /* Compute the position of the attach point relative to the veh CG in SR coordinates */
   V_SUB(ap_rel_pos_sr, ap_sr, veh_cg_sr);

   /* Compute position of the attach point relative to the veh CG in inertial coordinates */
   MxV(ap_rel_pos_body, veh_T_body_from_sr, ap_rel_pos_sr);
   MtxV(ap_rel_pos_inert, veh_T_body_from_inert, ap_rel_pos_body);

   /* Compute the attach point location in inertial coordinates */ 
   V_ADD(ap_pos_inert, veh_pos, ap_rel_pos_inert);

   /* Compute rotational velocity component of the attach point */
   MtxV(veh_omega_inert, veh_T_body_from_inert, veh_omega);
   V_CROSS(veh_wxr_vel_inert, veh_omega_inert, ap_rel_pos_inert); 

   /* Compute velocity of the attach point in inertial coordinates */
   V_ADD(ap_vel_inert, veh_vel, veh_wxr_vel_inert);

   return(0);
}
