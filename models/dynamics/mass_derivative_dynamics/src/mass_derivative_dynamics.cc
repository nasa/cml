/*******************************TRICK HEADER**********************************
PURPOSE: (Models the effect of a rapidly changing mass and inertia tensor.
          This model creates a pseudo-force and pseudo-torque such that
          integration can proceed under the assumption of constant
          mass properties while the physics of variable mass properties
          will still be handled)

LIBRARY DEPENDENCY:
   ((cml/models/utilities/math_utils/src/math_utils.cc)
   )

PROGRAMMERS:
  (((Gary Turner) (OSR) (Sep 2018) (Antares) (initial)))
*****************************************************************************/

#include "../include/mass_derivative_dynamics.hh"

#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "jeod/models/dynamics/mass/include/mass_properties.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
MassDerivativeDynamics::MassDerivativeDynamics(
    const double & time,
    const jeod::MassProperties & propellant_properties,
    const double R_nozzle_[3],
    jeod::DynBody & body_)
  :
  MassDerivativeDynamics(time,
                         propellant_properties.mass,
                         propellant_properties.inertia,
                         propellant_properties.position,
                         R_nozzle_,
                         body_)
{}
/****************************************************************************/
MassDerivativeDynamics::MassDerivativeDynamics(
    const double & time,
    const double & propellant_mass_,
    const double (& inertia_)[3][3],
    const double R_propellant_[3],
    const double R_nozzle_[3],
    jeod::DynBody & body_)
  :
  current_time(time),
  propellant_mass(propellant_mass_),
  intrinsic_prop_inertia(inertia_),
  R_propellant( R_propellant_),
  R_nozzle( R_nozzle_),
  body( body_),

  mass_order(2),
  R_propellant_order(2),
  J_order(2),
  pseudo_torque{0.0, 0.0, 0.0},
  pseudo_force{0.0, 0.0, 0.0},
  pseudo_dv{0.0, 0.0, 0.0},
  propellant_mass_history(),
  R_propellant_history(),
  inertia_history(),

  last_time(0.0),
  dt (0.0),
  include_velocity_effect(false),
  m_dot(0.0),
  r_nozzle{0.0, 0.0, 0.0},
  r_propellant{0.0, 0.0, 0.0},
  R_propellant_dot{0.0, 0.0, 0.0},
  r_nozzle_skew{{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0}},
  r_propellant_skew{{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0}},
  R_propellant_dot_skew{{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0}},
  torque_mx{{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0}},
  J_dot{{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0}}
{
  subscribe_name = "idot_dynamics";
  // R_propellant_history is a 3-array of history data for each component
  // inertia_history is a 3x3-array of history data for each component
  R_propellant_history.resize(3);
  inertia_history.resize(6);
  if (R_nozzle == nullptr ||
      R_propellant == nullptr) {
    // NOTE -- this block is not covered in regression testing.
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid construction\n",
      "The position vectors are NULL.\n");
  }
}



/*****************************************************************************
update
Purpose:(main executable)
*****************************************************************************/
void
MassDerivativeDynamics::update()
{
  if (!active) {
    return;
  }

  if ( MathUtils::is_near_equal( current_time, last_time)) {
    update_torque();
    return;
  }
  // Generate the derivatives
  dt =  current_time - last_time;
  last_time = current_time;
  compute_m_dot();
  compute_R_propellant_dot();
  compute_J_dot();

  // Generate the nozzle position wrt the CoM, still expressed in the structural
  // frame.  This is used for both the torque and force computations.
  // First, ensure that the composite-properties are up-to-date.
  body.mass.update_mass_properties();
  jeod::Vector3::diff( R_nozzle,
                 body.mass.composite_properties.position,
                 r_nozzle);

  // Generate the matrix [K] from documentation.
  // tau = [K] omega
  compute_torque_mx();

  update_force();
  update_torque();

  if (include_velocity_effect) {
    modify_velocity();
  }
}


/*****************************************************************************
set_include_velocity_effect
Purpose:(sets the include_velocity_effect boolean flag)
*****************************************************************************/
void
MassDerivativeDynamics::set_include_velocity_effect( bool flag)
{
  // if turning on velocity effects, check that the body is root before 
  // accepting the instruction; leave without effecting the change if it is not.
  if (flag && !include_velocity_effect) {
    if (!body.is_root_body()) {
      // NOTE -- this block is not covered in regression verifs, it has been
      // executed manually by modifying the return value of is_root_body() in
      // gdb.
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid application of pseudo_dv\n",
        "The pseudo_dv term can only be applied to a root body.\n"
        "This not a root-body so pseudo_dv will not be applied.\n");
      return;
    }
    // else accept the assignment
  }
  // if turning off velocity effects, remove existing effect.
  else if (!flag && include_velocity_effect) {
    double new_body_vel[3];
    jeod::Vector3::diff( body.composite_body.state.trans.velocity,
                   pseudo_dv,
                   new_body_vel);
    body.set_velocity( new_body_vel,
                       body.composite_body);
    body.propagate_state();
    // clear the pseudo-dv and the internally modified body frame velocity
    jeod::Vector3::initialize(pseudo_dv);
  }
  // else, no action, keep as is.
  include_velocity_effect = flag;
}


