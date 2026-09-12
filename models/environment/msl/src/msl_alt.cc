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

#include <cstddef>
#include <vector>

#include "../include/msl_alt.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
MslAlt::MslAlt(double &geod_alt_in,
   double &geod_lat_in,
   double &geod_lon_in)
   :
   geodLat(geod_lat_in),
   geodLon(geod_lon_in),
   geodAlt(geod_alt_in),
   mslTable(altFromTable),
   tableLat(geodLat),
   tableLon(geodLon, TableIndependentVariable::WrapAround)
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

   double scratchLat[latSize] = {0.0};
   for(int ii =0; ii<latSize; ++ii)
   {
      scratchLat[ii] = (ii-90.0) * rad_per_deg;
   }
   tableLat.load_data(scratchLat, latSize);

   double scratchLon[lonSize] = {0.0};
   for(int ii =0; ii<lonSize; ++ii)
   {
      scratchLon[ii] = ii * rad_per_deg;
   }
   tableLon.load_data(scratchLon, lonSize);

   // table-data is provided in the msl_alt_dd.cc file, which populates the
   // table[latSize][lonSize] array.
   const std::vector<std::size_t> size_vec{1, latSize, lonSize};
   mslTable.load_data(&table[0][0], size_vec );
   tableSet.add_table(mslTable);

   tableSet.add_independent_variable(tableLat);
   tableSet.associate_table_and_independent();

   tableSet.add_independent_variable(tableLon);
   tableSet.associate_table_and_independent();

   tableSet.initialize();
   SubscriptionBase::initialize();
   tableSet.subscribe();
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

   tableSet.update();
   mslAltitude = geodAlt - altFromTable;
}
