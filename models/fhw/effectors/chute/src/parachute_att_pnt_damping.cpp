/******************************* TRICK HEADER **********************************
PURPOSE:
    (To recalculate direction of chute riser force vector to be along velocity 
     vector of the chute attach point. This allows the model to achieve expected damping 
     of the pyld (CM) under the drogues. This damping model is only applicable to small
     chutes with low mass/inertias, hence its output is used only when the appropriate 
     flag is set to true. In MPCV this flag is set only when drogues are active) 
REFERENCE:
    (( -- ))                                   
ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))
LIBRARY DEPENDENCY:
    (
    )

PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (July 2011) (Initial Version))            
*******************************************************************************/

/*
 * Include files for package
 */

#include "../include/parachute_att_pnt_damping.hh"

#include "trick/trick_math.h"

/* Function prototypes */

void parachute_att_pnt_damping (ATT_PNT_DATA& apd) 
                        
{

  //Calculate the attach point moment arm in the body frame
  MxV(apd.work.att_pnt_moment_arm_body, apd.input.pyld_T_body_from_SR, apd.input.pyld_att_pnt_moment_arm);

  //Compute Att Pnt Velocity Vector
  V_CROSS(apd.work.att_pnt_vel_rel, apd.input.pyld_body_rates, apd.work.att_pnt_moment_arm_body);
  V_ADD(apd.work.att_pnt_vel_body, apd.work.att_pnt_vel_rel, apd.input.VairB);
  apd.work.att_pnt_vel_mag = V_MAG(apd.work.att_pnt_vel_body);
  
  //Protect against divide by 0
  double tol = 1e-9; 
  if ( apd.work.att_pnt_vel_mag < tol ) apd.work.att_pnt_vel_mag = tol; 

  //Compute riser force in CM body frame. Force is in opposite direction of att pnt velocity vector, hence the -1.0 multiplication
  apd.work.att_pnt_force_body[0] = 
     apd.input.load_mag * (-1.0*((apd.work.att_pnt_vel_body[0]/apd.work.att_pnt_vel_mag)));

  apd.work.att_pnt_force_body[1] = 
     apd.input.load_mag * (-1.0*((apd.work.att_pnt_vel_body[1]/apd.work.att_pnt_vel_mag)));

  apd.work.att_pnt_force_body[2] = 
     apd.input.load_mag * (-1.0*((apd.work.att_pnt_vel_body[2]/apd.work.att_pnt_vel_mag)));

  //Rotate riser force from CM body frame to CM SR frame 
  MtxV(apd.output.att_pnt_force_SR, apd.input.pyld_T_body_from_SR, apd.work.att_pnt_force_body);

  //Compute riser moment
  V_CROSS(apd.output.att_pnt_moment_SR, 
          apd.input.pyld_att_pnt_moment_arm,  
          apd.output.att_pnt_force_SR); 

}

