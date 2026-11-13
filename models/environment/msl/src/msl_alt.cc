/********************************* TRICK HEADER *******************************
PURPOSE:
   (Define method to determine MSL Altitude)

PROGRAMMERS:
   (((D. Brown) (Odyssey) (April 2013) (Initial implementation of MSL altitude))
    ((D. Merritt) (NASA)    (May   2013) (ANTARES) (AGDL200001016 - Initial
          Implementation in Grok V1.7.))
    ((D. Merritt) (NASA)    (Jan   2015) (ANTARES) (RM#1570 - Change enable
          to flag_enable and fix units and initialize internal variables))
    ((D. Merritt) (NASA)    (Feb   2015) (ANTARES) (RM#1570 - Move library
          definition from code to headers))
    ((R. Borland) (NASA) (Nov 2019) (C++ Implementation for ANTARES))
   )
*******************************************************************************/
#include "../include/msl_alt.hh"

#include <cstddef>
#include <vector>

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
MslAlt::MslAlt(double &geod_alt_in,
   double &geod_lat_in,
   double &geod_lon_in)
   :
   geod_lat(geod_lat_in),
   geod_lon(geod_lon_in),
   geod_alt(geod_alt_in),
   msl_table(alt_from_table),
   table_lat(geod_lat),
   table_lon(geod_lon, TableIndependentVariable::WrapAround)
{
   subscribe_name = "Mean Sea Level Altitude:";
}

/*****************************************************************************
initialize
Purpose:(Required initialization, sets )
*****************************************************************************/
void MslAlt::initialize()
{
   if (!enabled) { return; }

   constexpr double deg_to_rad = 1.0 / 57.29577951308231;

   double scratch_lat[lat_size] = {0.0};
   for(int ii = 0; ii < lat_size; ++ii)
   {
      scratch_lat[ii] = (ii-90.0) * deg_to_rad;
   }
   table_lat.load_data(scratch_lat, lat_size);

   double scratch_lon[lon_size] = {0.0};
   for(int ii = 0; ii < lon_size; ++ii)
   {
      scratch_lon[ii] = ii * deg_to_rad;
   }
   table_lon.load_data(scratch_lon, lon_size);

   // table-data is provided in the msl_alt_dd.cc file, which populates the
   // table[lat_size][lon_size] array.
   const std::vector<std::size_t> size_vec{1, lat_size, lon_size};
   msl_table.load_data(&table[0][0], size_vec );
   table_set.add_table(msl_table);

   table_set.add_independent_variable(table_lat);
   table_set.associate_table_and_independent();

   table_set.add_independent_variable(table_lon);
   table_set.associate_table_and_independent();

   table_set.initialize();
   SubscriptionBase::initialize();
   table_set.subscribe();
   update();
}

/*****************************************************************************
update
Purpose:(main executable)
*****************************************************************************/
void  MslAlt::update()
{
   //if the MSL is not active, get out of here
   if (!active) { return; }

   table_set.update();
   msl_altitude = geod_alt - alt_from_table;
}