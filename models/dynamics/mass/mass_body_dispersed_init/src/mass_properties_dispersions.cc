/*******************************************************************************
PURPOSE:
  (Define the methods for the MassPropertiesDispersions and
   MassPropertiesDispersionsFlags classes.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2015) (Antares) (Initial version))
   ((Brent Caughron) (OSR) (June 2017) (Antares) (Cleanup after code review))
   ((Gary Turner) (OSR) (April 2018) (Antares) (Refactor))
   ((Brent Caughron) (OSR) (Mar 2021) (Antares) (Secondary IV&V)))
*******************************************************************************/

// Local headers:
#include "../include/mass_properties_dispersions.hh"

// Library headers:
#include <random> // For the random number generator

/*******************************************************************************
Constructor
*******************************************************************************/
MassPropertiesDispersions::MassPropertiesDispersions()
  :
  mass(0.0),
  position(),
  moi(),
  poi(),
  position_match( DispMatchNone),
  position_match_sf(1.0),
  moi_match( DispMatchNone),
  moi_match_sf(1.0),
  poi_match( DispMatchNone),
  poi_match_sf(1.0),
  mass_disp_mag(0.0),
  position_disp_mag{0.0, 0.0, 0.0},
  moi_disp_mag{0.0, 0.0, 0.0},
  poi_disp_mag{0.0, 0.0, 0.0},
  seed_rand(),
  inertia()
{}
/******************************************************************************/
MassPropertiesDispersionsFlags::MassPropertiesDispersionsFlags()
  :
  random_num_distribution(false),
  mass_disp_is_relative(false),
  pos_disp_is_relative(false),
  inertia_moi_disp_is_relative(false),
  inertia_poi_disp_is_relative(false),
  poi_are_negative_integrals(true)
{}

/*******************************************************************************
generate_inertia_tensor
PURPOSE:(Generates the inertia tensor from the moment and products of inertia.)
*******************************************************************************/
void
MassPropertiesDispersions::generate_inertia_tensor(
    bool poi_are_negative_integrals)
{
  inertia[0][0]   =  moi[0];
  inertia[1][1]   =  moi[1];
  inertia[2][2]   =  moi[2];

  if (poi_are_negative_integrals) {
    inertia[0][1] =
    inertia[1][0] =  poi[0];
    inertia[0][2] =
    inertia[2][0] =  poi[1];
    inertia[1][2] =
    inertia[2][1] =  poi[2];
  }
  else {
    inertia[0][1] =
    inertia[1][0] = -poi[0];
    inertia[0][2] =
    inertia[2][0] = -poi[1];
    inertia[1][2] =
    inertia[2][1] = -poi[2];
  }
}

