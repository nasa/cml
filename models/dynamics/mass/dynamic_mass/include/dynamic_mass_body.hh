/*******************************************************************************
PURPOSE:
   (Define the dynamic mass body.
   (The DynamicMassBody is the extension of the JEOD jeod::MassBody concept, adding
    nominal properties and dynamic properties to the core-properties and
    composite-properties of the jeod::MassBody.)

ASSUMPTIONS AND LIMITATIONS:
   ((Assumption 1: The DynamicMassBody will be initialized in the same way as a
                   regular jeod::MassBody, using the MassBodyInit jeod::BodyAction)
    (Assumption 2: Following MassBodyInit initialization, the
                   initialize_dyn_mass method sets the nominal properties and
                   initializes the dynamic properties.)
    (Assumption 3: changes to the dynamic mass are made by changing the
                   mass_consumed_step variable in the dynamic properties.  This
                   value is set to zero at each update cycle, so it is safe to
                   increment it in the event that there may be more than one
                   affecting consideration)
    (Assumption 4: unless the interpolation is used, the position of the cg
                   remains constant and the core-properties inertia tensor scales
                   with mass.)
    (Assumption 5: if the interpolation is being used, it is assumed that the user
                   correctly loads the interpolation tables.)
    (Limitation 1: The update_mass method updates the core properties and sets
                   the flag for the tree update, but does not call
                   update_mass_properties on the root body.  That is left to an
                   independent call or to the DynamicMassGroup update method.)
   )

LIBRARY DEPENDENCIES:
   ((../src/dynamic_mass_body.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (March 2014)
                      (New implementation of dynamic mass for JEOD 2.x))
    ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
   )
 ******************************************************************************/

#ifndef CML_DYN_MASS_BODY_HH
#define CML_DYN_MASS_BODY_HH

#include "jeod/models/dynamics/mass/include/mass.hh"

#include "dynamic_mass_body_properties.hh"
#include "dynamic_mass_body_nominal_properties.hh"

// Inherit directly from JEOD Mass Body
class DynamicMassBody : public jeod::MassBody
{
 public:
  DynamicMassBodyProperties dynamic_properties; /* (--)
     Additional properties linked to the composite-properties of the original
     jeod::MassBody.*/

  DynamicMassBodyNominalProperties  nominal_properties; /* (--)
     Typical mass-properties as initialized with MassBodyInit prior to
     adjusting mass properties within the sim. */

  double residual_mass; /* (kg)
      The residual mass for this propellant tank.
      Total core mass = this value + dynamic_properties.consumable_mass*/
  bool interp_enabled; /* (--)
      Whether to interpolate the position and inertia from data lookup
      tables, or simply use scaling.*/
 private:
  bool dyn_mass_initialized; /* (--)
      Flag to inidicate whether method initialize_dyn_mass() has been called. */
  double CM_offset[3];   /* (m)
        The CM offset from parent frame to CM table Frame */

  double dry_mass_revert__consumable_mass; /* (kg)
      The consumable-mass of record at the time the model was transitioned
      to check on dry-mass data.*/
  bool dry_mass_revert__available; /* (--)
      Internal flag indicating that dry-mass-revert data is available.*/

 public:
  DynamicMassBody ();
  void initialize_dyn_mass(); // Run AFTER MassBodyInit.
  bool update_mass();
  void set_initial_position( bool overwrite_nominal,
                             const double pos[3]);
  void set_CM_offset(const double offset[3]);
  bool get_dyn_mass_initialized() {return dyn_mass_initialized;}

  void initiate_dry_mass_config();
  void revert_dry_mass_config();

  bool get_dry_mass_revert__available() {return dry_mass_revert__available;}

 protected:
  void update_mass_internal();
  bool interpolate_properties();

 private:
  // To disable the copy/assignment operations
  DynamicMassBody (const DynamicMassBody&);
  DynamicMassBody& operator = (const DynamicMassBody&);

};

#endif

