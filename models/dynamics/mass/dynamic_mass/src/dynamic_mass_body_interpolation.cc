/*******************************************************************************
PURPOSE:
  (Interpolation method for Dynamic Mass)

PROGRAMMERS:
  (
   ((Gary Turner) (OSR) (March 2014) (New implementation of dynamic mass for JEOD 2.x))
   ((Bingquan Wang) (OSR) (April 2017) (Fixed the compilation warning of float-point
                          number equality cmoparison))
    ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
   )
*******************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/dynamic_mass_body_interpolation.hh"

/********************************************************************************
Method: Constructor
********************************************************************************/
DynamicMassBodyInterpolation::DynamicMassBodyInterpolation(const double &mass_in)
  :
  position(),
  inertia(),
  mass_indep(mass_in),
  pos_dep_x(position[0]),
  pos_dep_y(position[1]),
  pos_dep_z(position[2]),
  moi_dep_xx(inertia[0][0]),
  moi_dep_yy(inertia[1][1]),
  moi_dep_zz(inertia[2][2]),
  poi_dep_xy(inertia[0][1]),
  poi_dep_xz(inertia[0][2]),
  poi_dep_yz(inertia[1][2]),
  table_lookup_set(),
  tab_mass_flag(false),
  tab_cg_flag(false),
  tab_moi_flag(false),
  tab_poi_flag(false),
  interp_position_master(true),
  interp_inertia_master(true),
  inertia_is_structural_cg(false),
  initialized(false)
{
  for (unsigned int ii = 0; ii< 3; ii++) {
    interp_position[ii] = true;
    interp_moi[ii] = true;
    interp_poi[ii] = true;
  }
  jeod::Vector3::initialize(position);
  jeod::Matrix3x3::initialize(inertia);
}

/********************************************************************************
Method: initialize
Purpose: (Configures the data-tables and checks for consistency)
********************************************************************************/
bool
DynamicMassBodyInterpolation::initialize()
{
  if (mass_indep.get_size() <= 1) {
    CMLMessage::error(
      __FILE__,__LINE__,"Initialization error.\n",
      "The number of data points on the independent variable (mass)\n"
      "needs to be more than one to do interpolation. \n");
    return false;
  }

  if (initialized) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Initialization warning.\n",
      "DynamicMassBodyInterpolation has already been initialized. "
      "It cannot be executed again. \n");
    return true;
  }

  initialized = true;

  table_lookup_set.add_independent_variable(mass_indep);
  if (pos_dep_x.is_data_loaded()) {
    if (!interp_position_master || !interp_position[0]) {
      warn_no_interp("CM position x");
    }
    table_lookup_set.add_table(pos_dep_x);
    table_lookup_set.associate_table_and_independent();
  }
  else if (interp_position_master && interp_position[0]) {
    warn_no_data("CM position x");
    interp_position[0] = false;
  }

  if (pos_dep_y.is_data_loaded()) {
    if (!interp_position_master || !interp_position[1]) {
      warn_no_interp("CM position y");
    }
    table_lookup_set.add_table(pos_dep_y);
    table_lookup_set.associate_table_and_independent();
  }
  else if (interp_position_master && interp_position[1]) {
    warn_no_data("CM position y");
    interp_position[1] = false;
  }

  if (pos_dep_z.is_data_loaded()) {
    if (!interp_position_master || !interp_position[2]) {
      warn_no_interp("CM position z");
    }
    table_lookup_set.add_table(pos_dep_z);
    table_lookup_set.associate_table_and_independent();
  }
  else if (interp_position_master && interp_position[2]) {
    warn_no_data("CM position z");
    interp_position[2] = false;
  }

  if (moi_dep_xx.is_data_loaded()) {
    if (!interp_inertia_master || !interp_moi[0]) {
      warn_no_interp("MOI Ixx");
    }
    table_lookup_set.add_table(moi_dep_xx);
    table_lookup_set.associate_table_and_independent();
  }
  else if (interp_inertia_master && interp_moi[0]) {
    warn_no_data("MOI Ixx");
    interp_moi[0] = false;
  }

  if (moi_dep_yy.is_data_loaded()) {
    if (!interp_inertia_master || !interp_moi[1]) {
      warn_no_interp("MOI Iyy");
    }
    table_lookup_set.add_table(moi_dep_yy);
    table_lookup_set.associate_table_and_independent();
  }
  else if (interp_inertia_master && interp_moi[1]) {
    warn_no_data("MOI Iyy");
    interp_moi[1] = false;
  }

  if (moi_dep_zz.is_data_loaded()) {
    if (!interp_inertia_master || !interp_moi[2]) {
      warn_no_interp("MOI Izz");
    }
    table_lookup_set.add_table(moi_dep_zz);
    table_lookup_set.associate_table_and_independent();
  }
  else if (interp_inertia_master && interp_moi[2]) {
    warn_no_data("MOI Izz");
    interp_moi[2] = false;
  }

  if (poi_dep_xy.is_data_loaded()) {
    if (!interp_inertia_master || !interp_poi[0]) {
      warn_no_interp("POI Ixy");
    }
    table_lookup_set.add_table(poi_dep_xy);
    table_lookup_set.associate_table_and_independent();
  }
  else if (interp_inertia_master && interp_poi[0]) {
    warn_no_data("POI Ixy");
    interp_poi[0] = false;
  }

  if (poi_dep_xz.is_data_loaded()) {
    if (!interp_inertia_master || !interp_poi[1]) {
      warn_no_interp("POI Ixz");
    }
    table_lookup_set.add_table(poi_dep_xz);
    table_lookup_set.associate_table_and_independent();
  }
  else if (interp_inertia_master && interp_poi[1]) {
    warn_no_data("POI Ixz");
    interp_poi[1] = false;
  }

  if (poi_dep_yz.is_data_loaded()) {
    if (!interp_inertia_master || !interp_poi[2]) {
      warn_no_interp("POI Iyz");
    }
    table_lookup_set.add_table(poi_dep_yz);
    table_lookup_set.associate_table_and_independent();
  }
  else if (interp_inertia_master && interp_poi[2]) {
    warn_no_data("POI Iyz");
    interp_poi[2] = false;
  }

  table_lookup_set.initialize();
  if (!table_lookup_set.is_initialized()) {
    initialized = false;
    return false;
  }

  set_interp_position(interp_position[0], interp_position[1], interp_position[2]);
  set_interp_moi(interp_moi[0], interp_moi[1], interp_moi[2]);
  set_interp_poi(interp_poi[0], interp_poi[1], interp_poi[2]);

  table_lookup_set.subscribe();
  initialized = true;
  return true;
}

