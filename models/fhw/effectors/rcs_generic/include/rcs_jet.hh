/*******************************TRICK HEADER******************************
PURPOSE:
  (Simple model of a single reaction control system jet.)

LIBRARY DEPENDENCY:
  ((../src/rcs_jet.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (April 2017) (Antares)
       (initial object-oriented implementation)))
**********************************************************************/

#ifndef RCS_GENERIC_JET_HH
#define RCS_GENERIC_JET_HH

#include <list>
#include <vector>

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "jeod/models/utils/quaternion/include/quat.hh"

#include "rcs_prop_pod.hh"
#include "rcs_group.hh"
#include "rcs_generic.hh"

/*****************************************************************************
RcsJet
Purpose:(The individual jets)
*****************************************************************************/
class RcsJet {
 protected:
  RcsGeneric & system;   /* (--) The overall Rcs system reference. */
  RcsPropPod & prop_pod; /* (--) Pod that this jet belongs to */
  RcsJetGroup & group;   /* (--) Model that this jet belongs to. */
  // sub-references:  this is extracted from system, but used so much that it
  // is given its own reference:
  const double & time_step; /* (s) reference to RcsGeneric time_step. */

  double isp;   /* (s) Specific impulse. Set only if blow_down NOT set */
  double isp_g; /* (m/s) isp * g at earth surface. */
  const double g_at_earth_surface; /* (m/s2) g at earth surface. */

  std::vector<double> component_flow_rate; /* (kg/s)
       Propellant flow rates for each propellant component assuming only one
       jet from each prop module.
       Used if RcsJetGroup.propc_use_isp = false
       Generated automatically at initialization if
         RcsGeneric::calc_flow_rate = true AND RcsGeneric::num_prop_comp = 2
       Set manually via set_component_flow_rate() otherwise */
  std::vector<double> component_consumption;/* (kg)
       Work space for prop consumption during delta_time_on maximum for each
       propellant component */

  double force_hat[3]; /* (--)
       Unit-vector force direction. */
  double T_str_to_case[3][3]; /* (--)
       transformation matrix from structural frame to a frame oriented such
       that the nominal force is oriented along the x-axis.
       y-axis and z-axis are ambiguous but not important.*/
  bool force_hat_changed; /* (--)
      set to true when force_hat is changed externally, used by
      generate_force_direction_matrix.*/

  double force_cl_with_err;  /* (N)
       force_cl + force_cl_err. */
  double force_hat_with_err[3]; /* (--)
       A direction unit-vector including the directional errors.*/

  double cone_angle_err; /* (rad)
       Angle of rotation of dispersed force_hat away from force_hat */
  double azimuth_angle_err; /* (rad)
       Angle of rotation of dispersed force_hat in plane perpendiculer to
       force_hat */

 public:
  /****** Inputs AND Outputs ******/
  double force[3];     /* (N)
      Force applied during a time step. It is also an Input during
      initialization if input_force=2) */

  /****** Inputs ******/
  enum RCSJetError{
    No_Errors = 0, // disregard all random errors
    Input_Errors,  // Errors are user-input
    Calc_Fire,     // calculate errors when a jet begins a new fire
    Calc_Always    // calculate errors whenever the jet is on
  };
  RCSJetError error;    /* (--) switch for random jet error implementation */

  double location[3]; /* (m)  position vector of the jet in structural frame */
  double force_cl;    /* (N)
       Force along the center line of the jet
       Set his value as an input only if:
         ((RcsGeneric::input_force == RcsGeneric::mag_and_uvec) AND
          (RcsJetGroup::blow_down  == false)). */

  double force_mag_std_dev; /* (--)
       Force magnitude error standard deviation as a fraction of the
       nominal center-line thrust.
       Used only if: (error != No_Errors)  */
  double force_mag_bias_frac; /* (--)
       Force magnitude error bias as a fraction of nominal center-line-thrust
       Total force error = force_cl *
                             (force_bias_frac + random * force_std_dev_frac)
       Used only if: (error != No_Errors)  */

  enum RcsDirectionError {
    Vector = 0,
    Angle = 1
  };
  RcsDirectionError direction_error; /* (--)
        switch for determining which method to use for dispersing directional
        aspect of force error. */

  double force_cl_err; /* (N)
       Error in force magnitude along the jet centerline.
       Input only in case of error = Input_Errors. */

  double force_hat_err[3]; /* (--)
       Error in force direction. Will be calculated if random noise added,
       will be an input otherwise.
       No need to be set if:
           (error != No_Errors) AND
           ((force_hat_std_dev != [0,0,0]) OR (force_axial_std_dev > 0) */
  double force_hat_std_dev[3];/* (--)
       Force direction error standard deviation as a fraction of the unit
       direction vector
       Set only if: (error != No_Errors)  */
  double force_hat_std_mean[3];/* (m)
       Force direction error standard mean
       Set only if: (error != No_Errors)  */

