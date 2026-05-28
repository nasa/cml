/*******************************************************************************
   PURPOSE:
   (Provides a generic dynamic mass capability.)

   ASSUMPTIONS AND LIMITATIONS:
   ((Limitation 1: dyn_mass_init_complete must be called after other
     initialization jobs that modify a DynMass's initial mass properties.)
    (Limitation 2: Monitors m-dot only.  While it provides scaling and interpolation,
                   it does not provide capability for the computation of the
                   cause of m-dot)
    (Limitation 3:  m-dot is assumed to come off at the vehicle velocity v.
                    There is no computation of the kinematic effect on the
                    vehicle of the mass loss.  The vehicle kinetic state
                    remains unchanged)
   )

   PROGRAMMERS:
     (
      ((Gary Turner) (OSR) (March 2014)
                        (New implementation of dynamic mass for JEOD 2.x))
      ((Bingquan Wang) (OSR) (April 2017) (Disabled the compilation warning
                                 of float-point number equality comparison))
      ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
     )
*******************************************************************************/

#include <cmath> //fabs
#include "cml/models/utilities/math_utils/include/math_utils.hh"
#include "jeod/models/dynamics/mass/include/mass.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"

#include "../include/dynamic_mass_body.hh"
#include "../include/dynamic_mass_body_properties.hh"
#include "../include/dynamic_mass_body_interpolation.hh"

/********************************************************************************
Method: DynamicMassBody
Purpose: (Constructor)
********************************************************************************/
DynamicMassBody::DynamicMassBody()
:
  jeod::MassBody(),
  dynamic_properties(core_properties.mass),
  nominal_properties(),
  residual_mass(-1.0),
  interp_enabled(false),
  dyn_mass_initialized(false),
  CM_offset(),
  dry_mass_revert__consumable_mass(0.0),
  dry_mass_revert__available(false)
{}

/********************************************************************************
Method: initialize_dyn_mass
Purpose: (Sets the dynamic properties and nominal properties values)
Assumptions: (MassBodyInit initialization is already complete.
             It sets the nominal configuration for a dynamic mass.
             These values are copied into the nominal data set, then adjusted
             to account for sim configuration.)
********************************************************************************/
void
DynamicMassBody::initialize_dyn_mass()
{
  if (dyn_mass_initialized) {
    CMLMessage::warn(
      __FILE__,__LINE__,"initialize_dyn_mass warning.\n",
      "DynamicMassBody (named ", name.get_name(), ") has already been initialized.\n"
      "It can only be run once.\n");
    return;
  }

  // Check whether the jeod::MassBody has been correctly initialized, and
  // core-properties set. mass_properties_initialized is a
  // jeod::MassBody-inherited flag.
  if (!get_mass_properties_initialized()) {
    CMLMessage::error(
      __FILE__,__LINE__,"initialization sequence error\n",
      "The mass-properties for this dynamic mass have not been set.\n"
      "Cannot initialize the dynamic aspects of the mass until the\n"
      "static aspects have been initialized.\n"
      "Aborting initialization.\nBody (named ", name.get_name(), ") remains uninitialized.\n");
    return;
  }

  // end of error-detection logic, at this point the initialization will
  // proceed.
  dyn_mass_initialized = true;

  if (dynamic_properties.interpolation.mass_indep.get_size() > 1) {
    if (!interp_enabled) {
      CMLMessage::inform(
        __FILE__,__LINE__,"initialize_dyn_mass information.\n",
        "The interp_enabled flag of DynamicMassBody is not enabled.\n"
        "However, there is an interpolation data source provided; it will be\n"
        "initialized in case it is required mid-simulation.\n"
        "The interp_enabled flag must be set to utilize these data.\n");
    }
    dynamic_properties.interpolation.initialize();
  }
  else if (interp_enabled) {
    CMLMessage::warn(
      __FILE__,__LINE__,"initialize_dyn_mass warning.\n",
      "The interp_enabled flag has been enabled, but the mass data is\n"
      "inadequate to support interpolation, containing ", dynamic_properties.interpolation.mass_indep.get_size(), " data points.\n"
      "Interpolation is being disabled.\n");
    interp_enabled = false;
  }

  // Copy the initial properties into the interpolation-properties.  This is
  // necessary to ensure that the output properties where a variable is not
  // interpolated will be equal to the initial values.
  dynamic_properties.interpolation.set_data(core_properties);

  // Run the interpolation.  This will override any initial mass properties
  // where a variable has been identified for interpolation.
  // This ensures that the initial mass properties are a consistent function
  // of the initial mass before assigning them to nominal properties.
  // Note - turning off the interpolation flags will result in the initial mass
  //        properties being retained.
  if (interp_enabled &&
      dynamic_properties.interpolation.mass_indep.get_size() > 1) {
    interpolate_properties();
  }

  // Set the nominal properties equal to the initial properties (unless nominal
  // properties have already been set elsewhere)
  nominal_properties.set_data(core_properties);

  // Divide up the mass into consumable and residual.  Users may have set one or
  // more of these values already.
  // Residual-mass and consumable-mass both default to -1.0 (illegal values).
  // If they have been deliberately set, use the assigned value.
  // There are 4 cases:
  // 1. If they have both been assigned, configure the body such that any
  //    difference between the initial mass and the sum of the residual and
  //    consumable is "consumed" in the first step. Note that in this case, the
  //    nominal mass is still the initial value assigned to core-mass and not
  //    the sum of residual + consumable.
  // 2. If only the residual has been assigned, the consumable is the difference
  //    between initial and residual
  // 3. If only the consumable has been assigned, the residual is the difference
  //    between initial and consumable
  // 4. If neither has been assigned, it is assumed that residual = 0 and
  //    everything is consumable.
  bool check_sign = true;
  if (residual_mass >= 0.0) {
    if (dynamic_properties.consumable_mass >= 0.0) {
      // Use these as the initialization parameters and adjust the nominal
      // data accordingly by specifying a mass_consumed_step.
      dynamic_properties.mass_consumed_step = core_properties.mass -
                     (residual_mass + dynamic_properties.consumable_mass);
      // This will be removed at the mass-update stage of initialization so
      // that the consumable mass returns to its assigned value and the core
      // mass gets set to the sum of consumable + residual masses.
      // Note - this value can be negative in the case of overfill beyond the
      //        nominal core-mass.
      dynamic_properties.consumable_mass += dynamic_properties.mass_consumed_step;
      check_sign = false; // don't check the sign on consumable or residual
                          // because they are only temporary and core will be
                          // adjusted accordingly.
    }
    else {
      dynamic_properties.consumable_mass = core_properties.mass - residual_mass;
    }
  }
  else {
    if (dynamic_properties.consumable_mass >= 0.0) {
      residual_mass = core_properties.mass - dynamic_properties.consumable_mass;
    }
    else {
      // Assume it is all consumable.
      residual_mass = 0.0;
      dynamic_properties.consumable_mass = core_properties.mass;
    }
  }
  if (check_sign &&
      (residual_mass < 0.0 ||
       dynamic_properties.consumable_mass < 0.0)) {
    CMLMessage::error(
      __FILE__,__LINE__,"mass initialization error.\n",
      "residual mass = ", residual_mass, "\n"
      "consumable mass = ", dynamic_properties.consumable_mass, "\n"
      "These should both be >= 0.0.\n"
      "The specified core mass (", core_properties.mass, ") may be too small to accommodate the\n"
      "specified residual or consumable-mass.\n"
      "The core-mass specification should be larger than the partial mass\n"
      "(i.e. residual or consumable) specification.\n"
      "This may have repercussions in the model using this dynamic mass.\n"
      "No action taken.");
  }
}

