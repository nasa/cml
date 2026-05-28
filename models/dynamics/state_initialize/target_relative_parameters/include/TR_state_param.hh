/*******************************************************************************
PURPOSE:
   (Provide the capability to transform between position/velocity
    vectors and a set of target relative parameters.)

LIBRARY DEPENDENCIES:
   (
    (../src/TR_state_param.cc)
   )

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (Nov 2023)
      (Additional implementation led to a separation of the shared components
       into this baseline class))
   )

*******************************************************************************/
#ifndef CML_TARGET_RELATIVE_PARAMETERS_STATE_BASELINE_HH
#define CML_TARGET_RELATIVE_PARAMETERS_STATE_BASELINE_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "TR_parameter_ref_frame.hh"
#include "TR_state_parameter_set.hh"


/*****************************************************************************
TargetRelative_StateParam
Purpose:
  Shared components of the two TR-state-parameter classes.
*****************************************************************************/
class TargetRelative_StateParam : public TR_ParameterSet
{
 public:

    /* Enumeration for positon input angle relative to target */
  enum InputPosAngle {
    INPUT_THETA_ROT               = 0, /* 0 --  Rotation angle about target
                                                vector */
    INPUT_PHI_ABS_THETA_ROT_LT_90 = 1, /* 1 --  Cross angle (phi), resolved
                                          such that abs(theta_Rot) < 90 deg */
    INPUT_PHI_ABS_THETA_ROT_GT_90 = 2, /* 2 --  Cross angle (phi), resolved
                                          such that abs(theta_Rot) > 90 deg */
  };


  /* Input parameters */
  bool    target_is_behind_vehicle;/* (--)
    Flag to distinguish the ambiguity in the specification of the lateral
    angle.
    Default: false. */
  double  K_theta;    /* (s/rad) Time-of-flight constant       */
  double position[3]; /* (m)
    The position associated with the TR-parameters. This may be set
    externally and used to compute parameters, or the parameters set
    externally and used to compute this vector.*/
  double velocity[3]; /* (m/s)
    The position associated with the TR-parameters. This may be set
    externally and used to compute parameters, or the parameters set
    externally and used to compute this vector.*/

 protected:
  /* Internal variables                                          */
  double  omega_mag;  /* (rad/s)
    Magnitude of planet rotation vector w.r.t. inertial frame        */
  double uhat_R[3];   /* (--) Unit position vector  */

  // TODO @bcaughro: Temporarily going to convert all MathUtils::is_near_equal()
  // calls against zero to compare instead to less than this epsilon
  const double epsilon = 1e-12; /* (--)
    Arbitrary epsilon to compare values against zero */

  TR_Parameter_RefFrame Target_Reference_frame;   /* (--)
    Basis vectors for working coordinate frame
    u-hat = Unit position vector of target point wrt pfix frame.
    n-hat = Unit normal to plane defined by target and reference points
           (i.e. Reference Target Plane)
    p-hat = Unit normal to target vector that lies in Reference Target Plane*/
  TR_Parameter_RefFrame Omega_Target_frame; /* (--)
    Basis vectors for working coordinate frame
    u-hat = Unit planet rotation vector w.r.t. inertial frame (planet rotation
           axis)
    n-hat = Unit normal to plane formed by rotation axis and target position
    p-hat = Unit normal to rotation axis that lies in plane formed by rotation
           axis and target position */
  TR_Parameter_RefFrame Position_BiasedTarget_frame; /* (--)
    Basis vectors for working coordinate frame
    u-hat = Unit position vector of initial position
    n-hat = Unit normal to plane formed by initial position and target
            postiion vectors
    p-hat = Unit normal to initial position vector lies in the plane toward
            the target position */
  double biased_target_pos_hat[3]; /* (--)
    Unit vector in the PCPF frame repesenting the biased-target position
    vector.*/
  double theta_RngBias; /* (rad)
    The angle between the initial position vector and the biased-target
    position vector. */

  double  sin_phi_omega;       /* (--)  Sine of angle between planet rotation
                                        axis and target position            */
  double  cos_phi_omega;       /* (--)  Cosine of angle between planet
                                        rotation axis and target position    */
  bool    flag_initialized;    /* (--)
      Model is initialized, planet is non-NULL and all internally generated
      vectors are computed. */
  bool omega_target_aligned;   /* (--)
      flag used to indicate when the target vector lies in proximity with
      the omega vector.*/
  bool k_theta_warning_sent; /* (--)
      flag controlling the broadcast of the warning about k_theta not
      having been set.  This makes the message send only once.
      Default (false). */
  bool target_is_behind_vehicle_check; /* (--) debugging tool.*/

 public:
  /* Constructor */
  TargetRelative_StateParam();

  /* Destructor */
  virtual ~TargetRelative_StateParam(){};

  /* Public Methods, establishing interfaces defined in the derived classes */
  virtual void initialize( const double R_Ref[3]) = 0;

  /* Public Methods, used by both derived classes */
  void compute_pos_vel_from_params(
                              InputPosAngle input_theta_type = INPUT_THETA_ROT);
  void compute_pos_vel_from_params( const TR_ParameterSet & param_set,
                                    InputPosAngle        input_theta_type,
                                    double               (&R)[3], // out
                                    double               (&V_wrt_PCI)[3]);

  void compute_velocity_from_params();
  void compute_velocity_from_params( double        Vmag,
                                     double        gamma,
                                     double        Lambda,
                                     const double  (&R)[3], // in
                                     double        (&V_wrt_PCI)[3]); // out
  void compute_velocity_from_params_SWIG( double        Vmag,
                                          double        gamma,
                                          double        Lambda,
                                          const double  R_PCPF[3], // in
                                          double        V_wrt_PCI_PCPF[3]); // out
  double * get_tgt_position() {return Target_Reference_frame.u_hat;}

  /* Protected methods, called internally from the out-facing methods in
   * the derived classes.*/
 protected:
  bool compute_params_from_vectors();
  bool compute_position_from_params(
                              InputPosAngle input_theta_type = INPUT_THETA_ROT);

  bool compute_biased_target();

 private:
  // copy-constructor and operator= made empty to prevent misuse.
  TargetRelative_StateParam ( const TargetRelative_StateParam&);
  TargetRelative_StateParam & operator = ( const TargetRelative_StateParam&);
};
#endif

