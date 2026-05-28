/******************************* TRICK HEADER **********************************
PURPOSE:
    (This routine is called from the routine, parachute_exec, to calculate the twist 
     torque the chute lines impart on the CM.)                 
REFERENCE:
    (( ))

ASSUMPTIONS AND LIMITATIONS:
    ((None))
LIBRARY DEPENDENCY:
   (
      (parachute_waveform.o)
   )

PROGRAMMERS:
    (
    ((Jeff Semrau)  (Honeywell) (Feb 2010) (OSIRIS 10.04 Release)) 
    ((Mark Kane)    (NASA-JSC)  (31-Mar-2011) (New Riser Twist Torque Model based on Flyout Angle))
    ((Brian Bihari) (ESCG)      (29-Apr-2011) (New Riser Twist Torque Model based on Flyout Angle))
    ((Jeff Semrau)  (Honeywell) (28-Jul-2011) (Updated phi_star to new value per LRS Model Memo 8))
    ((Brian Bihari) (ESCG)      (26-Apr-2012) (Main Flyout Model updates per LRS Model Memo 9    ))
    ((Jeff Semrau)  (Honeywell) (21-Mar-2013) (Waveform updates to incorporate Time Varying CdS  ))
    ((Jeff Semrau)  (Honeywell) (04/2015) (CR2673) (Updated twist torque model for textile risers))
    )

*******************************************************************************/

#include "../include/parachute_twist_torque.h"
#include "../include/parachute_eom.h"
#include "trick/constant.h"              /* For unit conversions */
#include "cml/models/utilities/table_interp/include/osiris_math.h"
#include "trick/vector_macros.h"
#include "trick/matrix_macros.h"

#define _USE_MATH_DEFINES
#include <math.h>
    
