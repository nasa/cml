/*******************************TRICK HEADER******************************
PURPOSE:
  (This model computes the offset between the WGS-84 ellipsoid and the
   EGM-2008 geoid.)

REFERENCE:
  (https://earth-info.nga.mil/GandG/wgs84/gravitymod/egm2008/egm08_wgs84.html
   on February 11, 2021.)

LIBRARY DEPENDENCY:
 ((../src/egm_2008.cc)
  (../src/egm_2008_data.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Feb 2021) (Antares) (initial)))
**********************************************************************/
#ifndef CML_EGM2008_HH
#define CML_EGM2008_HH

// CML Dependencies
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/table_interp_cpp/include/generic_multi_input_table.hh"
#include "cml/models/utilities/table_interp_cpp/include/table_independent_variable.hh"
#include "cml/models/utilities/table_interp_cpp/include/table_lookup_set.hh"

// JEOD Dependencies
#include "cml/models/utilities/cml_message/include/cml_message.hh"


/*****************************************************************************
EGM2008_WGS84
  Provides the daata lookup and interpolation algorithm for generating the
  delta between the WGS-84 ellipsoid and EGM-2008 geoid as a function of
  latitude and longitude.
*****************************************************************************/
class EGM2008_WGS84 : public SubscriptionBase
{
 public:
  double   delta_altitude; /* (m)
      Altitude of EGM2008 Geoid wrt WGS-84 ellipsoid.*/

 protected:
  TableLookupSet           table_set; /* (--) Interpolation table manager */
  GenericMultiInputTable   egm_table; /* (--) Geoid height table */
  TableIndependentVariable tiv_lat; /* (--) Lookup table latitude */
  TableIndependentVariable tiv_lon; /* (--) Lookup table longitude */


 public:
  EGM2008_WGS84( const double & wgs_84_lat,
                 const double & wgs_84_lon);
  virtual ~EGM2008_WGS84() {};

  void initialize();
  void load_data();
  void update();

 private:
  // Copy-constructor and operator= declared private and not implemented.
  EGM2008_WGS84 (const EGM2008_WGS84&);
  EGM2008_WGS84& operator= (const EGM2008_WGS84&);
};
#endif