/********************************* TRICK HEADER *******************************
PURPOSE:
   (Class that manages generation of a vehicle state relative to the
    atmosphere through which it is moving.)

REFERENCES:
    (((SES II) (atmos_relative_state function) (position_att.c))
     ((SES) (Parameters of Motion function) (POM.f))
     ((SORT) (Where At function) (WHEREAT.f)))

LIBRARY DEPENDENCY:
    ((../src/atmos_relative_state.cc))

PROGRAMMERS:
    (((Gavin Mendeck) (NASA) (May 2005) (ARES) (Initial implementation as pom))
     ((Edgar Medina) (NASA) (June 2005) (ARES)
         (Changed name and updated function))
     ((Jeremy Rea) (NASA) (August 2005) (ARES)
         (Added total angle of attack computation))
     ((Robert Gay) (NASA-JSC) (Jan 2006) (ARES)
         (Added general atmos params for data collection))
     ((Jeremy Rea) (NASA) (01/08) (ANTARES)
         (RDLaa10259: Add Reynolds number computation))
     ((Debbie Merritt) (NASA) (07/08) (ANTARES)
         (add qalpha, qbeta, qalpha_total computation))
     ((Carl Merry) (USA) (08/08) (ANTARES) (Added aero load computations)))
     ((Gary Turner) (OSR) (Feb 15) (ANTARES)
         (Conversion to C++ and interface to planet-rel-state.)))
****************************************************************************** */

#ifndef ANTARES_ATMOS_RELATIVE_STATE_HH
#define ANTARES_ATMOS_RELATIVE_STATE_HH

#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/dynamics/state_descriptors/extended_planetary_derived_state/include/extended_planetary_derived_state.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/environment/atmos/atmos_exec/include/atmosphere_exec_interface.hh"
#include "cml/models/environment/atmos/atmos_exec/include/atmosphere_exec_out.hh"


class AtmosRelativeState : public SubscriptionBase
{
 public:
  bool warn_on_v_parallel_r; /* (--)
        Indicates whether to warn on condition where velocity and radial
        vectors are parallel.*/
  bool terminate_on_nan_euler; /* (--)
        Determines the recovery action following invalid computation
        of euler angles.
        True: terminate.  False: set them to 0 and continue.
        Default: false.*/

  // Velocity vectors and magnitudes
  double free_stream_vel_mag;    /* (m/s) Magnitude of free-stream velocity */
  double free_stream_vel[3];     /* (m/s) Free-stream velocity (inertial) */
  double free_stream_vel_unit[3];/* (--)  unit-vector of free_stream_vel */
  double free_stream_td_vel[3];  /* (m/s) Free-stream topodetic velocity
                                          vector */
  double free_stream_body_vel[3];/* (m/s) Free-stream velocity in body frame */

  // Velocity angles and aerodynamic angles
  double free_stream_flight_path;/* (rad) Free stream flight path angle */
  double free_stream_azimuth;    /* (rad) Free stream azimuth */
  double angle_of_attack;        /* (rad) Angle of attack */
  double angle_of_sideslip;      /* (rad) Angle of sideslip */
  double bank_angle;             /* (rad) Bank angle (aerodynamic roll) */
  double total_angle_of_attack;  /* (rad) Total angle of attack */
  double phi_roll;               /* (rad) Angle from Z-body to wind vel in
                                        body YZ */

  // Other aerodynamic parameters
  double mach_number;      /*   (--) Mach number */
  double dynamic_pressure; /* (N/m2) Dynamic pressure */
  double drag_accel;       /* (m/s2) Drag acceleration */
  double side_accel;       /* (m/s2) Side acceleration */
  double lift_accel;       /* (m/s2) Lift acceleration */
  double sensed_accel_mag; /* (m/s2) Magnitude of the sensed acceleration */

