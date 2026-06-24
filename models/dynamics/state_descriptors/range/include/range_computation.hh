/********************************* TRICK HEADER *******************************
Purpose:
   (Defines the class for the computation of range and the simple data-only
   class containing the values that are thus computed.)

Library Dependency:
   (( ../src/range_computation.cc))

Programmers:
   (((Gary Turner) (OSR) (Sept 2014 (Initial version))
    ((Bingquan Wang) (OSR) (May 2017)
        (Moved acos_protect/asin_protect to math_utils of cml))
    ((Dan Jordan)  (Jacobs/JETS) (June 2017) (EM1 V&V cleanup))
   )

*******************************************************************************/

#ifndef CML_RANGE_COMPUTATION_VALUES
#define CML_RANGE_COMPUTATION_VALUES

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "pfix_reference_point.hh"

namespace jeod {
class RefFrameTrans;
class Planet;
}

/*****************************************************************************
RangeValues
Purpose:(Provides the collective range and range-angle variables in one place)
*****************************************************************************/
class RangeValues
{
 public:
  // Range angles
  double totalrange_angle;      /* (rad) Angle for total range */
  double crossrange_angle;      /* (rad) Angle for cross-track range component */
  double downrange_angle;       /* (rad) Angle for down-range component */

  // Range arc-lengths, using the average of the reference and current radii
  double cross_range_avg_rad;   /* (m) Cross-track range arc-length
                                       using average radius */
  double down_range_avg_rad;    /* (m) Down-range arc-length using
                                       average radius */
  double total_range_avg_rad;   /* (m) Total range arc-length using
                                       average radius */

  // Range arc-lengths, using the reference radius
  double cross_range_ref_rad;   /* (m) Cross-track range arc-length using
                                       reference radius */
  double down_range_ref_rad;    /* (m) Down-range arc-length using
                                       reference radius */
  double total_range_ref_rad;   /* (m) Total range arc-length using
                                       reference radius */

 public:
  RangeValues();
  virtual ~RangeValues(){};
 private:
   // Copy constructor and assignment operator for this class are
   // declared private and are not implemented.
  RangeValues(const RangeValues& rhs);
  RangeValues & operator = (const RangeValues&);
};


/*****************************************************************************
RangeComputation
Purpose:(Provides an abstract basis for computing the range from an "origin"
         to a "target".
         A reference point and the current vehicle position are used; whether
         the reference point is the origin or the target is determined in the
         derived classes RangeFromPfixReference and RangeToPfixReference.)
*****************************************************************************/
class RangeComputation : public RangeValues,
                         public SubscriptionBase
{
 public:
  PfixReferencePoint reference_data; /* (--) reference data */

 protected:
  double reference_radius;  /* (m)  Radius to the point defined by
                                    reference_data */
  double current_radius;    /* (m)  Radius to the current point of interest
                                    (state member variable) */
  double average_radius;    /* (m)  Average of the two radii */

  bool ref_point_set;       /* (--) Reference point values are set*/
  bool use_polar_direction; /* (--)
       Use lat-lon-azimuth formulation, (rather than Cartesian position
       formulation).  Default: false (use Cartesian) */
  bool set_direction;       /* (--)
       Flag indicates whether the set_reference_data methods are responsible
       for setting the direction.  This value is assigned at construction, a
       function of the choice of constructor.  Default: false -- do not set.*/

  double ref_sin_lat;     /* (--) Sine of the reference latitude*/
  double ref_cos_lat;     /* (--) Cosine of the reference latitude*/
  double origin_sin_lat;  /* (--) Sine of the origin latitude*/
  double origin_cos_lat;  /* (--) Cosine of the origin latitude*/
  double target_sin_lat;  /* (--) Sine of the target latitude*/
  double target_cos_lat;  /* (--) Cosine of the target latitude*/

  double origin_sin_azimuth; /* (--) Sine of the origin azimuth angle*/
  double origin_cos_azimuth; /* (--) Cosine of the origin azimuth angle*/

  double origin_longitude; /* (rad) Longitude of the origin point*/
  double target_longitude; /* (rad) Longitude of the target point*/

  double reference_position_unit_pfix[3]; /* (--)
      The unit vector in the pfix frame to the reference position. */
  double origin_position_unit_pfix[3]; /* (--)
      The unit vector in the pfix frame to the origin position. */
  double target_position_unit_pfix[3]; /* (--)
      The unit vector in the pfix frame to the target position. */

  double origin_direction[3]; /* (--)
      The 3-vector specifying the reference direction.
      This does not have to be a unit vector.*/

  double cos_totalrange_angle; /* (--) Cosine of the total-range angle*/
  double pos_x_pos_unit_pfix[3]; /* (--)
      Cross product of the target-position unit vector with the
      origin-position unit vector. */
  double dir_x_pos_unit_pfix[3]; /* (--)
      Cross product of the reference-direction unit vector with the
      origin-position unit vector. */

  const jeod::PlanetFixedPosition & state; /* (--)
      Reference to the PlanetFixedPosition that will be used for
      generating the range data. This is typically the vehicle state.*/

 public:
  RangeComputation(const jeod::PlanetFixedPosition & state_in);
  virtual ~RangeComputation(){};
  using SubscriptionBase::initialize;
  virtual void initialize( const jeod::Planet & planet_in);
  void set_reference_data_query(); /*
      external call to determine whether there is sufficient data to
      proceed with setting the reference data.*/
  virtual void set_reference_data();
  void set_reference_data_from_inrtl_state( const jeod::RefFrameTrans & trans_state);
  void set_reference_data_from_inrtl_state( const double position[3],
                                            const double direction[3]);
  void set_reference_data_from_state( const double position[3],
                                      const double direction[3]);
  void update();

 protected:
  virtual void update_using_cartesian( void );
  virtual void update_using_polar( void);
  virtual void is_abstract() = 0; // Makes this class non-instantiable

 private:
   // Copy constructor and assignment operator for this class are
   // declared private and are not implemented.
  RangeComputation(const RangeComputation& rhs);
  RangeComputation & operator = (const RangeComputation&);
};

#endif
