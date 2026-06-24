/*******************************************************************************
PURPOSE:
   (The ExtendedPlanetaryDerivedState extends the PlanetaryDerivedState to
    provide vehicle state with respect to various frames, including
    Topocentric/Topodetic NED, Boost Reference, Plumbline,
    Launch Range, Landing Range, EI Range, Range Safety,
    Pad SEU and left-handed pad frames,
    Hang and Roll angles )

LIBRARY DEPENDENCIES:
    ((../src/extended_planetary_derived_state.cc)
     (../src/topo_container.cc)
    )

PROGRAMMERS:
   (((Gary Turner) (OSR) (June 2014)
                   (New implementation of PlanetaryDerivedState for Antares))
    ((Bingquan Wang) (OSR) (May 2017)
                   (Moved the method acos_proctd into cml/models/utilities/math_utils))
    ((Dan Jordan) (Jacobs/JETS) (July 2017)
                   (EM1 V&V cleanup))
   )
 ******************************************************************************/

#ifndef CML_EXTENDED_PLANETARY_DERIVED_STATE_HH
#define CML_EXTENDED_PLANETARY_DERIVED_STATE_HH

#include "jeod/models/dynamics/derived_state/include/planetary_derived_state.hh"
#include "cml/models/dynamics/state_descriptors/range/include/range_from_pfix_reference.hh"
#include "cml/models/dynamics/state_descriptors/range/include/range_to_pfix_reference.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "range_safety_data.hh"
#include "topo_container.hh"
#include "point_to_point.hh"

