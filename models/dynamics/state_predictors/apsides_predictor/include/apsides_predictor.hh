/*******************************************************************************
PURPOSE:
   (Define the ApsidesPredictor class.
   (The ApsidesPredictor model provides a means for predicting the next apsis
   and time-to-apsis values.)

LIBRARY DEPENDENCIES:
   ((../src/apsides_predictor.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (Oct 2014) (New implementation for Antares))
    ((Bingquan Wang) (OSR) (May 2017) (cleaned up the code per its IV&V code review))
   )
 ******************************************************************************/

#ifndef CML_APSIDES_PREDICTOR_HH
#define CML_APSIDES_PREDICTOR_HH

#include "cml/models/dynamics/state_descriptors/orb_elem_subset/include/orb_elem_subset.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

// Forward declarations
namespace jeod {
class RefFrameTrans;
class Planet;
}

class ApsidesPredictor : public SubscriptionBase {

 public:
  enum NextApsisType{
    UNDEFINED = -1,
    APOAPSIS_NEXT_APSIS = 0,
    PERIAPSIS_NEXT_APSIS = 1,
    CIRCULAR_NO_APSIS = 2,
    HYPERBOLA_NO_APSIS = 3,
    PARABOLA_NO_APSIS = 4
  };
  // INPUT
  double small_tolerance; /* (--) small value check */
  double min_del_h;       /* (m) 
    Difference b/w apoapsis and periapsis for orbit to be considered circular. */
 
  // OUTPUT
  double time_to_next_apsis;     /* (s)  time still to go */
  NextApsisType next_apsis_type; /* (--) Identifier of the apo/peri apsis */

  double apoapsis_altitude;  /* (m) topocentric altitude at apoapsis */
  double periapsis_altitude; /* (m) topocentric altitude at periapsis */

 private:
  OrbElemSubset orb_elem;   /* (--) Orbital elements used in the model */
  const double & equatorial_radius; /* (m) of the planet */

 public:
  ApsidesPredictor(const jeod::RefFrameTrans & inrtl_state,
                   const jeod::Planet & planet,
                   const double & grav_mu);
  virtual ~ApsidesPredictor(){};

  void initialize();
  void update();

 protected:
  virtual void activate();

 private:
  void compute_next_apsis();

  ApsidesPredictor(const ApsidesPredictor&);
  ApsidesPredictor & operator = (const ApsidesPredictor&);
};
 
#endif
