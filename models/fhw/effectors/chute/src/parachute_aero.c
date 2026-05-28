/******************************* TRICK HEADER **********************************
PURPOSE:
    ((This function provides for the aerodynamic calculations of the parachute model.))

LIBRARY DEPENDENCY:
     (parachute_aero.o)

 PROGRAMMERS:
    (((Richard Burt) (LM) (Feb 5, 2009) (Initial Version))
     ((Jeff Semrau)  (HON) (May 18 2010) (Osiris 10.06 Release, removed Qgrad code, no longer in model memo))
     ((Jeff Semrau)  (HON) (Aug 2010) (Osiris 10.08 Release, DR 854, corrected computation of ballistic drag))
     ((Jeff Semrau)  (HON) (Jan 2011) (Osiris 11.02 Release, CR 859, updated simple wake effects model))
     ((Jeff Semrau)  (HON) (02/2012) (lmbp1395) (Chute Pressure Recovery Fraction))
     ((Jeff Semrau)  (HON) (05/2012) (lmbp1650) (Code Clean Up, removed call to wake_effects))
     ((Jeff Semrau)  (HON) (01/2013) (lmbp2290) (MM11 Updates))
     ((Bingquan Wang) (OSR) (5/17) (ANTARES) (Removed the library dependency
                  on table_interp due to its refactoring as inline functions))
    )
*******************************************************************************/

#include "trick/vector_macros.h"
#include "trick/matrix_macros.h"
#include "cml/models/utilities/table_interp/include/osiris_math.h"
#include "../include/parachute_aero.h"

int parachute_aero(AERO_DATA     *A)    /* INOUT:  --    Chute Aerodynamic Data Structure */