/*******************************************************************************
apply_correlations
PURPOSE:(Facilitates having matching or correlated dispersions applied on
         multiple axes without having to independently disperse these values
         with similar distribution parameters.)
*******************************************************************************/
void
MassPropertiesDispersions::apply_correlations()
{
  // Create switch cases when setting either the Y or Z position to a
  // lower indexed value. Such as setting either/both Y and Z axis positions
  // to match the X position. Or matching the Z axis position to the Y axis.
  // This is done by taking the lower indexed axis position and multiplying
  // it by the position match scale factor variable.
  switch (position_match) {
  case DispMatchNone:
    break;
  case DispMatchYX:
    position[1] = position[0] * position_match_sf;
    break;
  case DispMatchZX:
    position[2] = position[0] * position_match_sf;
    break;
  case DispMatchZY:
    position[2] = position[1] * position_match_sf;
    break;
  case DispMatchZYX:
    position[2] = position[1] = position[0] * position_match_sf;
    break;
  }

  // Create switch cases when setting either the Y or Z moi to a
  // lower indexed value. Such as setting either/both Y and Z axis moi
  // to match the X moi. Or matching the Z axis moi to the Y axis.
  // This is done by taking the lower indexed axis moi and multiplying
  // it by the moi match scale factor variable.
  switch (moi_match) {
  case DispMatchNone:
    break;
  case DispMatchYX:
    moi[1] = moi[0] * moi_match_sf;
    break;
  case DispMatchZX:
    moi[2] = moi[0] * moi_match_sf;
    break;
  case DispMatchZY:
    moi[2] = moi[1] * moi_match_sf;
    break;
  case DispMatchZYX:
    moi[2] = moi[1] = moi[0] * moi_match_sf;
    break;
  }

  // Create switch cases when setting either the Y or Z poi to a
  // lower indexed value. Such as setting either/both Y and Z axis poi
  // to match the X poi. Or matching the Z axis poi to the Y axis.
  // This is done by taking the lower indexed axis poi and multiplying
  // it by the poi match scale factor variable.
  switch (poi_match) {
  case DispMatchNone:
    break;
  case DispMatchYX:
    poi[1] = poi[0] * poi_match_sf;
    break;
  case DispMatchZX:
    poi[2] = poi[0] * poi_match_sf;
    break;
  case DispMatchZY:
    poi[2] = poi[1] * poi_match_sf;
    break;
  case DispMatchZYX:
    poi[2] = poi[1] = poi[0] * poi_match_sf;
    break;
  }
}

/*******************************************************************************
disp_mag_random_number_generator
PURPOSE:(Generates the random numbers used for the mass properties variables
         that are plugged into the dispersed property variables and creates
         a value between +/- *_disp_mag.)
*******************************************************************************/
void
MassPropertiesDispersions::disp_mag_random_number_generator()
{
  std::mt19937 generator(seed_rand);
  // For future work, multiple distributions types can be made available
  // with a switch block (or similar), For now though, a uniform
  // distribution is sufficient to meet needs.
  std::uniform_real_distribution<double> rand_uniform(-1.0, 1.0);

  // Create a new random value for each component, then scale that value
  // by the respective magnitude of the allowable dispersion.
  // Thus each dispersed variables has an independently-randomized value.
  mass           = rand_uniform( generator) * mass_disp_mag;
  for (unsigned int ii = 0; ii < 3; ii++) {
    position[ii] = rand_uniform(generator) * position_disp_mag[ii];
    moi[ii]      = rand_uniform(generator) * moi_disp_mag[ii];
    poi[ii]      = rand_uniform(generator) * poi_disp_mag[ii];
  }
  // Note that the values just generated are not the *dispersed values*.
  // They are either the *dispersions* or a precursor to the dispersions,
  //  - if the dispersions are interpreted as relative, these values
  //    represent fractions of the nominal values and are multiplied by the
  //    nominal values to get the dispersion
  //  - if the dispersions are absolute, these are the dispersions.
  // The dispersions are then summed with the nominal values to get the
  // dispersed values.
}

/*******************************************************************************
scale_*
PURPOSE:(Scales the base values found in this class by the nominal values
         of the associated variables in the case where the values in
         this class represent a fractional multiple of the nominal value.)
*******************************************************************************/
void MassPropertiesDispersions::scale_mass( double nominal)
{
  mass *= nominal;
}
/******************************************************************************/
void MassPropertiesDispersions::scale_position( double nominal[3])
{
  for (unsigned int ii = 0; ii <3; ii++) {
    position[ii] *= nominal[ii];
  }
}
/******************************************************************************/
void MassPropertiesDispersions::scale_moi( double nominal_xx,
                                           double nominal_yy,
                                           double nominal_zz)
{
  moi[0] *= nominal_xx;
  moi[1] *= nominal_yy;
  moi[2] *= nominal_zz;
}
/******************************************************************************/
void MassPropertiesDispersions::scale_poi( double nominal_xy,
                                           double nominal_xz,
                                           double nominal_yz)
{
  poi[0] *= nominal_xy;
  poi[1] *= nominal_xz;
  poi[2] *= nominal_yz;
}
