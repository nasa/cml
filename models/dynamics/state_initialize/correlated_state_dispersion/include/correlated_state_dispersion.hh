/********************************* TRICK HEADER *******************************
PURPOSE: (
   Disperses the state using a covariance matrix.
   Supports dispersion of:
      - position-velocity (default), or
      - position-velocity-attitude.
   Covariance matrix may be specified using any of the following coordinate
   systems:
      - inertial (default)
      - LVLH
      - UVW
      - radius--right-ascension--declination--speed--flight-path-angle--azimuth
      - relative LVLH
      - Target-relative parameters)

LIBRARY DEPENDENCY:
    ((../src/correlated_state_dispersion.cc))

PROGRAMMERS:
    (((Lanoix) (OSR) (04/04) (Initial Implementation))
     ((Robert Gay) (Odyssey) (05/04) (removed first call))
     ((Robert Gay) (NASA-JSC) (Sept 2006) (CEV) (RDLaa09121 - added
      pv_cov input option))
     ((Robert Gay) (NASA-JSC) (May 2007) (CEV) (RDLaa09805-add sig num mag))
     ((Scott Jenkins) (Draper) (Apr 2008) (Orion) (added att_err and corr_option))
     ((Scott Jenkins) (Draper) (June 2008) (Orion) (added corr_att_frame flag))
     ((Scott Jenkins) (Draper) (Jan 2009) (Orion) (added Att_Rotation_T flag))
     ((Gary Turner) (OSR) (Feb 2015) (Antares) (Conversion to C++))
     ((Jeremy Rea) (NASA_JSC) (May 2017) (Antares)
                                       (Add target relative parameter option))
     ((Brenton Caughron) (OSR) (Oct. 2017) (Antares) (IV&V Code Review))
     ((Daniel Ghan) (OSR) (Oct 2019) (Antares) (Switched to C++11 random number
               generation, added more dispersion options, fixed bugs, and made
               dispersion model more statistically robust.)))
******************************************************************************/

#ifndef ANTARES_CORRELATED_STATE_DISPERSIONS_HH
#define ANTARES_CORRELATED_STATE_DISPERSIONS_HH

#include <random> // default_random_engine,
                  // uniform_real_distribution,
                  // normal_distribution

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/orientation/include/orientation.hh" // Orientation::*
#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils::*

#include "cml/models/dynamics/state_initialize/target_relative_parameters/include/TR_state_parameters.hh" // TargetRelative_StateParameter


class CorrelatedStateDispersion {
 public:

  enum CovarianceFrame{
    PV_COVAR_INRTL            = 0, /* Covariance is in Inertial   */
    PV_COVAR_LVLH             = 1, /* Covariance is in LVLH (abs) */
    PV_COVAR_UVW              = 2, /* Covariance is in UVW  (abs) */
    PV_COVAR_R_DEC_RA_V_FP_AZ = 3, /* Covar is r, dec, right asc,
                                      v, fp angle, azimuth       */
    PV_COVAR_REL_LVLH         = 4, /* Covariance is in LVLH (rel) */
    PV_COVAR_TR_PARAM         = 5  /* Covariance is in Target
                                      Relative parameters         */
  };

  enum CorrelationOption{
    CORRELATED_PV       = 0, /* Correlated pos and vel covariance 6x6  */
    CORRELATED_PV_ATT   = 1  /* Correlated pos, vel, and att cov 9x9   */
  };

  //  The rotational state is built as a rotation by some amount relative to a
  //  reference frame.  The dispersion could be applied to the reference frame
  //  prior to the rotation, or applied to the body frame after the rotation.
  enum CorrelationFrameForAttitude{
    REFERENCE_FRM       = 0, /* Correlated att cov in inertial frame */
    BODY_FRM            = 1  /* Correlated att cov in body frame     */
  };