void parachute_twist_torque  (TWIST_DATA   *twist) 
{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in parachute_twist_torque.c \n";
    if (!twist) { 
       printf(error_msg);
       return;
    }

    if (twist->work.twist_torque_init) {

        twist->work.twist_torque_init  = false;
        twist->work.twist_angle        = twist->param.initial_twist_angle;
        twist->work.t                  = 0.0;
    }
       
    if(!twist->work.twist_torque_init ) {


        /* Declare local variables */
        twist->work.phi_star = 3.1107;

        /* Compute twist angle */    
        twist->work.prev_twist_angle = twist->work.twist_angle;
        twist->work.twist_angle = twist->work.twist_angle + (twist->input.roll_rate * twist->input.dt);


        /* Capture sign of twist angle/roll rate */

        if (twist->work.twist_angle < 0.0) { 
           twist->work.twist_sign = -1;
        } 
        else {
           twist->work.twist_sign = 1;
        }
        if (twist->input.roll_rate < 0.0) { 
           twist->work.rate_sign = -1;
        } 
        else {
           twist->work.rate_sign = 1;
        }

        /* Compute twist torque */
        if(twist->input.chutes_inflated){

            /* Update time elapsed */
            twist->work.t = twist->work.t + twist->input.dt;

            /* Calculate vehicle weight */
            twist->work.WT = twist->input.cev_weight;

            /* Calculate nominal chute area */
            twist->work.S = M_PI * twist->input.main_diameter * twist->input.main_diameter / 4.0;

            twist->transient.work.t = twist->work.t; 
            twist->damped.work.t = twist->work.t; 
            if (twist->work.t < twist->param.t_trans) { 
               parachute_waveform(&twist->transient);
               twist->work.flyout_angle = twist->transient.work.flyout_angle;
               twist->work.Sp           = twist->transient.work.Sp;
               twist->work.CdSF         = twist->transient.param.Sp_CdSF;
            }
            else if (twist->work.t < (twist->param.t_trans + twist->param.delta_t_trans)) { 
               twist->work.fblend = (twist->work.t - twist->param.t_trans) / twist->param.delta_t_trans;
               parachute_waveform(&twist->transient);
               parachute_waveform(&twist->damped);

               twist->work.flyout_angle = 
                  twist->transient.work.flyout_angle * (1.0 - twist->work.fblend) + twist->damped.work.flyout_angle * twist->work.fblend;
               twist->work.Sp   = 
                  twist->transient.work.Sp * (1.0 - twist->work.fblend) + twist->damped.work.Sp * twist->work.fblend;
               twist->work.CdSF = 
                  twist->transient.param.Sp_CdSF * (1.0 - twist->work.fblend) + twist->damped.param.Sp_CdSF * twist->work.fblend;
            }
            else { 
               parachute_waveform(&twist->damped);
               twist->work.flyout_angle = twist->damped.work.flyout_angle;
               twist->work.Sp           = twist->damped.work.Sp;
               twist->work.CdSF         = twist->damped.param.Sp_CdSF;
            }

            twist->output.CdS =  
               (twist->param.num_chutes * twist->work.Sp * cos(twist->work.flyout_angle)) * twist->work.CdSF * twist->work.S;

            /* Do lookups of Twist Torque parameters as function of flyout angle */

            if (twist->param.num_chutes == 3) { 
               twist->work.Rf = table_lookup_1d(twist->work.flyout_angle, 
                                             twist->param.index_Rf, 
                                             twist->param.flyout_angle_3main_table, 
                                             twist->param.Rf_3main_table);
                                         
               twist->work.Rw = table_lookup_1d(twist->work.flyout_angle, 
                                             twist->param.index_Rw, 
                                             twist->param.flyout_angle_3main_table, 
                                             twist->param.Rw_3main_table);
                                            
               twist->work.Kp = table_lookup_1d(twist->work.flyout_angle, 
                                             twist->param.index_Kp, 
                                             twist->param.flyout_angle_3main_table, 
                                             twist->param.Kp_3main_table);
                                         
               twist->work.phi0 = table_lookup_1d(twist->work.flyout_angle, 
                                             twist->param.index_phi0, 
                                             twist->param.flyout_angle_3main_table, 
                                             twist->param.phi0_3main_table);
            }
            else { 
               twist->work.Rf = table_lookup_1d(twist->work.flyout_angle, 
                                             twist->param.index_Rf, 
                                             twist->param.flyout_angle_2main_table, 
                                             twist->param.Rf_2main_table);
                                         
               twist->work.Rw = table_lookup_1d(twist->work.flyout_angle, 
                                             twist->param.index_Rw, 
                                             twist->param.flyout_angle_2main_table, 
                                             twist->param.Rw_2main_table);
                                           
               twist->work.Kp = table_lookup_1d(twist->work.flyout_angle, 
                                             twist->param.index_Kp, 
                                             twist->param.flyout_angle_2main_table, 
                                             twist->param.Kp_2main_table);
                                         
               twist->work.phi0 = table_lookup_1d(twist->work.flyout_angle, 
                                             twist->param.index_phi0, 
                                             twist->param.flyout_angle_2main_table, 
                                             twist->param.phi0_2main_table);
            }


            if (twist->work.Rf > twist->work.Rw) { 
               twist->work.phi_star_fwd = (M_PI/twist->work.Kp) - (asin((twist->work.Rw/twist->work.Rf)) / twist->work.Kp);
               twist->work.phi_star_rev = (M_PI/twist->work.Kp + twist->work.phi0) - 
                                          ((asin((twist->work.Rw/twist->work.Rf)) / twist->work.Kp) + twist->work.phi0); 
            }
            else { 
               twist->work.phi_star_fwd = M_PI/(2.0 * twist->work.Kp);   
               twist->work.phi_star_rev = M_PI/(2.0 * twist->work.Kp) + twist->work.phi0;   
            }

            if ((twist->work.twist_angle * twist->input.roll_rate) >= 0.0) {
               twist->work.fwd_twist = true;
            }
            else {
               twist->work.fwd_twist = false;
            }

            if (fabs(twist->work.twist_angle) > fabs(twist->work.phi_star_fwd)) twist->work.hysteresis_flag = true;

            twist->work.twist_check = twist->work.twist_angle * twist->work.prev_twist_angle;

            if (twist->work.twist_check < 0.0) twist->work.hysteresis_flag = false;

            if ((twist->work.twist_angle - twist->work.phi0) > 0.0) {
               twist->work.phi_phi0_sign = 1;
            }
            else {
               twist->work.phi_phi0_sign = -1;
            }

            //Compute Torque
            if (((twist->work.fwd_twist == true) && (fabs(twist->work.twist_angle) < twist->work.phi_star_fwd)) ||
                ((twist->work.fwd_twist == false) && (twist->work.hysteresis_flag == false))) {

               twist->work.twist_torque_mag =
                 -twist->param.k * twist->work.Rf * sin(twist->work.Kp * twist->work.twist_angle) *
                 (twist->input.riser_load * sin(twist->work.flyout_angle));
            }
            else if ((twist->work.fwd_twist == false) &&
                     (fabs(twist->work.twist_angle) < twist->work.phi_star_rev) &&
                     (twist->work.hysteresis_flag == true)) {

               twist->work.phi_check = sin(twist->work.Kp * (fabs(twist->work.twist_angle) - twist->work.phi0));
               if (twist->work.phi_check < 0.0) {
                  twist->work.phi_check = 0.0;
               }
               twist->work.twist_torque_mag =
                  -twist->work.twist_sign * twist->work.phi_check * twist->work.Rf * (twist->input.riser_load * sin(twist->work.flyout_angle));
            }
            else if ((twist->work.fwd_twist == true) && (fabs(twist->work.twist_angle) >= twist->work.phi_star_fwd)) {

               twist->work.twist_torque_mag =
                 -twist->param.k * twist->work.Rw * twist->work.twist_sign * (twist->input.riser_load * sin(twist->work.flyout_angle));
            }
            else if ((twist->work.fwd_twist == false) && (fabs(twist->work.twist_angle) >= twist->work.phi_star_rev)) {
               twist->work.twist_torque_mag =
                  -twist->work.Rw * twist->work.phi_phi0_sign * (twist->input.riser_load * sin(twist->work.flyout_angle));
            }
        }
        else
        {
            twist->work.flyout_angle     = 0.0;
            twist->work.twist_torque_mag = 0.0;
        }

        /* Compute Twist Torque in Inertial Frame */
        V_SCALE(twist->work.twist_torque_inertial, twist->input.inertial_vector, twist->work.twist_torque_mag);

        /* Rotate Twist Torque in Inertial Frame to CM Body Frame */
        MxV(twist->work.twist_torque_body, twist->input.T_ECI_to_Body, twist->work.twist_torque_inertial);

        /* Rotate Twist Torque in CM Body Frame to CM SR Frame */
        MxV(twist->output.twist_torque_SR, twist->input.T_Body_to_SR, twist->work.twist_torque_body);

   }

}