{

   const char error_msg[] = "\nERROR: Calling argument pointer is NULL in parachute_aero.c \n";
   if (!A) {
      printf(error_msg);
      return(0);
   }

   /* Zero out the aerodynamic force and torque. */
   V_INIT(A->output.force_SR);
   V_INIT(A->output.moment_SR );

   /*
    Simple Wake Effects Model
    16-Jan-11: Wake is poorly understood, the above model is not activated.
    To bound the effects wake will have on a chute, particularly small
    chutes like the FBC parachutes or pilots the following simple gain
    on chute qbar has been implemented. The model is turned on with the
    use of a flag. As wake is better understood the wake model will be
    updated.
   */

   if (A->param.Simple_Wake_Effects_Enabled == true) {

      // Compute Separation Distance
      V_SUB(A->work.sep_distance, A->input.att_pnt_inertial_pos, A->input.chute_inertial_pos);
      A->work.sep_distance_mag = V_MAG(A->work.sep_distance);

      if (A->work.sep_distance_mag < 0.0) {
          A->work.qbar_sf = 0.0;
      }
      else {
         //Max Value of Wake Knockdown
         double max_sf = 0.95;
         A->work.qbar_sf = max_sf - max_sf * exp(-A->work.sep_distance_mag/A->param.decay_factor);
      }

      A->input.dynamic_pressure = A->input.dynamic_pressure * A->work.qbar_sf;
   }

   /* Now Compute Aerodynamic Forces/Moments */
   if (A->input.aero_mode == ballistic_aero) {


      /* Calculate Velocity Unit Vector in the Structural Frame */
      A->work.vmag2 = V_DOT( A->input.VairSR , A->input.VairSR );
      A->work.vmag  = sqrt( A->work.vmag2 );

      double tol = 1e-9;
      if (A->work.vmag < tol) {
         A->work.vmag = tol;
      }
      V_SCALE( A->work.v_dir_str, A->input.VairSR, (-1.0/A->work.vmag) );

      /* Compute the aerodynamic drag force on the vehicle using BC. */
      A->output.drag = (A->input.dynamic_pressure * A->input.mass) / A->param.BC;

      /* Compute the force vector from the force magnitude and velocity vector. */
      V_SCALE( A->output.force_SR, A->work.v_dir_str, A->output.drag );
   }
   else if (A->input.aero_mode == aero_coef) {

      /* Axial Coefficient */
      A->work.aero_ca = table_lookup_2d(A->input.mach,
                               A->param.jdex_ca,
                               A->param.mach_ca_table,
                               A->input.alpha_total,
                               A->param.idex_ca,
                               A->param.alpha_ca_table,
                               A->param.ca_table);

      /* Normal Coefficient */
      A->work.aero_cy = table_lookup_1d(A->input.alpha_total,
                               A->param.idex_cy,
                               A->param.alpha_cy_table,
                               A->param.cy_table);

      /* Pitching Coefficient */
      A->work.aero_cm = table_lookup_1d(A->input.alpha_total,
                               A->param.idex_cm,
                               A->param.alpha_cm_table,
                               A->param.cm_table);

      /* Pitch Dampening */
      A->work.aero_cq = table_lookup_1d(A->input.mach,
                               A->param.idex_cq,
                               A->param.mach_cq_table,
                               A->param.cq_table);

      /* Calculate the roll angle */
      double Vair_aero[3];
      MtxV(A->work.VairB, A->input.T_body_to_SR, A->input.VairSR) ;
      MtxV(Vair_aero, A->input.T_aero_to_body, A->work.VairB) ;
      A->work.roll_angle = atan2(Vair_aero[1], -Vair_aero[2]);

      A->work.vmag2 = V_DOT( A->input.VairSR , A->input.VairSR );
      A->work.vmag  = sqrt( A->work.vmag2 );

      double cos_roll, sin_roll;
      cos_roll = cos(A->work.roll_angle);
      sin_roll = sin(A->work.roll_angle);

      /* Calculate resulting forces and moments on the parachute body, note this is
      not the standard GN&C body frame definition */
      A->work.QDS                  =  A->param.PRF * A->input.dynamic_pressure * A->input.Chute_Area;
      A->work.QDSC                 =  A->work.QDS * A->input.CBAR;
      A->work.damping              =  A->work.aero_cq * A->input.CBAR / (2.0 * A->work.vmag);
      A->work.force_chute_aero[0]  = -A->work.aero_ca * A->work.QDS;
      A->work.force_chute_aero[1]  = -A->work.aero_cy * sin_roll * A->work.QDS ;
      A->work.force_chute_aero[2]  =  A->work.aero_cy * cos_roll * A->work.QDS ;
      A->work.moment_chute_aero[0] =  0;
      A->work.moment_chute_aero[1] =  (A->work.aero_cm * cos_roll + A->work.damping * A->input.body_rates[1]) * A->work.QDSC ;
      A->work.moment_chute_aero[2] =  (-A->work.aero_cm * sin_roll + A->work.damping * A->input.body_rates[2]) * A->work.QDSC ;

      // Hold X component of chute force vector to be no less than min drag value,
      // Used in conjunction with deploy load model.
      if ( fabs(A->work.force_chute_aero[0]) < A->input.min_drag) {
         A->work.force_chute_aero[0] = -1.0 * A->input.min_drag;
      }
      /* Negate the yawing moment due to the negative convention */
      A->work.moment_chute_aero[2] =  -1*A->work.moment_chute_aero[2];

      /* Compute total drag force generated by chute */
      A->output.drag = V_MAG(A->work.force_chute_aero);

      /* Rotate Aero Forces/Torques from Chute Aero Frame to Chute Body Frame */
      MxV(A->work.force_chute_body, A->input.T_aero_to_body, A->work.force_chute_aero) ;
      MxV(A->work.moment_chute_body, A->input.T_aero_to_body, A->work.moment_chute_aero) ;


      /* Rotate Aero Forces/Torques from Chute Body Frame to Chute SR Frame */
      MxV(A->output.force_SR, A->input.T_body_to_SR, A->work.force_chute_body) ;
      MxV(A->output.moment_SR, A->input.T_body_to_SR, A->work.moment_chute_body) ;

   }

   return(0);

}
