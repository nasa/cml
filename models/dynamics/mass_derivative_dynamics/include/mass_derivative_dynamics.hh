/*******************************TRICK HEADER******************************
PURPOSE: (Models the effect of a rapidly changing mass and inertia tensor.
          This model creates a pseudo-force and pseudo-torque such that
          integration can proceed under the assumption of constant
          mass properties while the physics of variable mass properties
          will still be handled)

LIBRARY DEPENDENCY:
   ((../src/mass_derivative_dynamics.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Sep 2018) (Antares) (initial)))
**********************************************************************/
#ifndef CML_MASS_DERIVATIVE_DYNAMICS_HH
#define CML_MASS_DERIVATIVE_DYNAMICS_HH

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"
#include "jeod/models/dynamics/mass/include/mass_properties.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include <vector>
#include <list>

/*****************************************************************************
MassDerivativeDynamics
Purpose:(Provides a pseudo-force and pseudo-torque to emulate the effects of
         mass derivatives during an integration  processs that assumes
         constant mass properties.)
*****************************************************************************/
class MassDerivativeDynamics : public SubscriptionBase
{
 protected:
  const double & current_time; /* (--)
        Reference to the sim dynamic time. */
  const double & propellant_mass; /* (--)
        Reference to the propellant mass value.*/
  const double (& intrinsic_prop_inertia)[3][3]; /* (--)
        Reference to the tensor representing the intrinsic
        inertia of the propellant.*/
  const double * R_propellant; /* (--)
        Reference to the position of the center of mass of the
        propellant; expressed relative to structural frame.*/
  const double * R_nozzle; /* (--)
        Reference to the position of the point at which mass is released
        (typically a rocket motor nozzle -- hence the name);
        expressed relative to the structural frame of the root body, which
        is identified by the reference "body".*/
  jeod::DynBody & body; /* (--)
        Reference to the Dynamic Body representing the vehicle.
        Non-const only to accommodate the optional velocity effect.
        This body should be the root of the current mass tree.*/

 public:
  // INPUTS (model configuration)
  size_t mass_order;   /* (--)
      The desired order of the backward-difference operator to be used
      when computing m-dot.*/
  size_t R_propellant_order;  /* (--)
      The desired order of the backward-difference operator to be used
      when computing R-dot.*/
  size_t J_order;  /* (--)
      The desired order of the backward-difference operator to be used
      when computing J-dot.*/

  // OUTPUTS:
  double pseudo_torque[3]; /* (N*m)
      The pseudo-torque necessary to address the assumption of constant
      inertia in the state integration.*/
  double pseudo_force[3]; /* (N)
      The pseudo-force necessary to address the assumption of constant
      mass in the state integration.*/
  double pseudo_dv[3]; /* (m/s)
      The pseudo-velocity associated with the reassignment of the
      center-of-mass within the vehicle structure.*/

  std::list<double> propellant_mass_history; /* (kg)
          A list containing the history of the propellant mass.*/
  std::vector< std::list< double> > R_propellant_history;/* (m)
          3 lists, each containing the history of one component
          of the position vector of the propellant relative to the
          structural frame. */
  std::vector< std::list< double> > inertia_history; /* (kg*m2)
          6 lists, each containing the history of one of the unique
          elements of the inertia tensor. */

 protected:
  double last_time; /* (s)
        The value of dynamic-time on the most recent evaluation of
        derivatives. */
  double dt; /* (s)
        The time elapsed since the previous evaluation of derivatives. */
  bool include_velocity_effect; /* (--)
        Indicates whether to include the velocity effect.  Default false. */
  double m_dot; /* (kg/s)
        The time-derivative of the propellant mass. */
  double r_nozzle[3]; /* (m)
        The position of the nozzle relative to the center of mass
        expressed in the structural frame. */
  double r_propellant[3]; /* (m)
        The position of the propellant relative to the center of mass
        expressed in the structural frame. */
  double R_propellant_dot[3]; /* (m/s)
        The vector representing the time-derivative
        of the structural position of the propellant.*/

  double r_nozzle_skew[3][3]; /* (m)
        The skew matrix of the vector representing the position of the
        nozzle relative to the center of mass. */
  double r_propellant_skew[3][3]; /* (m)
        The skew matrix of the vector representing the position of the
        propellant relative to the center of mass. */
  double R_propellant_dot_skew[3][3]; /* (m/s)
        The skew matrix of the vector representing the time-derivative
        of the structural position of the propellant.*/
  double torque_mx[3][3]; /* (N*m*s)
        The multiplicative factor [K] in tau = [K] omega. */
  double J_dot[3][3]; /* (kg*m2/s)
        The derivative of the inertia tensor. */

 public:
  MassDerivativeDynamics( const double & time,
                          const jeod::MassProperties & propellant_properties,
                          const double R_nozzle[3],
                          jeod::DynBody & vehicle);
  MassDerivativeDynamics( const double & time,
                          const double & propellant_mass,
                          const double (& intrinsic_prop_inertia)[3][3],
                          const double R_propellant[3],
                          const double R_nozzle[3],
                          jeod::DynBody & vehicle);

  void update();
  void update_force_torque() {update_force(); update_torque();}
  void set_include_velocity_effect(bool flag);
 protected:
  void default_settings();
  virtual void activate();
  virtual void deactivate();
  void update_force();
  void update_torque();
  void compute_torque_mx();
  void compute_m_dot();
  void compute_R_propellant_dot();
  void compute_J_dot();
  void modify_velocity();
 private:
  // Disable the copy/assignment operations
  MassDerivativeDynamics (const MassDerivativeDynamics&);
  MassDerivativeDynamics& operator = (const MassDerivativeDynamics&);
};
#endif
