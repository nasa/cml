/*******************************TRICK HEADER******************************
PURPOSE: (This model determines the altitude based on the Mean Sea Level
(MSL).  This model is based off of the GROK MSL ALT function written in
2013 by Denise Brown)

LIBRARY DEPENDENCY:
 ((../src/msl_alt_dd.cc)
  (../src/msl_alt.cc))

PROGRAMMERS:
  (((Rachel Borland) (NASA) (Nov 2019) (Antares) (initial)))
**********************************************************************/
#ifndef CML_MSL_ALT_HH
#define CML_MSL_ALT_HH

// CML Dependencies
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/table_interp_cpp/include/generic_multi_input_table.hh"
#include "cml/models/utilities/table_interp_cpp/include/table_independent_variable.hh"
#include "cml/models/utilities/table_interp_cpp/include/table_lookup_set.hh"

/*****************************************************************************
MslAlt
Purpose:(Provides an altitude based on the MSL.)
*****************************************************************************/
class MslAlt : public SubscriptionBase
{
 protected: // External references
   double & geodLat; /* (rad) Geodetic latitude  */
   double & geodLon; /* (rad) Geodetic longitude */
   double & geodAlt; /* (m)   Geodetic altitude  */

 public:
   static const int latSize = 181; /* (--)
                                Array size for latitude for look up table */
   static const int lonSize = 361; /* (--)
                                Array size for longitude for look up table */

   static const double   table[latSize][lonSize]; /* (m)
                                Geoid vs. MSL error lookup table  */
   double   mslAltitude{0.0}; /* (m) Altitude with respect to mean sea level */

 protected:
   static const double      rad_per_deg; /* (rad/degree) unit converter.*/
   double                   altFromTable{0.0};  /* (m) Altitude from the table */
   GenericMultiInputTable   mslTable; /* (--) Mean sea level height table */
   TableLookupSet           tableSet; /* (--) Interpolation table manager */
   TableIndependentVariable tableLat; /* (--) Lookup table latitude */
   TableIndependentVariable tableLon; /* (--) Lookup table longitude */

 public:
   /*Constructor/Destructor*/
   MslAlt(double &geod_alt_in,
          double &geod_lat_in,
          double &geod_lon_in);

   /*Methods*/
   void initialize() override;
   void update();

 private:
   // Disable the copy/assignment operations
   MslAlt (const MslAlt&) = delete;
   MslAlt& operator = (const MslAlt&) = delete;
};

#endif /*CML_MSL_ALT_HH*/