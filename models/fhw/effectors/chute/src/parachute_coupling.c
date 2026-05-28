/******************************* TRICK HEADER **********************************
PURPOSE:
    ((This function provides for the coupling forces between two bodies
      connected by a riser.))

REFERENCE:
    ((  None ))

ASSUMPTIONS AND LIMITATIONS:
    (( None ))

LIBRARY DEPENDENCY:
     ((parachute_coupling.o))

 PROGRAMMERS:
    (((Richard Burt) (LM) (Feb 5, 2009) (Initial Version)))
    (((Jeff Semrau)  (HW) (Nov 2013)    (New code to compute inelastic load, LMBP#3148)))
*******************************************************************************/

#include "../include/parachute_coupling.h"
#include "trick/vector_macros.h"
#include "trick/matrix_macros.h"
#include "cml/models/utilities/table_interp/include/osiris_math.h"

#define _USE_MATH_DEFINES
#include <math.h>

int parachute_coupling( CHUTE_COUPLING_DATA    *riser /* Data Structure of Coupling Model */ )

{

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in parachute_coupling.c \n";
    if (!riser) {
       printf(error_msg);
       return(0);
    }

   // Code to gain chute moment arm as a function of current chute diameter
   double diam_check = 1e-8;
   if (riser->param.gain_moment == true) {
      if (riser->input.nominal_diameter > diam_check) {
         riser->work.moment_arm_gain = riser->input.current_diameter/riser->input.nominal_diameter;
      }
      V_SCALE(riser->input.endpoint2_vec_wrt_cg, riser->input.endpoint2_vec_wrt_cg, riser->work.moment_arm_gain);
   }

   /* Compute the deflection of the riser  */
   riser->output.deflection = riser->input.relstate_riser_distance - riser->input.nominal_length;

   // If we are to use the inelastic load, set it
   if (riser->input.use_inelastic_load == true) {
       riser->output.load = riser->param.inelastic_load;

      if (riser->input.use_mass_rate_load == true) {

         if (riser->input.deploy_distance <= (riser->input.riser_distance + riser->input.suspension_distance)) {

            riser->work.current_cumulative_mass = table_lookup_1d(riser->input.deploy_distance,
                                                                  riser->param.idex_mass,
                                                                  riser->param.dist_mass_table,
                                                                  riser->param.mass_table);

            riser->work.canopy_starting_mass = riser->work.current_cumulative_mass;

         }
         else {

            riser->work.canopy_deploy_dist = riser->input.deploy_distance - (riser->input.riser_distance + riser->input.suspension_distance);

            if (riser->input.nominal_diameter > diam_check) {
               riser->work.canopy_deployed_mass =
                  riser->input.canopy_mass * (2.0 * sin(((2.0*riser->work.canopy_deploy_dist)/riser->input.nominal_diameter) + (M_PI/6.0)) - 1.0);
            }

            riser->work.current_cumulative_mass = riser->work.canopy_starting_mass + riser->work.canopy_deployed_mass;

         }

         riser->work.deployed_mass = riser->work.current_cumulative_mass;
         riser->work.delta_mass = riser->work.current_cumulative_mass - riser->work.previous_cumulative_mass;
         riser->work.delta_dist = riser->input.deploy_distance - riser->work.previous_deploy_distance;

         double tol = 1e-8;
         if (fabs(riser->work.delta_dist) < tol) {
            riser->work.delta_dist = tol;
         }

         riser->work.mass_per_unit_length = riser->work.delta_mass/riser->work.delta_dist;
         riser->work.mass_flow_load = riser->work.mass_per_unit_length * pow(riser->input.velocity, 2);

         if (riser->input.deploy_distance <= riser->input.riser_distance) {
            riser->work.drag_area = riser->param.rs_cds_per_unit_length * riser->input.deploy_distance;
         }
         else if (/*(riser->input.deploy_distance > riser->input.riser_distance) &&*/
                  (riser->input.deploy_distance <= (riser->input.riser_distance + riser->input.suspension_distance))) {
            riser->work.drag_area = riser->param.rs_cds_per_unit_length * riser->input.deploy_distance;
         }
         else {
            riser->work.drag_area =
               riser->param.rs_cds_per_unit_length * (riser->input.riser_distance + riser->input.suspension_distance) +
               riser->param.canopy_cds_per_unit_length * (riser->input.deploy_distance - (riser->input.riser_distance + riser->input.suspension_distance));
            riser->work.canopy_drag_area =
               riser->param.canopy_cds_per_unit_length * (riser->input.deploy_distance - (riser->input.riser_distance + riser->input.suspension_distance));
         }
         riser->work.drag = riser->work.drag_area * riser->input.qbar;
         riser->output.load = riser->work.mass_flow_load + riser->work.drag;
         riser->output.deploy_load = riser->output.load;

         //Align deploy load to payload velocity vector
         MtxV(riser->work.VairInertial, riser->input.pyld_T_inertial_to_body, riser->input.VairB);
         V_NORM(riser->input.relstate_force_uvec, riser->work.VairInertial);
      }
      else {
         //Inelastic load only applies to endpoint1 and does not apply a moment
         V_INIT(riser->input.endpoint1_vec_wrt_cg);
      }
   }
   else {
      riser->output.deploy_load = 0.0;

      /* Now apply spring mass dampener equation to determine load across the
         riser. If riser is not at full length then set load to zero. */
      if (riser->output.deflection < 0) {
          riser->output.load = 0;
          riser->output.deflection = 0.0;
      }
      else {
          riser->output.load = riser->param.spring_constant * riser->output.deflection +
                               riser->param.damping_constant * riser->input.relstate_riser_velocity;
      }
   }

   // Set min force to 0, line can not apply compressive loads
   if (riser->output.load < 0.0) {
       riser->output.load = 0.0;
   }

   /* Save maximum load value of riser */
   if (riser->output.load > riser->output.max_riser_load ) {
       riser->output.max_riser_load = riser->output.load;
   }

   /* Calculate the resultant force on the end points */
   //Negating load in computation of EP1 to get EP1 force in correct SR direction
   V_SCALE( riser->work.endpoint1_force_eci, riser->input.relstate_force_uvec, -riser->output.load );
   V_SCALE( riser->work.endpoint2_force_eci, riser->input.relstate_force_uvec, riser->output.load );
   if (riser->input.use_inelastic_load == true) {
       V_INIT(riser->work.endpoint2_force_eci);
   }

   /* Translate the ECI force to the respective body frames */
   MxV(riser->output.endpoint1_force_body, riser->input.endpoint1_T_ECI_to_Body, riser->work.endpoint1_force_eci) ;
   MxV(riser->output.endpoint2_force_body, riser->input.endpoint2_T_ECI_to_Body, riser->work.endpoint2_force_eci) ;

   /* Rotate the force to the SR frame */
   MxV(riser->output.endpoint1_force_SR, riser->input.endpoint1_T_Body_to_SR, riser->output.endpoint1_force_body) ;
   MxV(riser->output.endpoint2_force_SR, riser->input.endpoint2_T_Body_to_SR, riser->output.endpoint2_force_body) ;

   /* Save maximum by-axis load values */
   if (riser->output.endpoint1_force_SR[0] > riser->output.endpoint1_max_force_SR[0] ) {
       riser->output.endpoint1_max_force_SR[0] = riser->output.endpoint1_force_SR[0];
   }
   if (riser->output.endpoint1_force_SR[1] > riser->output.endpoint1_max_force_SR[1] ) {
       riser->output.endpoint1_max_force_SR[1] = riser->output.endpoint1_force_SR[1];
   }
   if (riser->output.endpoint1_force_SR[2] > riser->output.endpoint1_max_force_SR[2] ) {
       riser->output.endpoint1_max_force_SR[2] = riser->output.endpoint1_force_SR[2];
   }
    if (riser->output.endpoint2_force_SR[0] > riser->output.endpoint2_max_force_SR[0] ) {
       riser->output.endpoint2_max_force_SR[0] = riser->output.endpoint2_force_SR[0];
   }
   if (riser->output.endpoint2_force_SR[1] > riser->output.endpoint2_max_force_SR[1] ) {
       riser->output.endpoint2_max_force_SR[1] = riser->output.endpoint2_force_SR[1];
   }
   if (riser->output.endpoint2_force_SR[2] > riser->output.endpoint2_max_force_SR[2] ) {
       riser->output.endpoint2_max_force_SR[2] = riser->output.endpoint2_force_SR[2];
   }

   /* Calculate the resulting moment on the bodies */
   V_CROSS(riser->output.endpoint1_moment_SR, riser->input.endpoint1_vec_wrt_cg, riser->output.endpoint1_force_SR);
   V_CROSS(riser->output.endpoint2_moment_SR, riser->input.endpoint2_vec_wrt_cg, riser->output.endpoint2_force_SR);

return(0);
}