/********************************************************************************
Method: interpolate_properties
Purpose: (uses an interpolation table to generate the position of the CG and the
          inertia tensor as a function of mass)
********************************************************************************/
bool
DynamicMassBodyInterpolation::interpolate()
{
  if (!initialized) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Interpolation error.\n",
      "DynamicMassBodyInterpolation was called prior to its initialization.\n"
      "Check your configuration.\n");
  }

  if (!table_lookup_set.update())
    return false;

  // The table-interpolation mdoel is configured to assign product-of-inertia
  // values into the lower-diagonal of the inertia matrix.  Copy those values
  // to their respective counterparts in the upper-right.
  inertia[1][0] = inertia[0][1];
  inertia[2][0] = inertia[0][2];
  inertia[2][1] = inertia[1][2];

  return true;
}


/*****************************************************************************
set_data
Purpose:(Sets the complete position and inertia values so that the
         non-interpolated elements will have meaningful values)
*****************************************************************************/
void
DynamicMassBodyInterpolation::set_data(
            jeod::MassProperties & properties_in)
{
  jeod::Vector3::copy(properties_in.position, position);
  // If the inertia interpolation-table is configured with values expressed
  // in the struct-cg frame, convert the mass-properties inertia (in body-cg
  // frame) back to inertia-cg for storage. The resulting output from the
  // interpolation will then all be in the same frame.
  if (inertia_is_structural_cg) {
    // I_struct = (T_body-to-struc) (I_body) (T_struc-to-body)
    //          = (T_struc-to-body)' (I_body) (T_struc-to-body)
    jeod::Matrix3x3::transpose_transform_matrix( properties_in.T_parent_this,
                                           properties_in.inertia,
                                           inertia);
  }
  else {
    jeod::Matrix3x3::copy(properties_in.inertia, inertia);
  }
}


/********************************************************************************
Method: set_interp_position_master
Purpose: (Enable/disable the interpolation on all the position components)
********************************************************************************/
void
DynamicMassBodyInterpolation::set_interp_position_master(bool flag)
{
  interp_position_master = flag;

  // Below to sync with table_lookup_set since the interpolation really happens
  // inside table_lookup_set.
  table_lookup_set.enable_table_interp(&pos_dep_x, flag&&interp_position[0]);
  table_lookup_set.enable_table_interp(&pos_dep_y, flag&&interp_position[1]);
  table_lookup_set.enable_table_interp(&pos_dep_z, flag&&interp_position[2]);
}


/********************************************************************************
Method: set_interp_inertia_master
Purpose: (Enable/disable the interpolation on all the inertia components)
********************************************************************************/
void
DynamicMassBodyInterpolation::set_interp_inertia_master(bool flag)
{
  interp_inertia_master = flag;

  // Below to sync with table_lookup_set since the interpolation really happens
  // inside table_lookup_set.
  table_lookup_set.enable_table_interp(&moi_dep_xx, flag&&interp_moi[0]);
  table_lookup_set.enable_table_interp(&moi_dep_yy, flag&&interp_moi[1]);
  table_lookup_set.enable_table_interp(&moi_dep_zz, flag&&interp_moi[2]);
  table_lookup_set.enable_table_interp(&poi_dep_xy, flag&&interp_poi[0]);
  table_lookup_set.enable_table_interp(&poi_dep_xz, flag&&interp_poi[1]);
  table_lookup_set.enable_table_interp(&poi_dep_yz, flag&&interp_poi[2]);
}


