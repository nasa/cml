/*******************************************************************************
PURPOSE:
   (Define the ImpactPoint class.
   (The ImpactPoint model provides a predictor for  the position of a later
   impact, assuming a ballistic trajectory)

ASSUMPTIONS AND LIMITATIONS:
   (
    (Assumption 1: Ballistic trajectory)
    (Assumption 2: Trajectory assumed to be in ascent, meaning non-elliptical orbits
       by definition cannot have an impact point)
   )

LIBRARY DEPENDENCIES:
   ((../src/impact_point.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (Oct 2014)
        (New implementation of ball_impact for Antares))
    ((Bingquan Wang) (OSR) (May 2014)
        (cleaned up the code per its IV&V code review))
   )   
 ******************************************************************************/

#ifndef CML_IMPACT_POINT_HH
#define CML_IMPACT_POINT_HH

#include "jeod/models/utils/planet_fixed/planet_fixed_posn/include/planet_fixed_posn.hh"
#include "jeod/models/environment/planet/include/planet.hh"
#include "cml/models/dynamics/state_descriptors/range/include/range_from_pfix_reference.hh"
#include "cml/models/dynamics/state_descriptors/orb_elem_subset/include/orb_elem_subset.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

// Forward declaration
class RefFrameTrans;

class ImpactPoint : public jeod::PlanetFixedPosition, 
                    public SubscriptionBase 
{
 protected:
  const double & dyn_time; /* (s) Reference to the simulation's dynamic time.*/
  const jeod::Planet & planet_ref; /* (--) 
       Reference to the central planet. Note - planet_ref is the reference;
       "planet" is a pointer to the same place that is inherited from
       PlanetFixedPosition but is not used in this model. */
  const double & equatorial_radius; /* (m)
       The planet's equatorial-radius, obtained directly from the Planet
       object.*/
  const double & grav_mu;  /* (m3/s2) gravitational parameter of the planet.*/

 public:
  enum ImpactType{
    UNDEFINED = -1,
    IMPACT_PENDING = 0,
    HYPERBOLA_NO_IMPACT = 1,
    PARABOLA_NO_IMPACT = 2,
    HIGH_ORBIT_NO_IMPACT = 3,
    BELOW_SURF_POST_IMPACT = 4
  };

  //INPUT
  bool auto_off; /* (--) Turns off automatically when vehicle hits the ground */
  double iteration_threshold; /* (m) 
       The threshold distance between the orbital position and the planet
       surface at which the iteration is considered to have converged. */
  unsigned int iteration_count_max; /* (--) max number of iterations */

  // OUTPUT
  double surface_range;  /* (m) Total range from the launch reference point */
  double time_to_impact; /* (s) Time until impact */
  double time_of_impact; /* (s) Simulation dynamic time of predicted impact. */
  ImpactType impact_type; /* (--) Identifies type of impact. */
  RangeFromPfixReference  launch_range; /* (--) 
       The range from the reference point, typically the launch site. */

 protected:
  double planet_ecc_factor; /* (--)
      ratio involving planetary eccentricity, e^2/(1-e^2) where e is the
      planet eccentricity */
  OrbElemSubset  orb_elem; /* (--) Orbital elements used in the model */

 public:
  // Constructor
  ImpactPoint(const double & time,
              const jeod::RefFrameTrans & inrtl_state,
              const jeod::Planet & planet,
              const double & grav_mu);
  virtual ~ImpactPoint(){};
  
  void initialize();
  void initialize( double ref_pos[3]);
  void update();

 protected:
  void update_equatorial();
  void update_non_equatorial();
  void update_time();
  virtual void activate();

 private:
   // Copy constructor and assignment operator for this class are
   // declared private and are not implemented.
  ImpactPoint(const ImpactPoint&);
  ImpactPoint & operator = (const ImpactPoint&);
};
 
#endif
