/*******************************************************************************
PURPOSE:
   (Define the dynamic mass body interpolation capability.
   (The DynamicMassBodyInterpolation provides a table-lookup capability for
   interpolating the ch position and inertia tensor as a function of remaining
   core mass.)

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
   ((../src/dynamic_mass_body_interpolation.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (March 2014)
                      (New implementation of dynamic mass for JEOD 2.x))
    ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
   )
 ******************************************************************************/

#ifndef ANTARES_DYN_MASS_BODY_INTERPOLATION_HH
#define ANTARES_DYN_MASS_BODY_INTERPOLATION_HH

#include "jeod/models/dynamics/mass/include/mass_properties.hh" // jeod::MassProperties
#include "cml/models/utilities/table_interp_cpp/include/table_lookup_set.hh"


class DynamicMassBodyInterpolation {
public:
   double position[3];    /* (m)
         Interpolated position value */

   double inertia[3][3];  /* (kg*m2)
         Interpolated inertia tensor */

   TableIndependentVariable mass_indep; /* (--)
         Represents the independent variable in the table-lookup /
         table-interpolation algorithm.  The table-model outputs values of
         position and inertia as a function of mass.*/

   GenericMultiInputTable pos_dep_x; /* (--)
         Provides a table of values for the x-component of the position of the
         center-of-mass in the structural frame.
         This is used by the table-lookup / table-interpolation model to
         generate the position of the CoM as a function of mass. */
   GenericMultiInputTable pos_dep_y; /* (--)
         Provides a table of values for the y-component of the position of the
         center-of-mass in the structural frame.
         This is used by the table-lookup / table-interpolation model to
         generate the position of the CoM as a function of mass. */
   GenericMultiInputTable pos_dep_z; /* (--)
         Provides a table of values for the z-component of the position of the
         center-of-mass in the structural frame.
         This is used by the table-lookup / table-interpolation model to
         generate the position of the CoM as a function of mass. */

   GenericMultiInputTable moi_dep_xx; /* (--)
         Provides a table of values for the x-component of the moment-of-inertia
         (i.e. the Ixx component of the inertia tensor).
         This is used by the table-lookup / table-interpolation model to
         generate the inertia as a function of mass. */
   GenericMultiInputTable moi_dep_yy; /* (--)
         Provides a table of values for the y-component of the moment-of-inertia
         (i.e. the Iyy component of the inertia tensor).
         This is used by the table-lookup / table-interpolation model to
         generate the inertia as a function of mass. */
   GenericMultiInputTable moi_dep_zz; /* (--)
         Provides a table of values for the z-component of the moment-of-inertia
         (i.e. the Izz component of the inertia tensor).
         This is used by the table-lookup / table-interpolation model to
         generate the inertia as a function of mass. */

   GenericMultiInputTable poi_dep_xy; /* (--)
         Provides a table of values for the xy-component of the
         product-of-inertia (i.e. the Ixy component of the inertia tensor).
         This is used by the table-lookup / table-interpolation model to
         generate the inertia as a function of mass.
         Data for this variable are assumed to be negative integrals:
               -int ( x y dm)
         so the values can populate the inertia tensor directly.*/
   GenericMultiInputTable poi_dep_xz; /* (--)
         Provides a table of values for the xz-component of the
         product-of-inertia (i.e. the Ixz component of the inertia tensor).
         This is used by the table-lookup / table-interpolation model to
         generate the inertia as a function of mass.
         Data for this variable are assumed to be negative integrals:
               -int ( x y dm)
         so the values can populate the inertia tensor directly.*/
   GenericMultiInputTable poi_dep_yz; /* (--)
         Provides a table of values for the yz-component of the
         product-of-inertia (i.e. the Iyz component of the inertia tensor).
         This is used by the table-lookup / table-interpolation model to
         generate the inertia as a function of mass.
         Data for this variable are assumed to be negative integrals:
               -int ( x y dm)
         so the values can populate the inertia tensor directly.*/

   TableLookupSet table_lookup_set; /* (--)
         Provides the collective set of tables, managing the
         mass_indep, pos*, moi* and poi* tables together under one umbrealla.*/

   // The following flags allow DynamicsMassBodyProperties::*_bias to
   // be added to the respective values in the table so that the
   // tables may be shifted.
   bool tab_mass_flag; /* (--)
         Flag to modify the mass table. */

   bool tab_cg_flag;  /* (--)
         Flag to modify the position table. */

   bool tab_moi_flag; /* (--)
         Flag to modify the moments of inertia in the inertia table. */

   bool tab_poi_flag; /* (--)
         Flag to modify the products of inertia in the inertia table. */

   explicit DynamicMassBodyInterpolation(const double &mass_in);
   virtual ~DynamicMassBodyInterpolation(){};

   bool initialize();
   bool interpolate();
   void set_data( jeod::MassProperties & properties_in);

   void set_interp_position_master(bool flag);
   void set_interp_inertia_master(bool flag);
   void set_interp_position(bool flag_x, bool flag_y, bool flag_z);
   void set_interp_moi(bool flag_xx, bool flag_yy, bool flag_zz);
   void set_interp_poi(bool flag_xy, bool flag_xz, bool flag_yz);

   bool is_inertia_structural_cg() const { return inertia_is_structural_cg; }
   void set_inertia_structural_cg_flag(bool flag);

protected:
   void warn_no_interp(const char* const table_name);
   void warn_no_data(const char* const table_name);

private:
   // to disable the copy/assignment operations
   DynamicMassBodyInterpolation (const DynamicMassBodyInterpolation&);
   DynamicMassBodyInterpolation& operator = (const DynamicMassBodyInterpolation&);

   bool interp_position_master; /* (--)
        controls access to all position interpolation/lookup options.*/
   bool interp_inertia_master; /* (--)
        controls access to all inertia interpolation/lookup options. */
   bool interp_position[3]; /* (--)
        3 flags that independently control access to interpolation/lookup
        of position on each of the 3 axes.  Unused if the position-master flag
        is switched off.*/
   bool interp_moi[3]; /* (--)
        3 flags that independently control access to interpolation/lookup of
        the 3 moments of inertia. Unused if the inertia-master flag is switched
        off.*/
   bool interp_poi[3]; /* (--)
        3 flags that independently control access to interpolation/lookup of
        the 3 (negative) products of inertia. Unused if the inertia-master flag
        is switched off.*/

   bool inertia_is_structural_cg;  /* (--)
        Used to specify the interpretation of the data representing the inertia
        tensor.  Typically, inertia is specified relative to body axes
        and center-of-mass.  If data is provided for the inertia expressed
        relative to structural axes, this flag will trigger an internal
        conversion.*/
   bool initialized; /* (--)
        flag to indicate whether method initialize() has been called. */
};

#endif
