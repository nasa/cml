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
   double & geod_lat; /* (rad) Geodetic latitude  */
   double & geod_lon; /* (rad) Geodetic longitude */
   double & geod_alt; /* (m)   Geodetic altitude  */

 public:
   static constexpr int lat_size = 181; /* (--)
                                Array size for latitude for look up table */
   static constexpr int lon_size = 361; /* (--)
                                Array size for longitude for look up table */

   static const double   table[lat_size][lon_size]; /* (m)
                                Geoid vs. MSL error lookup table  */
   double   msl_altitude{0.0}; /* (m) Altitude with respect to mean sea level */

 protected:
   double                   alt_from_table{0.0};  /* (m) Altitude from the table */
   GenericMultiInputTable   msl_table; /* (--) Mean sea level height table */
   TableLookupSet           table_set; /* (--) Interpolation table manager */
   TableIndependentVariable table_lat; /* (--) Lookup table latitude */
   TableIndependentVariable table_lon; /* (--) Lookup table longitude */

 public:
   /*Constructor/Destructor*/
   MslAlt(double &geod_alt_in,
          double &geod_lat_in,
          double &geod_lon_in);

   MslAlt(const MslAlt&) = delete;
   MslAlt& operator=(const MslAlt&) = delete;
   
   /*Methods*/
   void initialize() override;
   void update();
};

#endif /*CML_MSL_ALT_HH*/