/*****************************************************************************
activate
Purpose:(records values at the time the model is activated.)
*****************************************************************************/
void
MassDerivativeDynamics::activate()
{
  last_time = current_time;

  // Record initial values in history table
  size_t ix = 0;
  propellant_mass_history.push_front( propellant_mass);
  for (size_t ii = 0; ii < 3; ++ii) {
    R_propellant_history[ii].push_front( R_propellant[ii]);
    for (size_t jj = ii; jj < 3; ++jj) {
      inertia_history[ix].push_front( intrinsic_prop_inertia[ii][jj]);
      ++ix;
    }
  }
  active = true;
}


/*****************************************************************************
deactivate
Purpose:(Turns the model off)
*****************************************************************************/
void
MassDerivativeDynamics::deactivate()
{
  // clear the artifical pseudo-dv by calling set_include_velocity_effect(false)
  if (include_velocity_effect) {
    set_include_velocity_effect(false);

    // Reset include_velocity_effect to true since the above call
    // sets it to false
    include_velocity_effect = true;
  }

  // zero the pseudo force and torque; they won't be computed again because
  // the active flag blocks it.
  jeod::Vector3::initialize(pseudo_force);
  jeod::Vector3::initialize(pseudo_torque);
  active = false;
}

/*****************************************************************************
update_force
Purpose:(A method to be called at the integration midsteps to update the
         force based on the modified angular rates)
*****************************************************************************/
void
MassDerivativeDynamics::update_force()
{
  // pseudo-torque = m-dot * (omega cross r_N)
  double mdot_angular_rate_struc[3]; // angular rate expressed in structural
  jeod::Vector3::transform_transpose( body.mass.composite_properties.T_parent_this,
                                body.composite_body.state.rot.ang_vel_this,
                                mdot_angular_rate_struc);
  // scale by m-dot:
  jeod::Vector3::scale( m_dot,
                  mdot_angular_rate_struc);

  jeod::Vector3::cross( mdot_angular_rate_struc,
                  r_nozzle,
                  pseudo_force);
}

/*****************************************************************************
update_torque
Purpose:(A method to be called at the integration midsteps to update the
         torque based on the modified angular rates)
*****************************************************************************/
void
MassDerivativeDynamics::update_torque()
{
  // pseudo-torque = [torque_mx] * angular-rate.
  jeod::Vector3::transform( torque_mx,
                      body.composite_body.state.rot.ang_vel_this,
                      pseudo_torque);
}
/*****************************************************************************
compute_torque_mx
Purpose:(Generate the matrix [K] from documentation; tau = [K] omega)
*****************************************************************************/
void
MassDerivativeDynamics::compute_torque_mx()
{
  jeod::Vector3::diff( R_propellant,
                 body.mass.composite_properties.position,
                 r_propellant);

  // Generate the skew-matrices
  jeod::Matrix3x3::cross_matrix( r_nozzle,
                           r_nozzle_skew);
  jeod::Matrix3x3::cross_matrix( r_propellant,
                           r_propellant_skew);
  jeod::Matrix3x3::cross_matrix( R_propellant_dot,
                           R_propellant_dot_skew);

  // 1st term, 1st part: scratch1 = m-dot * ([r_N][r_N] - [r_P][r_P])
  double scratch1[3][3];
  double scratch2[3][3];
  jeod::Matrix3x3::product_right_transpose( r_nozzle_skew,
                                      r_nozzle_skew,
                                      scratch1);
  jeod::Matrix3x3::product_right_transpose( r_propellant_skew,
                                      r_propellant_skew,
                                      scratch2);
  jeod::Matrix3x3::decr( scratch2,
                   scratch1);
  jeod::Matrix3x3::scale( m_dot,
                    scratch1);

  // 1st term, 2nd part: scratch2 = -m * ([R_dot][r_p] + [r_p][R_dot])
  double scratch3[3][3];
  jeod::Matrix3x3::product_right_transpose( R_propellant_dot_skew,
                                      r_propellant_skew,
                                      scratch2);
  jeod::Matrix3x3::product_right_transpose( r_propellant_skew,
                                      R_propellant_dot_skew,
                                      scratch3);
  jeod::Matrix3x3::incr( scratch3,
                   scratch2);
  jeod::Matrix3x3::scale( propellant_mass,
                    scratch2);

  // 1st term: tau_A = [1st part - 2nd part] T'
  jeod::Matrix3x3::subtract( scratch1,
                       scratch2,
                       scratch3);
  jeod::Matrix3x3::product_right_transpose( scratch3,
                                      body.mass.composite_properties.T_parent_this,
                                      torque_mx);
  // scratch* are clear to use again


  // second term: scratch1 = [T'] [J-dot]
  jeod::Matrix3x3::product_left_transpose( body.mass.composite_properties.T_parent_this,
                                     J_dot,
                                     scratch1);

  // combined [K] = 1st term - 2nd term
  jeod::Matrix3x3::decr( scratch1,
                   torque_mx);
}

