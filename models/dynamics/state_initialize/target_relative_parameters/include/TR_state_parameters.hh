/*******************************************************************************
PURPOSE:
   (Provide the capability to transform between position/velocity
    vectors and a set of target relative parameters.)

LIBRARY DEPENDENCIES:
   (
    (../src/TR_state_parameters.cc)
   )

PROGRAMMERS:
   (
    ((Jeremy Rea) (NASA) (May 2017) (Initial implementation))
   )

NOTES:
  ((This implementation was the first of the two implementations so it gets the
  "primary" naming. It relies on setting the target point by alt/lat/lon
  specification whereas the alternative supports setting the target point by
  a 3-vector ... and does not require a planet-fixed position implementation
  to do the conversions.))
*******************************************************************************/
#ifndef CML_TARGET_RELATIVE_PARAMETERS_STATE_HH
#define CML_TARGET_RELATIVE_PARAMETERS_STATE_HH

#include "jeod/models/environment/planet/include/planet.hh"
#include "jeod/models/utils/planet_fixed/planet_fixed_posn/include/planet_fixed_posn.hh"

#include "TR_state_param.hh"

/*****************************************************************************
TargetRelative_StateParameter
Purpose:
  Main class handling the conversion between position and velocity
  3-vectors and target-relative state parameters.
*****************************************************************************/
class TargetRelative_StateParameter  : public TargetRelative_StateParam
{
  // Make CorrelatedStateDispersion a friend because it has to have accesss to
  // the pfix_position instance.  pfix_position has to be private to avoid the
  // problem of trying to auto-build a logging capability for the
  // self-referential pointers found at
  // pfix_position.planet[0].grav_body[0].planet[0].grav_body[0]. ...
 friend class CorrelatedStateDispersion;
 public:
  /* Input parameters */
  double  target_point_altitude;   /* (m)   Target geodetic altitude      */
  double  target_point_latitude;   /* (rad)   Target geodetic latitude      */
  double  target_point_longitude;  /* (rad)   Target longitude              */

  double  tolerance;  /* (m)
    Convergence tolerance for geodetic altitude iteration */
  int     iter_limit; /* (--)
    Maximum iteration limit for geodetic altitude iteration */

 protected:
  jeod::PlanetFixedPosition  pfix_position;  /* (--)
    Planet-fixed position class, this is used to provide a convenient
    conversion from alt-lat-lon to Cartesian. */


 public:
  /* Constructor */
  TargetRelative_StateParameter();

  /* Destructor */
  virtual ~TargetRelative_StateParameter(){};

  /* Public Methods */
  void initialize( double ref_geodetic_altitude,
                   double ref_geodetic_latitude,
                   double ref_longitude);
  void initialize( const double R_Ref_PCPF[3] ) override;
  void set_planet( const jeod::Planet * planet_IN);

  void compute_params_from_vectors();
  void compute_params_from_vectors(
                 const double   (&R_PCPF)[3],
                 const double   (&V_wrt_PCI_PCPF)[3],
                 TR_ParameterSet & param_set);
  void compute_params_from_vectors_SWIG(
                 double R_PCPF[3],
                 double V_wrt_PCI_PCPF[3],
                 TR_ParameterSet & param_set);


  void compute_position_from_params( InputPosAngle input_theta_type);
  void compute_position_from_params( double        geodetic_altitude,
                                     double        theta_Rng,
                                     double        theta_Rot_or_Cross,
                                     InputPosAngle input_theta_type,
                                     double        (&R_PCPF)[3]);
  void compute_position_from_params_SWIG( double        geodetic_altitude,
                                          double        theta_Rng,
                                          double        theta_Rot_or_Cross,
                                          InputPosAngle input_theta_type,
                                          double        R_PCPF[3]);


 private:
  /* Private methods */
  bool initialize_check_config();
  void initialize_internal(const double R_Ref_PCPF[3]);
  double compute_position_magnitude();

  // copy-constructor and operator= made empty to prevent misuse.
  TargetRelative_StateParameter (const TargetRelative_StateParameter&);
  TargetRelative_StateParameter & operator = (
                                 const TargetRelative_StateParameter&);
};
#endif