  bool direction_dispersion;  /* (--)
       Flag indicating whether the nominal force_hat vector should have a
       dispersion applied to it.  Defaults to false.*/
  double cone_angle_disp; /* (rad)
       Angle of rotation of dispersed force_hat away from force_hat.
       Used to represent a relatively constant dispersion rather than
       a random error.  For errors, use cone_angle_std and cone_angle_bias.*/
  double azimuth_angle_disp; /* (rad)
       Angle of rotation of dispersed force_hat in plane perpendicular to
       force_hat.  Used to represent a relatively constant dispersion rather
       than a random error.  For random errors, azimuth angle is automatically
       generated as a random value between 0 and pi.*/

  double cone_angle_bias; /* (rad)
       Fixed component used in computing cone_angle_err. */
  double cone_angle_std_dev; /* (rad)
       Standard deviation of random distribution used in computing
       cone_angle_err. */
  double base_impingement_force[3];  /* (N)
       Basis for computation of self-impingement force, structural
       frame referenced.  Scaled to provide scaled_impingement_force. */
  double base_impingement_torque[3];  /* (N*m)
       Basis for computation of self-impingement torque, about impingement
       reference centeri, referenced to the structural frame.  Scaled to
       provide scaled_impingement_torque. */
  enum RcsJetFailure {
    No_Failure = -1, // jet fires normally
    Failed_Off = 0,  // jet never fires
    Failed_On  = 1   // jet continuously fires
  };
  RcsJetFailure failure; /* (--) Monitoring jet failures.*/
  double thrust_factor;  /* (--)
       Per jet thrust factor (used if bool apply_thrust_factor_per_jet is on) */

  /****** Work space + Pointers + Structures ******/
  enum RcsJetStatus {
    Status_Off      = 0,
    Status_On       = 1,
    Status_BuildUp  = 2,
    Status_TrailOff = 3
  };
  RcsJetStatus status;  /* (--)  jet status */
  double on_com_time;   /* (s)
       Time since on command input not including any buffered time,
       i.e. on_com_time = 0 during the first time step that an rcs jet force
       is applied and is incremented by time_step while in build up or on*/
  double off_com_time;  /* (s)
       Time since off command input not including any buffered time,
       i.e. off_com_time = 0 during the first time step that an rcs jet force
       is not applied and is incremented by time_step while in trail off or off*/
  double on_com_time1;   /* (s)
       Time since on command input not including any buffered time,
       i.e. on_com_time = 0 during the first time step that an rcs jet force
       is applied and is incremented by time_step while in build up or on*/
  double off_com_time1;  /* (s)
       Time since off command input not including any buffered time,
       i.e. off_com_time = 0 during the first time step that an rcs jet force
       is not applied and is incremented by time_step while in trail off or off*/

  double time_left_in_trailoff; /* (s)  Time left in trail off */
  double delta_time_on;  /* (s)  Work space for time jet is on */
  double scaled_force;   /* (N)
       Scaled Force due to thrust degradation when multiple jets are firing;
       is equal to force_cl * thrust_factor */

  double total_delay_on;  /* (s)  Total delay before turning jets on   */
  double total_delay_off; /* (s)  Total delay before turning jets off  */

  /****** Outputs ******/
  std::list<bool> commands; /* (--)
       Jet command buffer to accomodate delays that are > time_step*/
  bool command;     /* (--) Current command-on status. */
  int    nfired;    /* (--)  number of times this jet has fired */
  double sum_time;  /* (s)  total time over which this jet is fired */
  double torque[3]; /* (N*m)  Torque applied during a time step */
  double scaled_impingement_force[3];  /* (N)
       Scaled value of base_impingement_force. */
  double scaled_impingement_torque[3];  /* (N*m)
       Scaled value of base_impingement_torque. */

  RcsJet( RcsGeneric & rcs_system_,
          RcsPropPod & prop_pod_,
          RcsJetGroup & group_);
  void initialize();
  void update (bool command_);
  void compute_jet_forces();
  void compute_prop_consumption();
  void get_force_direction(double force_dir[3]);
  double get_component_consumption( unsigned int ix);
  double get_component_flow_rate( unsigned int ix);
  void set_component_flow_rate( unsigned int component_ix, double rate);
  void set_force_direction( double force_dir[3]);
  void set_force_direction(
                 double force_dir_x, double force_dir_y, double force_dir_z);
  void scale_self_impingement();
  void set_isp( double new_isp);
  double get_isp() {return isp;}
 protected:
  void compute_component_flow_rates();
  void blow_down();
  void update_jet_status();
  void disperse_force();
  void generate_force_direction_matrix();
  void apply_direction_error();
  void apply_direction_dispersion();
  void switch_status( RcsJetStatus new_status);

 private:
   // Not implemented:
   RcsJet (const RcsJet& rhs);
   RcsJet & operator = (const RcsJet& rhs);;
};
#endif