/********************************************************************************
Method: set_interp_position
Purpose: (Enable/disable the interpolation on an individual position component)
********************************************************************************/
void
DynamicMassBodyInterpolation::set_interp_position(
    bool flag_x,
    bool flag_y,
    bool flag_z)
{
  interp_position[0]=flag_x;
  // To sync with table_lookup_set
  table_lookup_set.enable_table_interp(&pos_dep_x, interp_position_master&&flag_x);

  interp_position[1]=flag_y;
  // To sync with table_lookup_set
  table_lookup_set.enable_table_interp(&pos_dep_y, interp_position_master&&flag_y);

  interp_position[2]=flag_z;
  // To sync with table_lookup_set
  table_lookup_set.enable_table_interp(&pos_dep_z, interp_position_master&&flag_z);
}


/********************************************************************************
Method: set_interp_moi
Purpose: (Enable/disable the interpolation on an individual MOI component)
********************************************************************************/
void
DynamicMassBodyInterpolation::set_interp_moi(
    bool flag_xx,
    bool flag_yy,
    bool flag_zz)
{
  interp_moi[0]=flag_xx;
  // To sync with table_lookup_set
  table_lookup_set.enable_table_interp(&moi_dep_xx, interp_inertia_master&&flag_xx);

  interp_moi[1]=flag_yy;
  // To sync with table_lookup_set
  table_lookup_set.enable_table_interp(&moi_dep_yy, interp_inertia_master&&flag_yy);

  interp_moi[2]=flag_zz;
  // To sync with table_lookup_set
  table_lookup_set.enable_table_interp(&moi_dep_zz, interp_inertia_master&&flag_zz);
}


/********************************************************************************
Method: set_interp_poi
Purpose: (Enable/disable the interpolation on an individual POI component)
********************************************************************************/
void
DynamicMassBodyInterpolation::set_interp_poi(
    bool flag_xy,
    bool flag_xz,
    bool flag_yz)
{
  interp_poi[0]=flag_xy;
  // To sync with table_lookup_set
  table_lookup_set.enable_table_interp(&poi_dep_xy, interp_inertia_master&&flag_xy);

  interp_poi[1]=flag_xz;
  // To sync with table_lookup_set
  table_lookup_set.enable_table_interp(&poi_dep_xz, interp_inertia_master&&flag_xz);

  interp_poi[2]=flag_yz;
  // To sync with table_lookup_set
  table_lookup_set.enable_table_interp(&poi_dep_yz, interp_inertia_master&&flag_yz);
}


/********************************************************************************
Method: set_inertia_structural_cg_flag
Purpose: (Set the flag to indicate if the interia table data is in structrual
          frame or not)
********************************************************************************/
void
DynamicMassBodyInterpolation::set_inertia_structural_cg_flag(bool flag)
{
  if (flag == inertia_is_structural_cg) {
    // nothing to do.
    return;
  }

  if (initialized) {
    CMLMessage::fail(
      __FILE__,__LINE__,"set_inertia_structural_cg_flag failed.\n",
      "The DynamicMassBodyInterpolation flag inertia_is_structural_cg is "
      "used during initialization.\n"
      "It cannot be reset after the model has been initialized.\n"
      "Attempt to reset its value failed.\n");
  }
  inertia_is_structural_cg = flag;
}


/********************************************************************************
Method: warn_no_data
Purpose: (Helper method to show the warning message for empty data table with
          interpolation flag enabled.)
********************************************************************************/
void
DynamicMassBodyInterpolation::warn_no_data(const char* const table_name)
{
   CMLMessage::error(
     __FILE__,__LINE__,"No interpolation data.\n",
     "The interpolation flag was set to true for the table ", table_name, ",\n"
     "but the table was not populated with any data.\n"
     "Interpolation flag for this table has been switched off.\n");
}


/********************************************************************************
Method: warn_no_interp
Purpose: (Helper method to show the warning message for non-empty data table with
          interpolation flag disabled.)
********************************************************************************/
void
DynamicMassBodyInterpolation::warn_no_interp(const char* const table_name)
{
   CMLMessage::warn(
     __FILE__,__LINE__,"Interpolation flag disabled  warning.\n",
     "Data have been loaded for the DynamicMassBodyInterpolation table ", table_name, ",\n"
     "but the flag controlling its usage has been set to false.\n"
     "Remember to switch the flag to true to use these data.\n"
     "Data remain available for use, but the interpolation will not execute\n"
     "until the control flag is set to true.\n");
}
