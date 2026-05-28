/******************************** TRICK HEADER **********************************
PURPOSE:
    (Data structure for FirstOrderHold.)

LIBRARY DEPENDENCY:
    ((../src/first_order_hold.cc))

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (Oct 2014) (Antares))
   )
********************************************************************************/

#ifndef ANTARES_FIRST_ORDER_HOLD_HH
#define ANTARES_FIRST_ORDER_HOLD_HH

#include "cml/models/environment/atmos/atmos_exec/include/atmosphere_exec_out.hh"
#include "cml/models/dynamics/state_descriptors/extended_planetary_derived_state/include/extended_planetary_derived_state.hh"

class FirstOrderHold {
 public:
  bool FO_hold;            /* (--) First order holding will be performed */

 protected:
  bool FO_init;            /* (--) First order holding is initialized */
  double reference_time;   /* (s)  time at most recent atmos exec call */
  double previous_hold_time;    /* (s)  time the hold was last called */
  double slope_valid_time; /* (s)  time interval for which the slope is valid */

  double reference_wind[3]; /* (m/s) 
        wind velocity at most recent atmos exec call. */
  double slope_reference_wind[3]; /* (m/s) 
        wind velocity at atmos_exec call previous to reference_wind. */
  double slope[3];                /* (m/s2)  time gradient of wind */

 public:
  FirstOrderHold( const double & dyn_time_in,
                  const ExtendedPlanetaryDerivedState & planet_state,
                  AtmosExecOutput & atmos_output_in);
  virtual ~FirstOrderHold(){};

  void activate();
  void deactivate();
  void update();

 protected:
  const double & dyn_time; /* (s) reference to simulation dynamic time. */
  const ExtendedPlanetaryDerivedState & planet_state; /* (--)
           reference to vehicle's planet-state.*/
  AtmosExecOutput & out;   /* (--) reference to atmos-exec output struct */
 private:
  FirstOrderHold (const FirstOrderHold&);
  FirstOrderHold & operator = (const FirstOrderHold&);
};
#endif