/*****************************************************************************
compute_m_dot
Purpose:(Computes the rate of change of mass)
*****************************************************************************/
void
MassDerivativeDynamics::compute_m_dot()
{
  propellant_mass_history.push_front( propellant_mass);
  m_dot = MathUtils::compute_backward_difference( propellant_mass_history) /
          dt;
  // Keep enough of the most recent history to generate the next derivative,
  // clear the oldest value out of the back.
  if (propellant_mass_history.size() > mass_order) {
    propellant_mass_history.pop_back();
  }
}

/*****************************************************************************
compute_R_propellant_dot
Purpose:(computes the rate of change of the position of the propellant tank
         relative to the structural frame.)
*****************************************************************************/
void
MassDerivativeDynamics::compute_R_propellant_dot()
{
  for (size_t ii = 0; ii < 3; ++ii) {
    R_propellant_history[ii].push_front( R_propellant[ii]);
    R_propellant_dot[ii] =
         MathUtils::compute_backward_difference( R_propellant_history[ii]);
  }
  jeod::Vector3::scale( (1/dt),
                  R_propellant_dot);

  // Keep enough of the most recent history to generate the next derivative,
  // clear the oldest value out of the back.
  if (R_propellant_history[0].size() > R_propellant_order) {
    for (size_t ii = 0; ii < 3; ++ii) {
      R_propellant_history[ii].pop_back();
    }
  }
}

/*****************************************************************************
compute_J_dot
Purpose:(computes the rate of change of the inertia tensor)
*****************************************************************************/
void
MassDerivativeDynamics::compute_J_dot()
{
  size_t ix = 0;
  double J_dot_6[6]; // The differences of the 6 unique elements of the
                     // inertia tensor.
  // Step through the upper triangle (including diagonals) of the inertia
  // tensor.  Ignore the 3 terms in the lower triangle; they are duplicates.
  for (size_t ii = 0; ii < 3; ++ii) {
    for (size_t jj = ii; jj < 3; ++jj) {
      inertia_history[ix].push_front( intrinsic_prop_inertia[ii][jj]);
      J_dot_6[ix] =
          MathUtils::compute_backward_difference( inertia_history[ix]);
      ++ix;
    }
  }
  // Construct the 9-element inertia tensor derivative.
  J_dot[0][0] = J_dot_6[0];
  J_dot[0][1] =
  J_dot[1][0] = J_dot_6[1];
  J_dot[0][2] =
  J_dot[2][0] = J_dot_6[2];
  J_dot[1][1] = J_dot_6[3];
  J_dot[1][2] =
  J_dot[2][1] = J_dot_6[4];
  J_dot[2][2] = J_dot_6[5];

  jeod::Matrix3x3::scale( (1/dt),
                    J_dot);

  // Keep enough of the most recent history to generate the next derivative,
  // clear the oldest value out of the back.
  if (inertia_history[0].size() > J_order) {
    for (size_t ii = 0; ii < 6; ++ii) {
      inertia_history[ii].pop_back();
    }
  }
}

/*****************************************************************************
modify_velocity
Purpose:(Applies a temporary non-inertial, non-momentum carrying modification
         to the velocity of the center of mass.)
*****************************************************************************/
void
MassDerivativeDynamics::modify_velocity()
{
  // remove the previously applied delta-v
  double new_body_vel[3];
  jeod::Vector3::diff( body.composite_body.state.trans.velocity,
                 pseudo_dv,
                 new_body_vel);
  // now compute and apply the new one.
  jeod::Vector3::transform_transpose( body.structure.state.rot.T_parent_this,
                                r_nozzle,
                                pseudo_dv);
  jeod::Vector3::scale (MathUtils::divide_protected( m_dot,
                                               body.mass.composite_properties.mass,
                                               0.0),
                  pseudo_dv);
  jeod::Vector3::incr( pseudo_dv,
                 new_body_vel);
  body.set_velocity( new_body_vel,
                     body.composite_body);
  body.propagate_state();
}