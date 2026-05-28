/******************************* TRICK HEADER **********************************
PURPOSE:
    ((This function calculates the apparent mass and enclosed mass of a 
       parachute.))
 
REFERENCE:
    (( This is the DSS chute mass model, rehosted into Osiris for use in the Osiris 
       High Fidelity Chute Model ))
 
ASSUMPTIONS AND LIMITATIONS:
    (( None ))
 
LIBRARY DEPENDENCY:
     ((parachute_mass_properties_mark2.o))

 PROGRAMMERS:
    (((Jeff Semrau)  (HON) (Aug 2010) (11.02 Release, New Mass Props model with apparent mass updates, 
                                       Emphirically dervied and documented in LRS Chute Modeling Memo)))
    (((Jeff Semrau)  (HON) (May 2012) (LMBP 1650 - LRS MMv9 Updates)))
    (((Jeff Semrau)  (HON) (Aug 2013) (LMBP 2720 - Added filter of atm density to smooth out mass spikes)))
    (((Jeff Semrau)  (HON) (Nov 2013) (LMBP 3148 - New I/F to include sim_time, needed to compute correct mass rate)))
*******************************************************************************/

#include "trick/vector_macros.h"
#include "trick/matrix_macros.h"
#include "../include/parachute_mass_props.h"

#define _USE_MATH_DEFINES
#include <math.h>

int parachute_mass_properties_mark2(double sim_time_local, 
                                    INFLATION_DATA   *inflation, 
                                    MASS_DATA        *mass)

