/*******************************TRICK HEADER**********************************
PURPOSE: (Uses JEOD radiation pressure model components to generate an
          illumination fraction without executing the whole radiation
          pressure model.)

ASSUMPTION: (The vehicle is not partially eclipsed by Earth and the Moon at
             the same time)

LIBRARY DEPENDENCY:
   ((../src/eclipse_calculator.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2019) (Antares) (initial)))
*****************************************************************************/

#ifndef CML_ECLIPSE_CALCULATOR_HH
#define CML_ECLIPSE_CALCULATOR_HH

#include "jeod/models/interactions/radiation_pressure/include/radiation_source.hh"
#include "jeod/models/interactions/radiation_pressure/include/radiation_third_body.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

namespace jeod {
class DynManager;
class RefFrame;
}

class EclipseCalculator : public SubscriptionBase
{
 protected:
  const double & time; /* (s)
     Reference to the dynamic time; this is used by the RadiationThirdBody
     update.*/
  jeod::DynManager & dyn_manager; /* (--)
      Reference to the dynamics manager / ephemeris manager.*/
  jeod::RefFrame & vehicle_frame; /* (--)
      Reference to the vehicle reference frame being tested for being in
      eclipse */
  char earth_name[6]; /* (--) "Earth" */
  char moon_name[5];  /* (--) "Moon" */

  jeod::RadiationSource sun; /* (--) Radiation source provider */
  jeod::RadiationThirdBody earth; /* (--) eclipsing body */
  jeod::RadiationThirdBody moon; /* (--) eclipsing body */

 public:
  double illum_factor; /* (--)
      Output factor indicating the fraction of the solar disk currently
      visible by veh_struc_frame.*/


  EclipseCalculator(const double & time,
                    jeod::DynManager & dyn_manager,
                    jeod::RefFrame & vehicle_frame);
  ~EclipseCalculator() override = default;

  void initialize() override;
  void update();
  void set_earth_shadow_geometry(jeod::RadiationThirdBody::ShadowGeometry new_sg) {
    earth.shadow_geometry = new_sg;
  }
  void set_moon_shadow_geometry(jeod::RadiationThirdBody::ShadowGeometry new_sg) {
    moon.shadow_geometry = new_sg;
  }

 protected:
  void activate() override;
  void deactivate() override;

 private: // and undefined:
  EclipseCalculator (const EclipseCalculator& rhs);
  void operator = (const EclipseCalculator& rhs);
};
#endif
