/*******************************************************************************
Purpose:
   (POD class containing data describing the atmospheric state in the
   vicinity of the payload.
   This class works with LaggedAtmosWind.
   For LaggedAtmosphere or LaggedWinds, use PayloadData.)
*******************************************************************************/
#ifndef CML_LAGGED_ATMOS_PAYLOAD_DATA_HH
#define CML_LAGGED_ATMOS_PAYLOAD_DATA_HH

#include "jeod/models/utils/math/include/vector3.hh"

class LaggedAtmosPayloadData {
  public:

  double altitude; /* (m)
      Altitude of interest. This variable is used as the
      independent variable in determining the other values of atmospheric
      data.  The values that go to output are interpolated from the nodes
      using altitude as the input.  This will most commonly refer to a
      topodetic altitude, but the type of altitude is not particularly
      relevant as long as it is consistent between the type used to populate
      the nodes and the type of altitude used as the input.*/

  double density; /* (kg/m3)
      Local atmospheric density at the payload. */

  double speed_of_sound; /* (m/s)
      Local atmospheric speed of sound at the payload. */

  double planetodetic_wind_velocity[3]; /* (m/s)
      Local wind velocity at the payload, resolved in
      the payload's planetodetic NED frame. */


  // constructor
  LaggedAtmosPayloadData()
    :
    altitude(0.0),
    density(0.0),
    speed_of_sound(0.0),
    planetodetic_wind_velocity()
  {
    jeod::Vector3::initialize(planetodetic_wind_velocity);
  }

  // copy-constructor
  LaggedAtmosPayloadData( const LaggedAtmosPayloadData& orig)
    :
    altitude(orig.altitude),
    density(orig.density),
    speed_of_sound(orig.speed_of_sound)
  {
    jeod::Vector3::copy(orig.planetodetic_wind_velocity,
                        planetodetic_wind_velocity);
  }
};
#endif