/********************************************************************************
Method: update_mass
Purpose: (Updates the core mass properties and sets the update flag.)
Limitations: (While changes performed here will set the needs_update flag all
              the way up the tree, this method does not call the
              update_mass_properties method to actually update the mass tree.
              This method must be called independently, or, if DynamicMassGroup
              is being used, the group will perform this action.)
********************************************************************************/
bool
DynamicMassBody::update_mass() /* return: -- whether the mass has changed*/
{
  if (!dyn_mass_initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"update_mass error.\n",
      "The DynamicMassBody (named ", name.get_name(), ") has not been initialized.\n"
      "Cannot update the mass\n");
    return false;
  }

  if (std::fabs(dynamic_properties.mass_consumed_step) > 0.0) {
    dynamic_properties.consumable_mass -= dynamic_properties.mass_consumed_step;
    dynamic_properties.mass_consumed_step = 0.0;
    if (dynamic_properties.consumable_mass < 0) {
      dynamic_properties.consumable_mass = 0.0;
    }
    update_mass_internal();
  }
  return needs_update;
}

/*****************************************************************************
update_mass_internal
Purpose:(The core part of updating the mass -- sets the core-mass and
         interpolates or scales to get the position and inertia value.)
*****************************************************************************/
void
DynamicMassBody::update_mass_internal()
{
  core_properties.mass = residual_mass + dynamic_properties.consumable_mass;

  if (interp_enabled) {
    if (!interpolate_properties()) {
      CMLMessage::error(
        __FILE__,__LINE__,"Interpolation failed.\n",
        "The mass interpolation failed.\n"
        "Check the configuration on the table-interpolation model.\n");
      return;
    }
  }
  else {
    // scale the original inertia tensor by the ratio
    // between the current and original masses.
    if (nominal_properties.core_mass > 0.0) {
      double mass_ratio = core_properties.mass / nominal_properties.core_mass;
      jeod::Matrix3x3::scale(nominal_properties.inertia,
                       mass_ratio,
                       core_properties.inertia);
    }
  }

  set_update_flag(); // Sets needs_update = true
}

