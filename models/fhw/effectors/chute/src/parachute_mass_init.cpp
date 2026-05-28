/******************************* TRICK HEADER **********************************

PURPOSE:
    (Initialization of the high fidelity parachute mass and positions) 
ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))

PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (Jan 2016) (Initial Release))           
*******************************************************************************/

/*
 * Include files for package
 */

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "trick/trick_math.h"
#include "../include/parachute_exec.hh"

void parachute_mass_init (
                          const bool                   plt_chute, 
                          const MassBodyDispersedInit *chute_mass_props, 
                          MASS_DATA                   *chute_mass, 
                          PARACHUTE_DATA&              chute) 
{

   if (!chute_mass_props || !chute_mass) {
      CMLMessage::fail(__FILE__,__LINE__,"Information only: \n","pointer is null \n");
      return;
   }

   for(int j=0; j<chute.param.num_chutes; j++) { 

      if (plt_chute == false) { 
         V_COPY(chute.param.chute_pos_SR[j], chute_mass_props[j].properties.position);
         chute_mass[j].param.parachute_dry_mass = chute_mass_props[j].properties.mass * 
            (chute.param.riser_mass_ratio + chute.param.suspension_line_mass_ratio + chute.param.canopy_mass_ratio);
         chute_mass[j].param.mass_below_skirt = 
            chute_mass_props[j].properties.mass * (chute.param.riser_mass_ratio + chute.param.suspension_line_mass_ratio);
         chute_mass[j].param.canopy_mass = chute_mass_props[j].properties.mass * chute.param.canopy_mass_ratio;
      }
      else { 
         V_COPY(chute.param.pilot_chute_pos_SR[j], chute_mass_props[j].properties.position);
         chute_mass[j].param.parachute_dry_mass = chute_mass_props[j].properties.mass * 
            (chute.param.plt_riser_mass_ratio + chute.param.plt_suspension_line_mass_ratio + chute.param.plt_canopy_mass_ratio);
         chute_mass[j].param.mass_below_skirt = 
            chute_mass_props[j].properties.mass * (chute.param.plt_riser_mass_ratio + chute.param.plt_suspension_line_mass_ratio);
         chute_mass[j].param.canopy_mass = chute_mass_props[j].properties.mass * chute.param.plt_canopy_mass_ratio;
      }
   }
}
