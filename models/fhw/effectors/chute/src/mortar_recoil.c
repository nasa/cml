/******************************* TRICK HEADER **********************************
PURPOSE:
    (The Chute Mortar recoil model is called when a chute mortar has fired.  
     The resulting forces and moments from firing the mortar are 
     computed and applied to the CM.)
REFERENCE:
    ((  TBD ))                 

PROGRAMMERS:
    (((Jeff Semrau) (Honeywell) (Dec 2009))
     ((Bingquan Wang) (OSR) (5/17) (ANTARES) (Removed the library dependency
                  on table_interp)) 
    )
*******************************************************************************/

#include "trick/constant.h"      
#include "trick/exec_proto.h"            /* For get_job_cycle routine */
#include "trick/trick_math.h"
#include "../include/mortar.h"

void mortar_recoil (double          LRS_origin_in_SR_Frame[3],
                    double          LRS_to_SR_rotation[3][3],
                    MORTAR_DATA    *chute_mortar)
          
{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in mortar_recoil.c \n";
    if (!chute_mortar || !LRS_origin_in_SR_Frame || !LRS_to_SR_rotation) {
       printf(error_msg);
       return;
    }

   //Copy PEC command to local parameter, this is so model will always know
   //when PEC was fired. 
   if ( chute_mortar->input.mortar_fired == 1 ) {
      chute_mortar->work.mortar_deployed = true;
   }

   //Use local copy of PEC command to determine whether to run or not. 
   //Using this variable because using the PEC command directly might make 
   //model just run for a couple of cycles, since the PEC command may only be set 
   //to On for a cycle or two. This implementation ensures the model runs after
   //PEC command was set to On. 
   if (chute_mortar->work.mortar_deployed == false) return;

   if (chute_mortar->work.first_pass == true) {
      chute_mortar->work.start_time  = chute_mortar->input.sim_time_local;
      chute_mortar->work.first_pass  = false;
   }

   if (chute_mortar->work.impulse_finished == true) {
      V_INIT(chute_mortar->output.mortar_forces); 
      V_INIT(chute_mortar->output.mortar_moments); 
      chute_mortar->output.firing_complete   = true;
      return;
   }

   if (chute_mortar->param.delay_mortar == true) { //Code to handle staggered mortar fire (CEV drogues)
      double time_tol = 1e-10; // To ensure mortar fire occurs on desired frame, not one frame late.
      chute_mortar->work.elasped_time = chute_mortar->input.sim_time_local - chute_mortar->work.start_time;
      if (chute_mortar->work.elasped_time >= (chute_mortar->param.delay_time - time_tol)) {
         chute_mortar->work.start_impulse = true;
         chute_mortar->work.vel_first_pass = true;
      }
   }
   else { 
      chute_mortar->work.start_impulse = true;
      chute_mortar->work.vel_first_pass = true;
   }

   //Mortar Recoil now only runs 1 execution frame
   //LRS only supplies mortar impulse data now, no duration, so assuming impulse last for one execution frame now
   if (chute_mortar->work.start_impulse == true) {

     double cos_elevation, sin_elevation, cos_azimuth, sin_azimuth;
     cos_elevation = cos(chute_mortar->param.mortar_elevation * DTR); 
     sin_elevation = sin(chute_mortar->param.mortar_elevation * DTR); 
     cos_azimuth   = cos(chute_mortar->param.mortar_azimuth * DTR); 
     sin_azimuth   = sin(chute_mortar->param.mortar_azimuth * DTR); 

     //Compute Direction of Mortar Force in SR Frame
     chute_mortar->work.mortar_direction_LRS[0] = sin_elevation;
        
     chute_mortar->work.mortar_direction_LRS[1] = cos_elevation * sin_azimuth;

     chute_mortar->work.mortar_direction_LRS[2] = -cos_elevation * cos_azimuth;

     MxV(chute_mortar->work.mortar_direction_SR, 
         LRS_to_SR_rotation, 
         chute_mortar->work.mortar_direction_LRS);

     //Transform Mortar Position from LRS Frame to SR Frame
     V_ADD(chute_mortar->work.mortar_position_SR_frame_temp, 
           chute_mortar->param.mortar_position_LRS_frame, 
           LRS_origin_in_SR_Frame);
     MxV(chute_mortar->work.mortar_position_SR_frame, 
         LRS_to_SR_rotation, 
         chute_mortar->work.mortar_position_SR_frame_temp);

     //Compute Moment Arms for Drogue Mortars
     V_SUB(chute_mortar->work.mortar_moment_arms, 
           chute_mortar->work.mortar_position_SR_frame, 
           chute_mortar->input.CG_position ); 
 
      //Compute Mortar Force based on Documented Impulse and 1 execution cycle
      chute_mortar->work.force = chute_mortar->param.impulse / exec_get_job_cycle(0);

      //Compute Mortar Force X, Y, Z Components in SR Frame
      V_SCALE(chute_mortar->output.mortar_forces, 
              chute_mortar->work.mortar_direction_SR, 
              chute_mortar->work.force);

      //Compute X,Y and Z Moment components of mortar in SR frame
      V_CROSS(chute_mortar->output.mortar_moments, 
              chute_mortar->work.mortar_moment_arms, 
              chute_mortar->output.mortar_forces);
     
     chute_mortar->work.impulse_finished = true;
     chute_mortar->work.start_impulse = false; 
   }

}
