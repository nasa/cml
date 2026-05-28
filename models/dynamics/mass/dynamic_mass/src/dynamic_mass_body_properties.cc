/*******************************************************************************
PURPOSE:
   (Method for setting the interpolation lookup values)

PROGRAMMERS:
   ( ((Gary Turner) (OSR) (March 2014)
                      (New implementation of dynamic mass for JEOD 2.x))
    ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
   )

*******************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"

#include "../include/dynamic_mass_body_properties.hh"
#include "../include/dynamic_mass_body_nominal_properties.hh"


/*****************************************************************************
Constructors
*****************************************************************************/
DynamicMassBodyPropertiesInterface::DynamicMassBodyPropertiesInterface()
  :
  mass_consumed_step(0.0),
  consumable_mass(0.0)
{}
//*******************************************************************************
DynamicMassBodyProperties::DynamicMassBodyProperties(const double &mass_in)
  :
  DynamicMassBodyPropertiesInterface(),
  mass_bias(0.0),
  mass_dispersion_flag(false),
  interpolation(mass_in)
{
  consumable_mass = -1.0;
  jeod::Vector3::initialize(cg_bias);
  jeod::Vector3::initialize(moi_bias);
  jeod::Vector3::initialize(poi_bias);
}
//*******************************************************************************
DynamicMassBodyNominalProperties::DynamicMassBodyNominalProperties()
:
  core_mass(0.0),
  data_assigned(false)
{
  jeod::Vector3::initialize(position);
  jeod::Matrix3x3::initialize(inertia);
}

/*****************************************************************************
set_data
Purpose:(Sets the nominal data from a set of mass properties)
*****************************************************************************/
void
DynamicMassBodyNominalProperties::set_data(
            jeod::MassProperties & properties_in)
{
  // Single shot; allows subsequent calls to this method without danger of
  // overwriting true nominal properties.
  if (data_assigned) {
    return;
  }
  core_mass = properties_in.mass;
  jeod::Matrix3x3::copy(properties_in.inertia ,  inertia);
  jeod::Vector3::copy(  properties_in.position , position);
  data_assigned = true;
}

/********************************************************************************
Method: disperse_mass_properties
Purpose: (Disperse the mass properties interpolation tables by applying a bias
          uniformly to the values coded into those tables.)
********************************************************************************/
void
DynamicMassBodyProperties::disperse_mass_properties ()
{
  if (!mass_dispersion_flag) {
    return;
  }

  const size_t npts = interpolation.mass_indep.get_size();
  for (size_t kk = 0; kk < npts; ++kk) {
    if (interpolation.tab_mass_flag) {
      interpolation.mass_indep.bias_data(mass_bias, kk);
    }

    if (interpolation.tab_cg_flag) {
      interpolation.pos_dep_x.bias_data(cg_bias[0], kk);
      interpolation.pos_dep_y.bias_data(cg_bias[1], kk);
      interpolation.pos_dep_z.bias_data(cg_bias[2], kk);
    }

    if (interpolation.tab_moi_flag) {
      interpolation.moi_dep_xx.bias_data(moi_bias[0], kk);
      interpolation.moi_dep_yy.bias_data(moi_bias[1], kk);
      interpolation.moi_dep_zz.bias_data(moi_bias[2], kk);
    }

    if (interpolation.tab_poi_flag) {
      interpolation.poi_dep_xy.bias_data(poi_bias[0], kk);
      interpolation.poi_dep_xz.bias_data(poi_bias[1], kk);
      interpolation.poi_dep_yz.bias_data(poi_bias[2], kk);
    }
  }
}