  //  This enumeration provides the information of how the attitude error
  //  should be interpreted.
  enum AttitudeRotation{
    TrueToPerturbed  = 0, /*
       Use for state-estimation only:
       Rotation from true to a perturbed frame
       REFERENCE_FRM: The dispersions provide the rotation from the true
                      reference frame to the perturbed reference frame.
                      The estimated state will be from the perturbed
                      reference frame to the true body frame.
       BODY_FRM:      The dispersions provide the rotation from the truth
                      body attitude to the perturbed body attitude.
                      The estimated state will be from the true
                      reference frame to the perturbed body frame.  */
    NominalToTrue  = 0, /*
       Use for state-initialization only:
       Rotation from the nominal specification to resulting true attitude
       REFERENCE_FRM: The dispersions provide the rotation from the nominal
                      reference frame to the true reference frame.
                      The specified attitude will provide the attitude of the
                      body frame relative to the nominal reference frame.  It
                      will be combined with this dispersion
                      to yield the attitude of the body frame relative to the
                      true reference frame.
       BODY_FRM:      The dispersions provide the rotation from the nominal
                      body attitude to the true body attitude.
                      The specified attitude will be applied to yield the
                      nominal body attitude; this dispersion will be added to
                      that to yield the true body attitude.*/
    PerturbedToTrue       = 1, /*
       Use for state-estimation only:
       Rotation from Pertubed to true Frame.
       REFERENCE_FRM: the dispersions provide the rotation from the perturbed
                      reference frame to the true reference frame.
                      The estimated state will be from the perturbed
                      reference frame to the true body frame.
       BODY_FRM:      the dispersions provide the rotation from the perturbed
                      body attitude to the real body attitude.
                      The estimated state will be from the true
                      reference frame to the perturbed body frame.  */
    TrueToNominal  = 1  /*
       Use for state-initialization only:
       Rotation from the nominal specification to resulting true attitude
       REFERENCE_FRM: The dispersions provide the rotation from the true
                      reference frame to the nominal reference frame.
                      The specified attitude will provide the attitude of the
                      body frame relative to the nominal reference frame.  It
                      will be combined with this dispersion
                      to yield the attitude of the body frame relative to the
                      true reference frame.
       BODY_FRM:      The dispersions provide the rotation from the true
                      body attitude to the nominal body attitude.
                      The specified attitude will be applied to yield the
                      nominal body attitude; this dispersion will be subtracted
                      to that to yield the true body attitude.*/
  };

  enum DispersionDistribution {
    // NOTES
    // - in the descriptions for all these options, the term
    //   "standard-deviations" refers to the values extracted from the
    //   square-root of the covariance matrix.
    // - while the descriptions allude to random dispersions, the random
    //   number generator will only generate random values if it seeded
    //   randomly.  To avoid repetitive data in a MonteCarlo environment,
    //   be sure to either:
    //   - disperse the "seed" variable (options 1-4), or
    //   - disperse the values in user_specified_distribution (option 5)
    NoDispersion      = 0,/* No dispersions applied */
    Constant          = 1,/* Dispersions are all computed as the product of the
                             distribution's standard deviations and the
                             variable "sigma_limit"; supports, for example:
                             "disperse value to be exactly +1.5 sigma away
                              from nominal" */
    Uniform           = 2,/* State is randomly dispersed according to a uniform
                             distribution within bounds computed as the product
                             of the distribution's standard deviation and the
                             the values +- sigma_limit.  Supports uniform
                             coverage of a dispersion across a range, such
                             as +- 3-sigma.
                             For multi-dimensional considerations, this option
                             provides a value uniformly distributed across the
                             entire hyper-volume.
                             The dispersion bounds are the same as those for
                             the TruncatedGaussian option, but the distribution
                             within those bounds is different.
                             Correlations are preserved. */
    Gaussian          = 3,/* State is randomly dispersed according to the
                             multivariate Gaussian distribution.
                             The mean of this dispersion is zero and the
                             standard deviation equal to the standard deviation
                             of each variable; a distribution with mean=0 and
                             std-dev=1 is used to generate a raw value that is
                             later scaled by the standard deviation (in the case
                             of correlated variables by the decomposed covariance
                             matrix) for each variable.
                             This distribution is unbounded.*/
    Normal            = 3,/* Alternative specification for Gaussian */
    TruncatedGaussian = 4,/* Based off a Gaussian (Normal) distribution
                             but with the distribution-tails removed.
                             The dispersion values are bounded to the same
                             interval as those described for the Uniform
                             distribution. */
    TruncatedNormal   = 4,/* Alternative specification for TruncatedGaussian */
    UserInput         = 5 /* Dispersions are the product of the distribution's
                             standard deviations and the values specified in
                             the user_specified_distribution array.
                             Supports use-cases where the desired distribution
                             is not covered by the options available.  In this
                             case, the user_specified_distribution values can be
                             generated according to any statistical
                             distribution supported externally, e.g. by a
                             MonteCarlo engine.*/
  };

  const static size_t max_dimension = 9; /* (--)
      The maximum number of variables to be correlated in this model.
      Drives array sizing.*/


  // Input
  CovarianceFrame    pv_covar_frame;  /* (--)
      Frame type for pv covar.*/
  CorrelationOption  corr_option;     /* (--)
      Correlated covariance type.*/
  CorrelationFrameForAttitude corr_att_frame;/* (--)
      Attitude covariance frame.*/
  AttitudeRotation   att_rot_defined; /* (--)
      Attitude rotation definition */
  DispersionDistribution dispersion_distribution; /* (--)
      Type of distribution. */

  bool hold_previous_random_vec; /* (--)
      Flag that manages whether a new random vector should be generated.  For
      cases where a change to the covariance matrix drives a change to the
      state, it may be desirable to keep the same "direction" to the dispersion
      and just use the new standard deviations. Default: false*/

