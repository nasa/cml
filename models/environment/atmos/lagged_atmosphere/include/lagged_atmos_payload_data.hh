/*******************************************************************************
Purpose:
   (POD class containing data describing the atmospheric state in the
   vicinity of the payload.
   This class works with LaggedAtmosWind.
   For LaggedAtmosphere or LaggedWinds, use PayloadData.)
*******************************************************************************/
#ifndef CML_LAGGED_ATMOS_PAYLOAD_DATA_HH
#define CML_LAGGED_ATMOS_PAYLOAD_DATA_HH

namespace cml {

class LaggedAtmosPayloadData {
  public:

  double altitude{0.0}; /* (m)
      Altitude of interest. This variable is used as the
      independent variable in determining the other values of atmospheric
      data.  The values that go to output are interpolated from the nodes
      using altitude as the input.  This will most commonly refer to a
      topodetic altitude, but the type of altitude is not particularly
      relevant as long as it is consistent between the type used to populate
      the nodes and the type of altitude used as the input.*/

  double density{0.0}; /* (kg/m3)
      Local atmospheric density at the payload. */

  double speed_of_sound{0.0}; /* (m/s)
      Local atmospheric speed of sound at the payload. */

  double planetodetic_wind_velocity[3]{}; /* (m/s)
      Local wind velocity at the payload, resolved in
      the payload's planetodetic NED frame. */


  // constructor
  LaggedAtmosPayloadData() = default;

  // copy-constructor
  LaggedAtmosPayloadData(const LaggedAtmosPayloadData& orig) = default;
  // copy assignment operator
  LaggedAtmosPayloadData& operator=(const LaggedAtmosPayloadData& orig) = default;
};


} // namespace cml

#endif