{ 

    const char error_msg[] = "\nERROR: Calling argument pointer is NULL in parachute_mass_properties_mark2.c \n";
    if (!inflation || !mass) {
       printf(error_msg);
       return(0);
    }

   if (inflation->work.fill_time <= 0.0) { 
      mass->work.dnorm_sf = 1.0;
   }
   else if (inflation->work.fill_time < 3.0) { 
      mass->work.dnorm_sf = -0.17 * inflation->work.fill_time / 3.0 + 1.0;
   }
   else { 
      mass->work.dnorm_sf = 0.83;
   }

   if (inflation->work.reef_stage == xreef_1) {
      mass->work.dnorm0 = 0.0;
      mass->work.dnorm1 = sqrt(inflation->work.area_ratio[inflation->work.reef_stage]);
   }
   else {
      if (inflation->work.reef_stage > 0) { //Index check to prevent pointing out range of array with -1 operation in dnorm0
         mass->work.dnorm0 = sqrt(inflation->work.area_ratio[inflation->work.reef_stage-1]);
         mass->work.dnorm1 = sqrt(inflation->work.area_ratio[inflation->work.reef_stage]);
      }
   }

   if (inflation->work.stage_elasped_time < inflation->work.fill_time) { 
      mass->work.normalized_diameter = mass->work.dnorm0 + (mass->work.dnorm1 - mass->work.dnorm0) * 
                                       pow((inflation->work.stage_elasped_time/inflation->work.fill_time), mass->work.dnorm_sf);
   }
   else {
      mass->work.normalized_diameter = mass->work.dnorm1;
   }

   mass->work.current_diameter = mass->input.nominal_diameter * mass->work.normalized_diameter;

   mass->work.diameter_sqrd       = pow(mass->work.current_diameter, 2);

   // Compute Added Mass Gain
   mass->work.rr_prime = pow(mass->work.normalized_diameter, 2);


   double slope_percentage;
   slope_percentage = (mass->work.rr_prime - mass->param.XK2)/(mass->param.XK2 - mass->param.XK1); 

   mass->work.X  = (tanh(-mass->param.TNHFAC * slope_percentage) - 1.0) * (mass->param.XM1 - mass->param.XM2)/2.0 + mass->param.XM1;
                   
   mass->work.B  = (tanh(-mass->param.TNHFAC * slope_percentage) - 1.0) * (mass->param.B1 - mass->param.B2)/2.0 + mass->param.B1;

   mass->work.DCLF  = mass->work.X * mass->work.rr_prime + mass->work.B;

   // Compute Enclosed Mass
   // Enclosed Mass is described by two volumes, the volume of a hemisphere and 
   // the volume of a cylinder. The cylinder volume is used in the reefing stages 
   // and decreases as the reefing stages increases. At full inflation the cylinder volume 
   // will go to 0. 
   /* Compute Hemisphere Chute Volume, using DSS algorithms to compute enclosed volume of chute */
   mass->work.hemi_volume = M_PI * pow((mass->work.current_diameter), 3) / 12.0; 

   //Filter input air density, this smoothes out jumps in density that result from GRAM calling rate slowing down in Sim as
   //CM slows down under the chutes. The jump in density causes a spike in mass rate and accels of the chute, filtering density 
   //smoothes these spikes out. 
   if (mass->work.prev_density > 0.0 && mass->input.first_pass == false) { 
      mass->work.rc = 0.1; 
      mass->work.alp = mass->input.dt / (mass->work.rc + mass->input.dt);
      mass->work.new_density = mass->work.prev_density + mass->work.alp * (mass->input.air_density - mass->work.prev_density);
      mass->work.prev_density = mass->work.new_density;
   }
   else { 
      mass->work.new_density = mass->input.air_density;
      mass->work.prev_density = mass->input.air_density;
   }

   /* Compute Enclosed Air Mass in Chute Volume Described by a Hemisphere */
   mass->work.enclosed_hemisphere_mass =  mass->param.enclosed_mass_sf * mass->param.volume_sf * mass->work.new_density * mass->work.hemi_volume;

   /* Compute Volume Described by a Cylinder */
   mass->work.cyl_volume = 
      (mass->work.diameter_sqrd * mass->input.nominal_diameter * (1 - mass->work.normalized_diameter))/ (2.0 * M_PI);

   if (mass->work.cyl_volume < 0.0) {
      mass->work.cyl_volume = 0.0;
   }   

   /* Compute Enclosed Air Mass in Chute Volume Described by the Cylinder */
   mass->work.enclosed_cylinder_mass = mass->param.enclosed_mass_sf * mass->work.cyl_volume * mass->work.new_density;

   /* Compute Total Enclosed Air Mass */ 
   mass->work.enclosed_mass = (mass->work.enclosed_hemisphere_mass + mass->work.enclosed_cylinder_mass) * mass->work.DCLF; 

   /* Compute Apparent Mass, Apparent Mass is only applied in Axial Direction */
   mass->work.apparent_mass = ((mass->param.app_mass_sf * mass->work.new_density * 
                              (mass->param.volume_sf * M_PI * pow((mass->work.current_diameter),3)))/8.0) * mass->work.DCLF;

   /* Compute Axial, Normal & Lateral Masses */
   mass->work.parachute_mass_x = mass->param.parachute_dry_mass + mass->work.enclosed_mass + mass->work.apparent_mass; 
   mass->work.parachute_mass_y = mass->param.parachute_dry_mass + mass->work.enclosed_mass; 
   mass->work.parachute_mass_z = mass->work.parachute_mass_y;

   /* Compute MOI's of parachute */
   double volume_sf_for_inertia;
   volume_sf_for_inertia = pow(mass->param.volume_sf, 2.0/3.0);
   mass->work.canopy_inertia = 
      (mass->param.parachute_dry_mass - mass->param.mass_below_skirt) * (volume_sf_for_inertia * mass->work.diameter_sqrd) / 6.0;

   /* Limit canopy inertia to be no less than 0.1 kg*M2 */
   if (mass->work.canopy_inertia < 0.1) mass->work.canopy_inertia = 0.1;

   /* Compute chute inertia, hard coded numbers (12.0, 3.0) im calculations are to be completely traceable to documented equation in LRS Model Memo
   LM-ORN-0598. These nuumbers are not defined or explained in model memo. */
   M_INIT(mass->output.inertia);
   mass->output.inertia[0][0] = mass->work.canopy_inertia + (mass->param.mass_below_skirt * volume_sf_for_inertia * mass->work.diameter_sqrd) / 12.0;
   mass->output.inertia[1][1] = mass->work.canopy_inertia + (mass->param.mass_below_skirt * pow(mass->param.dlp, 2)) / 3.0;
   mass->output.inertia[2][2] = mass->output.inertia[1][1]; 

   if (mass->input.first_pass == true) { 
      mass->work.prev_parachute_mass_x    = mass->work.parachute_mass_x;
      mass->work.prev_parachute_mass_y    = mass->work.parachute_mass_y;
      mass->work.prev_parachute_mass_z    = mass->work.parachute_mass_z;
      mass->input.first_pass = false;
   }

   /* Compute Mass Rate from enclosed and apparent masses */
   mass->work.prelim_mass_x_rate = (mass->work.parachute_mass_x - mass->work.prev_parachute_mass_x) / mass->input.dt;
   if (mass->work.prelim_mass_x_rate < 0.0) mass->work.prelim_mass_x_rate = 0.0;

   mass->work.mass_y_rate = (mass->work.parachute_mass_y - mass->work.prev_parachute_mass_y) / mass->input.dt;
   mass->work.mass_z_rate = (mass->work.parachute_mass_z - mass->work.prev_parachute_mass_z) / mass->input.dt;

   mass->work.prev_time = inflation->work.stage_elasped_time - mass->input.dt; 
   if (mass->work.prev_time >= 0.0) {
      mass->work.corrected_time = mass->work.prev_time;
   }
   else {
      mass->work.corrected_time = 0.0;
   }

   mass->work.R1 = 1.0/(mass->param.A * (pow(mass->work.corrected_time, mass->param.D) - mass->param.C)) + mass->param.B;
   if ((inflation->work.stage_elasped_time < inflation->work.fill_time) && (mass->work.R1 <= 1.0)) { 
      mass->work.mass_x_rate = mass->work.prelim_mass_x_rate;
   }
   if ((inflation->work.stage_elasped_time < inflation->work.fill_time) && (mass->work.R1 > 1.0)) {
      mass->work.mass_x_rate = mass->work.prelim_mass_x_rate/mass->work.R1;
   }

   if ((inflation->work.stage_elasped_time < inflation->work.fill_time) &&
       (inflation->work.fill_time_computed[inflation->work.reef_stage] == true))   {
      mass->work.max_mass_x_rate = mass->work.mass_x_rate;
      // Take snapshot of current time 
      mass->work.tdone = sim_time_local;                          
   }
   else {
      // Compute time since inflation complete
      mass->work.t2 =  sim_time_local - mass->work.tdone;
     // Limit time to 10 sec, to avoid floating point exception in exp calculation below
     if (mass->work.t2 > 10.0) { 
        mass->work.t2 = 10.0;
     }
      mass->work.mass_x_rate = mass->work.max_mass_x_rate * exp(-14.2 * mass->work.t2);
   }

   mass->work.prev_normalized_diameter = mass->work.normalized_diameter; 
   mass->work.prev_parachute_mass_x    = mass->work.parachute_mass_x;
   mass->work.prev_parachute_mass_y    = mass->work.parachute_mass_y;
   mass->work.prev_parachute_mass_z    = mass->work.parachute_mass_z;

   /* Compute Accelerations due to mass rate, will be used in translational EOM */
   mass->output.mass_accel[0] = (-mass->work.mass_x_rate * mass->input.chute_velocity[0])/mass->work.parachute_mass_x;       
   mass->output.mass_accel[1] = (-mass->work.mass_y_rate * mass->input.chute_velocity[1])/mass->work.parachute_mass_y; 
   mass->output.mass_accel[2] = (-mass->work.mass_z_rate * mass->input.chute_velocity[2])/mass->work.parachute_mass_z;   

   /* Copy Parachute masses over to output vector */
   mass->output.chute_mass[0] = mass->work.parachute_mass_x;
   mass->output.chute_mass[1] = mass->work.parachute_mass_y;
   mass->output.chute_mass[2] = mass->work.parachute_mass_z;

   return (0);
    
}