/********************************************************************************
Method: interpolate_properties
Purpose: (uses an interpolation table to generate the position of the CG and the
          inertia tensor as a function of mass)
********************************************************************************/
bool
DynamicMassBody::interpolate_properties()
{
   // Test whether there are any interpolation sources - indicating that
   //   interpolation tables have been loaded properly.
   if (dynamic_properties.interpolation.mass_indep.get_size() <= 1) {
     CMLMessage::warn(
       __FILE__,__LINE__,"initialize_dyn_mass warning.\n",
       "The interp_enabled flag has been enabled, but the mass data is\n"
       "inadequate to support interpolation, containing ", dynamic_properties.interpolation.mass_indep.get_size(), " data points.\n"
       "Interpolation is being disabled.\n");
     interp_enabled = false;
     return true; // not a failure, a misconfiguration.
   }

   // Attempt the interpolation/lookup, return with "false" signature if this
   // external model fails for any reason.
   if (!dynamic_properties.interpolation.interpolate()) {
     return false;
   }

   // Add the offset vector to account for the difference between the table
   // origin and the structural origin.
   jeod::Vector3::sum (CM_offset,
                 dynamic_properties.interpolation.position,
                 core_properties.position);


   if (dynamic_properties.interpolation.is_inertia_structural_cg()) {
     // Antares inertias in the data files are in the structural frame,
     // but the jeod::MassBody inertias are in the body frame.
     // So we convert these structural frame inertias to the body frame:
     // I_body = T_str_bdy * I_struct * transpose(T_str_bdy)
     jeod::Matrix3x3::transform_matrix(core_properties.T_parent_this,
                                 dynamic_properties.interpolation.inertia,
                                 core_properties.inertia);
   }
   else {
     jeod::Matrix3x3::copy( dynamic_properties.interpolation.inertia,
                      core_properties.inertia);
   }
   return true;
}

/********************************************************************************
Method:  (set_initial_position)
Purpose: (Overrides the MassBodyInit position at the start of the sim.)
Assumption: (Simply a move of the cg in the structural frame of this body,
             effectively a move in the location of the structural origin.
             That does not affect the inertia tensor, which is in the body
             frame.)
********************************************************************************/
void
DynamicMassBody::set_initial_position(
       bool overwrite_nominal,
       const double position_in[3])
{
  if (position_in == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid input.\n",
      "The position array passed in to the set_initial_position method is NULL."
      "\nThis is not a valid value.\n"
      "Attempt to set initial-position has failed.\n");
    return;
  }

  if (overwrite_nominal) {
    jeod::Vector3::copy(position_in, nominal_properties.position);
  }
  jeod::Vector3::copy(position_in, core_properties.position);
  set_update_flag();
}

/*****************************************************************************
set_CM_offset
Purpose:(provides a bias to the values in the lookup-tables such that the
         output position becomes the sum of the value from the tables and this
         vector)
*****************************************************************************/
void
DynamicMassBody::set_CM_offset(const double offset[3])
{
  if (offset == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"set_CM_offset warning.\n",
      "DynamicMassBody::set_CM_offset method was called with a NULL argument.\n"
      "Ignoring call.\n");
    return;
  }

  CMLMessage::inform(
    __FILE__,__LINE__,"set_CM_offset warning.\n",
    "The CM_offset of DynamicMassBody is being reset to (", offset[0], ", ", offset[1], ", ", offset[2], ").\n");

  CM_offset[0]=offset[0];  CM_offset[1]=offset[1];  CM_offset[2]=offset[2];
}


/*****************************************************************************
initiate_dry_mass_config
Purpose:(Removes all consumable mass)
*****************************************************************************/
void
DynamicMassBody::initiate_dry_mass_config()
{
  if (!dyn_mass_initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"update_mass error.\n",
      "The DynamicMassBody (named ", name.get_name(), ") has not been initialized.\n"
      "Cannot update the mass\n");
    return;
  }
  // Note - when reverting, we want to recreate the exact situation, so need
  //        a current copy of the consumable-mass
  dry_mass_revert__consumable_mass = dynamic_properties.consumable_mass;
  dry_mass_revert__available = true;
  // Set consumable-mass to 0.0
  dynamic_properties.consumable_mass = 0.0;
  // Update the mass properties of this body.
  update_mass_internal();
}
/*****************************************************************************
revert_dry_mass_config
Purpose:(Resets the body mass properties to what they were before switching
         to a dry-mass configuration)
*****************************************************************************/
void
DynamicMassBody::revert_dry_mass_config()
{
  if (!dry_mass_revert__available) {
    return; // return silently; messaging is handled by DynamicMassGroup.
  }
  // Resets the consumable mass to what it was.  This allows a recreation of
  // the total mass properties as they were before the dry-mass configuration
  // was applied.  Note - the mass-consumed-step value is still unaffected.
  dynamic_properties.consumable_mass = dry_mass_revert__consumable_mass;
  dry_mass_revert__available = false;
  update_mass_internal();
}
