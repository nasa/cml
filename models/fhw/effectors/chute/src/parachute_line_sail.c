/******************************* TRICK HEADER **********************************
PURPOSE:
    (To calculate line sail drag duing deployment of a parachute)
REFERENCE:
    (( -- ))                                   
ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))
LIBRARY DEPENDENCY:
    (parachute_line_sail.o)
    )

PROGRAMMERS:
    (((Jeff Semrau) (Honeywell) (Jan 2011) (Osiris Release 11.02, Initial Version))             
     ((Jeff Semrau) (Honeywell) (Oct 2012) (LMBP2122, Completely New Line Sail Model, 
                                            based on Pete Cuthbert's model))
     ((Jeff Semrau) (Honeywell) (Nov 2013) (LMBP3148, Tweaked line sail model, changed 
                                            direction force is applied, added theta check 
                                            and increasing bag area during deploy))
     ((Bingquan Wang) (OSR) (Apr 2017) (Fixed the compilation warning of float-point
                                        number comparison))
    )
*******************************************************************************/

/*
 * Include files for package
 */

#include "../include/parachute_line_sail.h"

#include "trick/trick_math.h"

#define _USE_MATH_DEFINES
#include <math.h>

/* Function prototypes */
void parachute_line_sail (LINE_SAIL_DATA   *line_sail)          
                        
{
    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in parachute_line_sail.c \n";
    if (!line_sail) {
       printf(error_msg);
       return;
    }

// Pete's Line Sail Model
   V_SCALE(line_sail->work.VairB, line_sail->input.VairB, -1.0);
   line_sail->work.VairB_mag = V_MAG(line_sail->work.VairB);

   //Compute relative inertial position of chute to payload
   V_SUB(line_sail->work.chute_rel_Ipos, line_sail->input.chute_inertial_pos, line_sail->input.pyld_inertial_pos)

   //Rotate relative inertial position of chute to payload from inertial to body frame
   MxV(line_sail->work.chute_rel_Bpos, line_sail->input.pyld_T_inertial_to_body, line_sail->work.chute_rel_Ipos);

   //Compute cross product of body rel pos of chute and VairB
   V_CROSS(line_sail->work.cp1, line_sail->work.chute_rel_Bpos, line_sail->work.VairB);
   line_sail->work.cp1_mag = V_MAG(line_sail->work.cp1);

   //Compute cross product to get vector from VairB to chute rel body pos
   V_CROSS(line_sail->work.cp2, line_sail->work.chute_rel_Bpos, line_sail->work.cp1);

   //Get vector pointing from chute bag to vel and perpendicular to vel
   V_SCALE(line_sail->work.force_direction_pbody, line_sail->work.cp2, -1.0);

   //Get angle between velocity vector and position vector
   line_sail->work.chute_rel_Bpos_mag = V_MAG(line_sail->work.chute_rel_Bpos);

   if ((line_sail->work.chute_rel_Bpos_mag > 0.0) && fabs(line_sail->work.VairB_mag)>0.0 ) {

      line_sail->work.arg1 =
         line_sail->work.cp1_mag / (line_sail->work.chute_rel_Bpos_mag * line_sail->work.VairB_mag);

      if (fabs(line_sail->work.arg1) <= 1.0) {

         line_sail->work.theta = asin(fabs(line_sail->work.arg1));
         //Rotate line drag force vector to inertial frame
         MtxV(line_sail->work.force_direction_inertial,
              line_sail->input.pyld_T_inertial_to_body,
              line_sail->work.force_direction_pbody);

         //Rotate line drag force vector from inertial to chute body frame
         MtxV(line_sail->work.force_direction_cbody,
              line_sail->input.chute_T_struct_to_inertial,
              line_sail->work.force_direction_inertial);
         line_sail->work.force_direction_cbody_mag = V_MAG(line_sail->work.force_direction_cbody);

         //Apply a drag force
         if (line_sail->work.force_direction_cbody_mag > 0.0) {

            line_sail->param.bag_area = line_sail->param.bag_area + line_sail->param.delta_S; 
            if (line_sail->param.bag_area > line_sail->param.max_bag_area) { 
               line_sail->param.bag_area = line_sail->param.max_bag_area;
            }

            V_NORM(line_sail->work.force_direction_cbody_uvec, line_sail->work.force_direction_cbody);

            line_sail->work.drag =
               line_sail->param.Cd * line_sail->input.chute_qbar *
               line_sail->param.bag_area * line_sail->work.theta * (2.0 / M_PI);

            V_SCALE(line_sail->output.drag_SR, line_sail->work.force_direction_cbody_uvec, line_sail->work.drag);

         }
      }
   }
}

