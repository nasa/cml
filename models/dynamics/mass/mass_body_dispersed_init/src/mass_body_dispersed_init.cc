/*******************************************************************************
PURPOSE:
  (Define the methods for the MassBodyDispersedInit class.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2015) (Antares) (Initial version))
   ((Brent Caughron) (OSR) (June 2017) (Antares) (Cleanup after code review))
   ((Gary Turner) (OSR) (April 2018) (Antares) (Refactor))
   ((Brent Caughron) (OSR) (Mar 2021) (Antares) (Secondary IV&V)))
*******************************************************************************/

// Local headers:
#include "../include/mass_body_dispersed_init.hh"

// JEOD headers:
#include "jeod/models/utils/math/include/matrix3x3.hh"

/*******************************************************************************
Constructor
*******************************************************************************/
MassBodyDispersedInit::MassBodyDispersedInit()
  :
  disperse()
{}

/*******************************************************************************
apply
PURPOSE:(Applies the body-action after first adding the dispersions to the
         nominal values. Nominal values are inherited.)
*******************************************************************************/
void
MassBodyDispersedInit::apply(jeod::DynManager & dyn_mgr_passthrough)
{
  // Create random numbers from *_disp_mag variables to be used in the
  // associated dispersed properties
  if (random_num_distribution) {
    disperse.disp_mag_random_number_generator();
  }

  // By default, dispersed values are absolute x = x_nom +- disp
  // Optionally, dispersed values could be relative: x = x_nom * (1.0 +- disp)
  // In this case, make them absolute.
  if (mass_disp_is_relative) {
    disperse.scale_mass( properties.mass);
  }
  if (pos_disp_is_relative) {
    disperse.scale_position( properties.position);
  }

  if (inertia_moi_disp_is_relative) {
    disperse.scale_moi( properties.inertia[0][0],
                        properties.inertia[1][1],
                        properties.inertia[2][2]);
  }
  if (inertia_poi_disp_is_relative) {
    disperse.scale_poi( properties.inertia[0][1],
                        properties.inertia[0][2],
                        properties.inertia[1][2]);
  }

  // Construct the dispersed inertia tensor from the moment and products of
  // inertia dispersions.
  disperse.generate_inertia_tensor( poi_are_negative_integrals);

  // Add the dispersed values:
  properties.mass += disperse.mass;
  jeod::Vector3::incr( disperse.position,
                 properties.position);
  jeod::Matrix3x3::incr( disperse.inertia,
                   properties.inertia);

  // Now call the base MassBodyInit method to apply the new properties.
  // variables to the Mass Body.
  MassBodyInit::apply( dyn_mgr_passthrough);
}
