/*******************************************************************************
Purpose:
   (Atmosphere state for a body that lags another body's atmosphere. For intance,
  chutes and payload.)

Library Dependency:
   ((../src/lagged_atmos_wind.cc))

Author:
 (((Gary Turner) (OSR) (Jan 2021) (Antares)
    (Based on LaggedAtmospehre and LaggedWinds by Jeff Semrau,
     written to support EG-chutes code.))
  )
*******************************************************************************/

#ifndef LAGGED_ATMOS_WIND_HH
#define LAGGED_ATMOS_WIND_HH

#include <list>
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "lagged_atmos_payload_data.hh"

// The atmosphere of the payload is saved as a function of altitude, and is
// then interpolated using the parachute's altitude.  This causes the
// parachute to fly through the same atmosphere profile as the payload, but
// with a lag based on the difference in altitude.

class LaggedAtmosWind {

  public:
  LaggedAtmosPayloadData data_out; /* (--)
    Set of data interpolated from the history at the specified altitude.*/

  LaggedAtmosWind();
  LaggedAtmosWind( const std::string& name);
  virtual ~LaggedAtmosWind() {};


  void compute( double input_altitude);
  void compute_with_query( double input_altitude);

  void update_history( const LaggedAtmosPayloadData & payload_data);
  void set_min_delta_altitude( double min_delta_altitude);
  void set_max_delta_altitude( double max_delta_altitude);

  protected:
  std::string model_name; /* (--)
    Model name used for messages.*/
  // Parameters...
  double min_delta_altitude; /* (m)
     Minimum amount by which altitude must decrease before a new node
     is recorded. NOTE -- this must be less than the shortest distnace
     over which lagging will be applied (e.g. distance ebtween body and
     parachute), or the lagged entity (parachute) will move below the
     lowest node, before the enxt node is generated, which results in
     sim-termination.  Default: 0.0 (record everything)*/
  double max_delta_altitude; /* (m)
    Maximum altitude above the payload for which nodes are
    retained. */

  bool initialized; /* (--)
     flag indicating that the nodes list has been populated with at
     least one element.  Default: false.*/

  std::list<LaggedAtmosPayloadData> nodes; /* (--)
     A set of {altitude, density, speed-of-sound} data elements from
     the vehicle's history.  These data points are ordered by altitude,
     monotonically decreasing.  A new point can only be added to this
     list when the vehicle altitude is lower than that of the last point
     in the list, see the update_history method.*/
  std::list<LaggedAtmosPayloadData>::iterator above; /* (--)
     The location in the nodes list that is immediately above the current
     altitude.  The nodes list is monotonically ordered by
     decreasing altitude, so this reference point is to the "left" of the input
     altitude.*/

  private:
  LaggedAtmosWind(const LaggedAtmosWind &);
  LaggedAtmosWind & operator=(const LaggedAtmosWind &);
};

#endif