  // Alternate computations
  double angle_of_attack_alt;   /* (rad) Alternate angle of attack computation */
  double angle_of_sideslip_alt; /* (rad) Alternate sideslip angle computation */
  double total_angle_of_attack_alt; /* (rad) Alternate total angle of attack
                                           computation */
  double phi_roll_alt;          /* (rad) Angle from Z-body to wind vel in
                                       body YZ */

  // Transformation matrices
  double T_inrtl_traj[3][3]; /* (--) Inertial-to-trajectory transformation */
  double T_traj_body[3][3];  /* (--) Trajectory-to-body transformation */
  double T_traj_wind[3][3];  /* (--) Trajectory-to-wind transformation */
  double T_inrtl_wind[3][3]; /* (--) Inertial-to-wind transformation */
  double T_wind_stab[3][3];  /* (--) Wind-to-stability transformation */
  double T_inrtl_stab[3][3]; /* (--) Inertial-to-stability transformation */

  // General Atmospheric Params
  double reynolds_number;    /* (--) Reynolds number */

  // Atmospheric load terms
  double qalpha_total;        /* (N*rad/m2)  Aerodynamic load due to
                                           total angle of attack */
  double qalpha_total_psfdeg; /* (lbf*degree/ft2) Aerodynamic load due to
                                             total angle of attack in
                                             (lb-force * degree) / (ft^2) */
  double qalpha;              /* (N*rad/m2) Aerodynamic load due to
                                          angle of attack */
  double qbeta;               /* (N*rad/m2) Aerodynamic load due to
                                          angle of sideslip */

 protected:

  // External model access:
  const jeod::DynBody       & body; /* (--) The body in the atmosphere */
  ExtendedPlanetaryDerivedState & planet_rel_state; /* (--)
        The state relative to the planet frame */
  AtmosphereExecInterface       & atmos_exec; /* (--) for subscription mgmt. */
  const AtmosExecOutput         & atmos;/* (--) atmospheric parameters. */
  const double * reference_length; /* (m)
        Pointer to externally-defined reference length.
        Used only for full-calculation. */

  // Internal variables.
  unsigned int compute_angles_subscriptions; /* (--)
        Count of the number of external models needing the computation of the
        flight-path-body angles. */
  unsigned int complete_calculation_subscriptions; /* (--)
      Count of the number of external models needing the complete calculation */
  bool ref_length_null_msg_sent; /* (--) Has error message been sent. */
  double traj_to_body_RYP[3]; /* (rad)
        The Roll-Yaw-Pitch euler angles associated with the
        trajectory-to-body frame transformation.*/
  const double psfdeg_converter; /* (--)
        A unit converter to convert from SI units to traditional units.
        This is a dimensionless quantity with a value of 1, or equivalently,
        1.19664723485755 ((deg/rad) ((lbf/ft^2)/(N/m^2))) */


 public:
  void set_reference_length( double * ref_length_ptr);
  void update();

  void subscribe_complete_calculation();
  void unsubscribe_complete_calculation();
  void subscribe_angles_calculation();
  void unsubscribe_angles_calculation();
  unsigned int get_angles_subscription_count() {
    return compute_angles_subscriptions;
  }
  void compute_T_inrtl_traj();

  AtmosRelativeState( const jeod::DynBody       & body_ref,
                      ExtendedPlanetaryDerivedState & prs_ref,
                      AtmosphereExecInterface       & atmos_exec_ref);
  virtual ~AtmosRelativeState(){};

 protected:
  virtual void activate();
  virtual void deactivate();

  void update_zero_vel();
  void compute_angles();
  void compute_fpangle_azimuth();
  void compute_wind_stability_transforms();
  void compute_accel_wind();
  void compute_alternate_angles();
  void compute_dynamic_pressure_extras();
  void compute_reynolds_number();
  void check_euler_for_nan();
  // This method is virtual purely to allow testing of check_euler_for_nan()
  virtual void normalize_T_traj_body();

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies
  AtmosRelativeState (const AtmosRelativeState &);
  AtmosRelativeState & operator= (const AtmosRelativeState &);
};


#endif