  size_t max_iterations; /* (--)
      Provides protection against infinite loops for the truncation of the
      Gaussian distribution.  Depending on the dimensionality of the problem
      and how tightly the distribution is to be truncated, it may take many
      iteration to identify a distribution that fits within the limits.
      Default - 2000. */
  double sigma_limit; /* (--)
      Limit for Uniform and Truncated-Gaussian distributions.
      The dispersion will be bounded by +- sigma-limit standard deviations
      for all variables.*/
  unsigned int seed; /* (--) Seed for random number generator. */
  double user_specified_distribution[max_dimension]; /* (--)
      For distribution-option "UserInput", the values in this array specify the
      number of standard deviations to apply to each variable / dimension.
      Note that this does not specify the range in which the dispersion may
      fall, it specifies an exact multiple of standard deviations.
      E.g. dispersion position[0] by  0.123 sigma, and
                      position[1] by -0.234 sigma,*/
  double covariance[max_dimension][max_dimension]; /* (--)
      Covariance Matrix.*/
  std::string corr_base_frame_name; /* (--)
      The name of the reference frame against which covariance is referenced. */

  // Input / Output
  double TR_geodetic_altitude_disp;  /* (m)
      Geodetic altitude dispersion about nominal geodetic altitude at epoch
      for target relative dispersions */
  TargetRelative_StateParameter TR_param; /* (--)
      Instance of model responsible for  converting between position/velocity
      and the TargetRelative parameter set.  Included as a class instance
      to facilitate population of target location parameters, which are
      class members.*/

  // Output
  double pos_error[3];/* (m)
      position error produced from covariance in desired frame.
      This variable may also be used internally to represent the error in a
      (r-mag, declination,  right-ascension) format.
      In this case, the units are incorrect, but this usage is limited to internal
      application; before the code exits, these values will have been converted
      back to dimensions of length with units of meters.*/
  double vel_error[3];/* (m/s)
      velocity error produced from covariance in desired frame.
      This variable may also be used internally to represent the error in a
      (v-mag, azimuth, flight-path angle) format.
      In this case, the units are incorrect, but this usage is limited to internal
      application; before the code exits, these values will have been converted
      back to dimensions with units of meters/second. */
  double att_error[3];/* (rad)
      attitude error produced from covariance in desired frame.*/
  double prm_error[5];/* (--)
      Target-relative state-parameter errors produced from covariance matrix. */

  double r_mag;       /* (m)   radius vector magnitude             */
  double declination; /* (rad) declination of pos wrt inrtl        */
  double right_asc;   /* (rad) right ascension of pos wrt inrtl    */
  double v_mag;       /* (m/s) velocity vector magnitude           */
  double fp_angle;    /* (rad) flight-path-angle of vel wrt topocentric frame */
  double azimuth;     /* (rad) azimuth angle wrt to topocentric    */

 protected:
 bool generator_seeded; /* (--)
     Flag indicating whether the random number generator ahs been seeded.*/
  unsigned int dimension;       /* (--)
     The number of values being dispersed.  Options are:
     5: Uses the target-relative state-parameter dispersion capabilities
     6: disperses the position and velocity values (PV-dispersion)
     9: disperses the position, velocity, and attitude values (PVA-dispersion)*/
  double random_vec[max_dimension]; /* (--)
      Random values used to generate dispersions. This can be a user input
      (if dispersion_distribution = UserInput) */
  double sqrt_covariance[max_dimension][max_dimension]; /* (--)
      decomposition of Covariance Matrix */

  std::default_random_engine generator; /* (--) Random number generator */
  #ifndef SWIG // SWIG doesn't like templates and doesn't need to access these.
  std::uniform_real_distribution<double> rand_uniform; /* (--)
     Uniform distribution dispersion-generator */
  std::normal_distribution<double> rand_norm; /* (--)
     Normal distribution dispersion-generator */
  #endif

 public:
  CorrelatedStateDispersion();
  ~CorrelatedStateDispersion(){};

  void disperse_state( const jeod::RefFrameState & trans_base_wrt_cov_base,
                       const double (&rot_base_wrt_cov_base)[3][3],
                       double position[3],
                       double velocity[3],
                       double (&trans_mx)[3][3]);
  void disperse_state( const jeod::RefFrameState & state_base_wrt_cov_base,
                       double position[3],
                       double velocity[3],
                       double (&trans_mx)[3][3]);
  void disperse_state( double position[3],
                       double velocity[3],
                       double (&trans_mx)[3][3]);
  bool disperse_state( double position[3],
                       double velocity[3]);
  void disperse_state( double (&trans_mx)[3][3]);
  void apply_translational_dispersions( double position[3],
                                        double velocity[3]);
  void apply_attitude_dispersions( double (&trans_mx)[3][3]);

 protected:
  bool initialize(); /* returns a flag indicating whether the model
                        initialization ran correctly */
  void generate_random_vec();
  void populate_random_vec_norm();
  void populate_random_vec_unit();
  void populate_random_vec_uniform_sphere();
  void generate_dispersions();
  void transform_ra_dec_fp( const double position[3],
                            const double velocity[3]);
  void transform_TR_param(  const double position[3],
                            const double velocity[3]);

 private:
  CorrelatedStateDispersion (const CorrelatedStateDispersion&);
  CorrelatedStateDispersion & operator = (const CorrelatedStateDispersion&);
};
#endif
