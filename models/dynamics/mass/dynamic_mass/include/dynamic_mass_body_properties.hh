/*******************************************************************************
PURPOSE:
   (Define the dynamic mass body properties.
   (The DynamicMassBodyProperties provides the mass variability capabilities.
   The mass_consumed_step is the primary means of adjusting the mass; it
   represents the mass consumed since the last update.)

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

#ifndef ANTARES_DYN_MASS_BODY_PROPERTIES_HH
#define ANTARES_DYN_MASS_BODY_PROPERTIES_HH

#include "dynamic_mass_body_interpolation.hh"

/*****************************************************************************
DynamicMassBodyPropertiesInterface
Purpose:( This is the typical interface that external models will use to 
          increment the mass they have used, and see how much remains.  It
          is deliberately small to minimize the impact on external models.
          This interface is inherited by:
           - DynamicMassProperties
             -- so is found at DynamicMassBody.dynamic_properties
           - DynamicMassString
          so instances of these types can be accepted as 
          DynamicMassBodyPropertiesInterface and models can gain access to
          these elements without having to bring in the entire model.
*****************************************************************************/
class DynamicMassBodyPropertiesInterface {
public:
   double mass_consumed_step;    /* (kg)
       The mass that has been consumed since last update.*/

   double consumable_mass;  /* (kg)
       The mass remaining to be potentially consumed.  >= 0.  */

   DynamicMassBodyPropertiesInterface();
   virtual ~DynamicMassBodyPropertiesInterface() { };
};


/*****************************************************************************
DynamicMassBodyProperties
Purpose:(Represents the time-changing properties of the mass body)
*****************************************************************************/
class DynamicMassBodyProperties : public DynamicMassBodyPropertiesInterface{
public:
   // NOTE:
   // mass_consumed_step  and
   // consumable_mass
   // are provided via inheritance from DynamicMassBodyPropertiesInterface.
   // This interface is shared with DynamicMassString

   double mass_bias;       /* (kg)
         additive mass bias                          */

   double cg_bias[3];      /* (m)
         additive cg bias                            */

   double moi_bias[3];     /* (kg*m2)
         additive moments of inertia bias            */

   double poi_bias[3];     /* (kg*m2)
         additive products of inertia bias           */

   bool mass_dispersion_flag; /* (--)
         Controls the dispersion (i.e. application of the bias values).*/

   DynamicMassBodyInterpolation       interpolation; /* (--)
         Performs the interpolation from pre-loaded data tables.*/

   // Methods:
   explicit DynamicMassBodyProperties (const double &mass_in);
   void disperse_mass_properties();

private:
   // To prevent the copy/assignment operations
   DynamicMassBodyProperties (const DynamicMassBodyProperties&);
   DynamicMassBodyProperties& operator = (const DynamicMassBodyProperties&);
};

#endif