/*****************************************************************************
ExtendedPlanetaryDerivedState
Purpose:(Provides a plethora of options for expressing the state relative
         to a planet.)
*****************************************************************************/
class ExtendedPlanetaryDerivedState : public jeod::PlanetaryDerivedState,
                                      public SubscriptionBase
{
 public:
  // Reference vectors for hang and roll angles
  double hang_angle_body_vec[3]; /* (--) Unit body vector used for hang angle */
  double roll_wrt_hdg_body_vec[3];  /*  (--)
           Unit body vector use for roll angle wrt heading*/

  // Reference point coordinates for boost reference and plumbline frames
  double br_ref_longitude;        /* (rad) Longitude of boost-reference
                                           origin.*/
  double br_ref_geodetic_latitude;/* (rad) Geodetic latitude of boost-
                                           reference origin */
  double pl_ref_longitude;        /* (rad) Longitude of plumbline origin */
  double pl_ref_geodetic_latitude;/* (rad) Geodetic latitude of plumbline
                                           origin */
  double pl_ref_azimuth;          /* (rad) Reference azimuth for plumbline
                                           frame */

  // OUTPUT VALUES:

  RangeFromPfixReference launch_range;  /* (--) Range from user-specified
                                                launch site to vehicle */
  RangeToPfixReference   landing_range; /* (--) Range from vehicle to
                                                user-specified landing site */
  RangeFromPfixReference entry_range;   /* (--) Range from user-specified
                                                location of entry interface
                                                to the vehicle */

  // Positions
  double topocentric_altitude;    /* (m)  radial above the ellipsoid. */
  PointToPointManager pt_to_pt;   /* (--) the set of pt-to-pt computations
                                          between a vehicle point and a
                                          planetary point.*/

  // Range Safety Data:
  RangeSafetyData range_safety;   /* (--) Range safety data, state values in
                                          the pad and VRT frames */

  TopoContainer topodetic;   /* (--) State values in the topodetic NED frame */
  TopoContainer topocentric; /* (--) State values in the topocentric NED frame*/

  // Velocity - inertial
  double inrtl_vel_mag;      /* (m/s) Magnitude of the inertial velocity.*/

  // Velocity - Planet-relative:
  double relative_vel[3];    /* (m/s) Planet-relative velocity (expressed
                                      in inrtl) */
  double relative_vel_mag;   /* (m/s) Magnitude of relative_vel */
  double pfix_rel_vel[3];    /* (m/s) Planet-relative velocity (expressed
                                      in pfix) */

  // Planet-relative acceleration - pfix cartesian:
  double relative_accel[3];  /* (m/s2) Planet-relative acceleration vector*/

  // Transformation matrices
  double T_inrtl_br[3][3];   /* (--) Inertial-to-boost-reference */
  double T_inrtl_pl[3][3];   /* (--) Inertial-to-plumbline */

  // Euler angles
  double E_br_body_YPR[3];   /* (rad) Boost reference to body Yaw-Pitch-Roll */
  double E_pl_body_PYR[3];   /* (rad) Plumbline to body Pitch-Yaw-Roll */

  // Hang and roll angles
  double hang_angle;         /*  (rad) Angle between the Topodetic-Z (down)
                                       vector and a unit vector specified in
                                       the body frame */

  double roll_wrt_heading; /*  (rad)
           Angle between the horizontal projection of the Topodetic relative
           velocity vector and a unit vector specified in the body frame and
           projected onto the Topodetic horizontal plane.*/
 protected:
   // Protected counts of number of models requiring this particular feature
   // Models that increment this count should decrement it
   // Users should use sub/unsub-scribe_* methods, which increment/decrement
   // these counts and also make sure that initialization has been performed
   // when applicable
   int calc_rel_vel;               /* (--)
       subscription-count to rel-vel computation. */
   int calc_topocentric;           /* (--)
       subscription-count to topocentric computation.*/
   int calc_topocentric_altitude;  /* (--)
       subscription-count to topocentric-altitude computation.
       Separated from topocentric primarily so that it can be called without
       invoking the full topocentric.  */
   int calc_topodetic;             /* (--)
       subscription-count to topodetic computation.*/
   int calc_pt_to_pt;        /* (--)
       subscription-count to point-to-point computations.*/

   int calc_boost_reference;      /* (--)
       subscription-count to boost-ref computation.*/
   int calc_plumbline;            /* (--)
       subscription-count to plumbline computation.*/
   int calc_range_safety;         /* (--)
       subscription-count to computation.*/
   int calc_hang_roll;            /* (--)
       subscription-count to hang-roll computation.*/

   bool boost_ref_initialized;    /* (--)
       internal flag; boost-ref init method processed*/
   bool plumbline_initialized;    /* (--)
       internal flag; plumbline init method processed*/
   bool range_safety_initialized; /* (--)
        internal flag; range-safety init method processed*/
   bool using_launch_range;       /* (--)
       internal flag; subscription to launch range model.
       NOTE - only used during initialization, run-time queries ping the
       launch-range active flag directly.  So once this is set, it is never
       unset, even if all subscriptions go away.*/
   bool using_landing_range;      /* (--)
       internal flag; subscription to landing range model.
       NOTE - only used during initialization, run-time queries ping the
       landing-range active flag directly.  So once this is set, it is never
       unset, even if all subscriptions go away.*/
   bool using_entry_range;        /* (--)
       internal flag; subscription to entry range model.
       NOTE - only used during initialization, run-time queries ping the
       entry-range active flag directly.  So once this is set, it is never
       unset, even if all subscriptions go away.*/


 public:

   ExtendedPlanetaryDerivedState(void);
   virtual ~ExtendedPlanetaryDerivedState(void){};
   using SubscriptionBase::initialize;
   virtual void initialize(jeod::DynBody & subject_body, jeod::DynManager & dyn_manager);

   virtual void update (void);

   inline void subscribe_rel_vel()              { calc_rel_vel++;}
   inline void subscribe_topocentric()          { calc_topocentric++;}
   inline void subscribe_topocentric_altitude() { calc_topocentric_altitude++;}
   inline void subscribe_topodetic()            { calc_topodetic++;}
   inline void subscribe_pt_to_pt()             { calc_pt_to_pt++;}
   void subscribe_hang_roll();
   void subscribe_boost_reference();
   void subscribe_plumbline();
   void subscribe_launch_range();
   void subscribe_landing_range();
   void subscribe_entry_range();
   void subscribe_range_safety();

   /*****************************************************************************
   unsubscribe_*()()
   Purpose: Decrement subscription of submodels by one
   *****************************************************************************/
   inline void unsubscribe_rel_vel()              {calc_rel_vel--;}
   inline void unsubscribe_topocentric()          {calc_topocentric--;}
   inline void unsubscribe_topocentric_altitude() {calc_topocentric_altitude--;}
   inline void unsubscribe_topodetic()            {calc_topodetic--;}
   inline void unsubscribe_pt_to_pt()             {calc_pt_to_pt--;}
   inline void unsubscribe_hang_roll()            {calc_hang_roll--;}
   inline void unsubscribe_boost_reference()      {calc_boost_reference--;}
   inline void unsubscribe_plumbline()            {calc_plumbline--;}
   inline void unsubscribe_launch_range()         {launch_range.unsubscribe();}
   inline void unsubscribe_landing_range()        {landing_range.unsubscribe();}
   inline void unsubscribe_entry_range()          {entry_range.unsubscribe();}
   inline void unsubscribe_range_safety()         {calc_range_safety--;}


   /*****************************************************************************
   is*_subscribed()
   Purpose: Return whether or not model has at least one subscription to
            submodels
   *****************************************************************************/
   inline bool is_rel_vel_subscribed()       {return (calc_rel_vel > 0);}
   inline bool is_topocentric_subscribed()   {return (calc_topocentric > 0);}
   inline bool is_topocentric_altitude_subscribed() {
                                    return (calc_topocentric_altitude > 0);}
   inline bool is_topodetic_subscribed()     {return (calc_topodetic > 0);}
   inline bool is_pt_to_pt_subscribed()      {return (calc_pt_to_pt > 0);}
   inline bool is_hang_roll_subscribed()     {return (calc_hang_roll > 0);}
   inline bool is_boost_reference_subscribed() {return (calc_boost_reference>0);}
   inline bool is_plumbline_subscribed()     {return (calc_plumbline > 0);}
   inline bool is_launch_range_subscribed()  {return (using_launch_range);}
   inline bool is_landing_range_subscribed() {return (using_landing_range);}
   inline bool is_entry_range_subscribed()   {return (using_entry_range);}
   inline bool is_range_safety_subscribed()  {return (calc_range_safety > 0);}

protected:
   void init_boost_reference();
   void init_plumbline();
   void init_range_safety();
   virtual void activate()  { active = true; update();}
   void calculate_topocentric_altitude();
   void calculate_relative_vel();
   void calculate_relative_accel();
   void hang_roll();
   void analyze_range_safety();

private:
   ExtendedPlanetaryDerivedState (const ExtendedPlanetaryDerivedState & rhs);
   ExtendedPlanetaryDerivedState & operator = (const ExtendedPlanetaryDerivedState & rhs);

};


#endif
