/*
PURPOSE:
    (State integration using a 1st order explicit Euler scheme.)

REFERENCE:
    ((None))

ASSUMPTIONS AND LIMITATIONS:
    (( ))

CLASS:
    (N/A)

LIBRARY DEPENDENCY:
	(parachute_eom.o)

PROGRAMMERS:
    ((Jeff Semrau) (Honeywell) (Mar 2009) (High Fidelity Chute Model))
    ((Jeff Semrau) (Honeywell) (May 2010) (Osiris CR 763))                
    ((Jeff Semrau) (Honeywell) (Nov 2015) (CR4609, updates for new integration method))
*/

#include "../include/parachute_eom.h"
#include "trick/trick_math.h"

void parachute_eom ( EOM_DATA        *EOM)
{                               

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in parachute_eom.c \n";
    if (!EOM) {
       printf(error_msg);
       return;
    }


   int i; 

   // Compute Body Translational Derivatives (aka Accels)
   if(EOM->input.modeling_chute == true) {
      for (i = 0; i < 3; i++) {
        EOM->work.trans_accel[i] = 
           (EOM->input.pilot_force_SR[i] + EOM->input.mortar_force_SR[i] + EOM->input.riser_force_SR[i] + EOM->input.aero_force_SR[i]) /
           EOM->input.chute_mass[i];
      }
   }
   else {
      for (i = 0; i < 3; i++) {
         EOM->work.trans_accel[i] = 
           (EOM->input.pilot_force_SR[i] + EOM->input.mortar_force_SR[i] + EOM->input.riser_force_SR[i] + EOM->input.aero_force_SR[i]) / 
            EOM->input.mass;
      }
   }
   
   //Summarize Translational Derivatives (aka Accels) with Envirnoment Derivs
   V_ADD(EOM->work.total_accel, EOM->work.trans_accel, EOM->input.apparent_mass_accel);

   //Transform Translational Derivatives (aka Accels) from SR to Body Frame
   MxV(EOM->work.body_accel, EOM->input.T_SR_to_body, EOM->work.total_accel);
   
   //Transform Translational Derivatives (aka Accels) from Body to Inertial Frame
   MxV(EOM->work.inertial_accel, EOM->output.T_body_to_inertial, EOM->work.body_accel);

   //CODE TO COMUTE GRAV ACCEL 
   double temp_accel[3];
   if (EOM->input.modeling_chute == true) {  
      V_SCALE(EOM->work.gravaccel_mass, EOM->input.gravaccel,  EOM->input.chute_dry_mass);

      MxV(EOM->work.gravaccelmass_SR, EOM->output.T_inertial_to_body, EOM->work.gravaccel_mass);
      EOM->work.gravSR[0] = EOM->work.gravaccelmass_SR[0] / EOM->input.chute_mass[0];
      EOM->work.gravSR[1] = EOM->work.gravaccelmass_SR[1] / EOM->input.chute_mass[1];
      EOM->work.gravSR[2] = EOM->work.gravaccelmass_SR[2] / EOM->input.chute_mass[2];

      MxV(EOM->work.gravaccel, EOM->output.T_struct_to_inertial, EOM->work.gravSR);

   //Add Gravity accel (already in inertial frame) to non-conservative accels
      /* Gravity accel is applied to chute dry mass to compute force, but resultant accel of the chute mass 
         is computed using total mass of chute, including enclosed and apparent mass */  
      V_ADD(temp_accel, EOM->work.inertial_accel, EOM->work.gravaccel);
      V_COPY(EOM->work.inertial_accel, temp_accel);
   }
   else {
      /* Modeling Confluence Point */ 
      V_ADD(temp_accel, EOM->work.inertial_accel, EOM->input.gravaccel);
      V_COPY(EOM->work.inertial_accel, temp_accel);
   }   

   /////// End Computation of Translational Accels ////////

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////

   //Compute Rotational Accels

   //First determine if rotational EOM is to be done for this body, 
   //if not, exit routine
   if (EOM->input.rotational_eom_enabled == false) {
      return;
   }

   //Sum up moments
   for (i = 0; i < 3; i++) {
      EOM->work.SR_torque[i] = EOM->input.aero_moment_SR[i] + EOM->input.riser_moment_SR[i] + EOM->input.pilot_moment_SR[i]; 
   }

   //Rotate aero torques from SR to Body Frame
   MxV(EOM->work.body_torque, EOM->input.T_SR_to_body, EOM->work.SR_torque);

   //Compute 2nd order rotational dynamics, in body frame
   //Following JEOD implementation of computations here for commonality
   //Compute Angular Momentum in Body Axes
   MxV(EOM->work.ang_mom_body, EOM->input.inertia, EOM->output.body_rates);
   
   //Compute Cross Product Term of Rotational EOM 
   V_CROSS(EOM->work.inertial_torque, EOM->output.body_rates, EOM->work.ang_mom_body);

   //Compute resultant torques
   V_DECR(EOM->work.body_torque, EOM->work.inertial_torque);

   //Compute Rotational Accels to feed into integrator
   //Invert Inertia Matrix to solve for angular accel, alpha = Torque/Inverse_Inertia_Matrix
   dm_invert_symm(EOM->work.inverse_inertia, EOM->input.inertia);

   //Solve for rotational accels
   MxV(EOM->work.rot_accel, EOM->work.inverse_inertia, EOM->work.body_torque);

   double tol = 1e-15;
   //Truncate rotational accels to eliminate feedback bit error
   if (fabs(EOM->work.rot_accel[0]) < tol) EOM->work.rot_accel[0] = 0.0;
   if (fabs(EOM->work.rot_accel[1]) < tol) EOM->work.rot_accel[1] = 0.0;
   if (fabs(EOM->work.rot_accel[2]) < tol) EOM->work.rot_accel[2] = 0.0;

   //Compute transformation matrices 
   M_TRANS(EOM->output.T_body_to_inertial, EOM->output.T_inertial_to_body);
   MxM(EOM->output.T_struct_to_inertial, EOM->output.T_body_to_inertial, EOM->input.T_SR_to_body);

   return;
}
