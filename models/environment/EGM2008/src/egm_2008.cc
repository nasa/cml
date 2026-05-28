/*******************************TRICK HEADER******************************
PURPOSE:
  (This model computes the offset between the WGS-84 ellipsoid and the
   EGM-2008 geoid.)

REFERENCE:
  (https://earth-info.nga.mil/GandG/wgs84/gravitymod/egm2008/egm08_wgs84.html
   on February 11, 2021.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Feb 2021) (Antares) (initial)))
**********************************************************************/

// Local header:
#include "../include/egm_2008.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
EGM2008_WGS84::EGM2008_WGS84( const double & wgs_84_lat,
                              const double & wgs_84_lon)
  :
  delta_altitude(0),
  table_set(),
  egm_table(delta_altitude),
  tiv_lat(wgs_84_lat),
  tiv_lon(wgs_84_lon, TableIndependentVariable::WrapAround)
{}

/*****************************************************************************
initialize
Purpose:
  reads in the data, populates the tables prepares for interpolation
*****************************************************************************/
void
EGM2008_WGS84::initialize()
{
  if (!enabled) return;

  load_data();
  table_set.subscribe();
  table_set.initialize();

  if (!table_set.is_active()) {
    CMLMessage::error(
      __FILE__,__LINE__,"Table-initialization error.\n",
      "Error in initializing the lookup table.\n"
      "EGM2008 Geoid cannot be constructed.\n"
      "Initialization failed.\n");
    return;
  }
  SubscriptionBase::initialize();
  update();
}

/*****************************************************************************
update
Purpose:
  Process the table-lookup
*****************************************************************************/
void
EGM2008_WGS84::update()
{
  if (!active) return;

  table_set.update();
}