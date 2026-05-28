/*******************************************************************************
PURPOSE:
   (Provide the capability to transform between position/velocity
    vectors and a set of target relative parameters.)

LIBRARY DEPENDENCIES:
   (
    (../src/TR_state_parameters_alt.cc)
   )

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (Nov 2023) (Additional implementation))
   )

NOTES:
  ((This implementation was the second of the two implementations so it gets the
  "alternative" naming. It relies on setting the target point by a 3-vector
  specification and as such does not require a planet-fixed position
  implementation to do the conversions. But then it requires specification
  of the planet rotation rate, which the other implementation extracts from its
  pfix-position instance.))
*******************************************************************************/
#ifndef CML_TARGET_RELATIVE_PARAMETERS_STATE_ALT_HH
#define CML_TARGET_RELATIVE_PARAMETERS_STATE_ALT_HH

#include "TR_state_param.hh"
/*****************************************************************************
TargetRelative_StateParameter_Alt
Purpose:
  Main class handling the conversion between position and velocity
  3-vectors and target-relative state parameters.
  This is an alternative implementation of TargetRelative_StateParameter_Alt
*****************************************************************************/
class TargetRelative_StateParameter_Alt : public TargetRelative_StateParam
{
 public:
  /* Input parameters */
  double target_point[3]; /* (m)
    Location of the target point, expressed in the desired frame. */
  double omega_planet[3]; /* (rad/s)
    Planetary rotation rate in the desired frame, expressed in the same
    desired frame. */

 protected:
  /* Internal variables                                          */
  double target_r_mag; /* (m)
    Magnitude of the target position vector.*/

 public:
  /* Constructor */
  TargetRelative_StateParameter_Alt();

  /* Destructor */
  virtual ~TargetRelative_StateParameter_Alt(){};

  /* Public Methods */
  void initialize( const double R_Ref[3] ) override;

  void compute_params_from_vectors();
  void compute_params_from_vectors(
                 const double   (&R)[3],
                 const double   (&V_wrt_PCI)[3],
                 TR_ParameterSet & param_set);

  void compute_position_from_params( InputPosAngle input_theta_type);
  void compute_position_from_params( double        relative_altitude,
                                     double        theta_Rng,
                                     double        theta_Rot_or_Cross,
                                     InputPosAngle input_theta_type,
                                     double        (&R)[3]);

 private:
  // copy-constructor and operator= made empty to prevent misuse.
  TargetRelative_StateParameter_Alt (const TargetRelative_StateParameter_Alt&);
  TargetRelative_StateParameter_Alt & operator = (
                                 const TargetRelative_StateParameter_Alt&);
};
#endif

