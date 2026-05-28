/*******************************************************************************
PURPOSE:
   (Define the dynamic mass body nominal properties.
   (Nominal properties are those that are intialized wih the MassBodyInit
   Body Action.  These are not adjusted through the sim, but are used as a
   reference for computing the inertia, and for comparison if the position and
   inertia are interpolated from lookup tables.)

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
   ((../src/dynamic_mass_body_properties.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (March 2014)
                      (New implementation of dynamic mass for JEOD 2.x))
    ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
   )
 ******************************************************************************/

#ifndef ANTARES_DYN_MASS_BODY_NOMINAL_PROPERTIES_HH
#define ANTARES_DYN_MASS_BODY_NOMINAL_PROPERTIES_HH

#include "jeod/models/dynamics/mass/include/mass_properties.hh" // jeod::MassProperties

// NOTE - this class does not inherit from jeod::MassProperties because of the
// extraneous capabilities that jeod::MassPoint (parent to jeod::MassProperties)
// provides that this class will never need.  A cleaner interface is to just
// duplicate the data elements - mass, position, inertia.
class DynamicMassBodyNominalProperties {
public:
   double position[3];   /* (m)
          Center of mass location in the parent body's structural frame. */
   double core_mass;     /* (kg)    Mass (always >= 0). */
   double inertia[3][3]; /* (kg*m2) Inertia matrix in element body frame. */

protected:
   bool   data_assigned; /* (--) Nominal data has been assigned. */

// NOTE - source code found in dynamic_mass_body_properties.cc
public:
   DynamicMassBodyNominalProperties();
   void set_data( jeod::MassProperties & properties_in);

private:
   // To prevent the copy/assignment operations
   DynamicMassBodyNominalProperties (const DynamicMassBodyNominalProperties&);
   DynamicMassBodyNominalProperties& operator = (const DynamicMassBodyNominalProperties&);
};

#endif

