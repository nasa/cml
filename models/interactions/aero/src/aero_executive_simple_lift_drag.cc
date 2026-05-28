/*******************************TRICK HEADER************************************
PURPOSE: (Provides a simple non-rotating aerodynamic lift/drag model.)

PROGRAMMERS:
  (((Jeremy Rea) (NASA) (March 2025) (Antares) (initial version, based on AeroExecutiveSimple)))
*******************************************************************************/

#include <cstring>  // NULL
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"  // Matrix3x3
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/aero_executive_simple_lift_drag.hh"

/*******************************************************************************
Constructor:
*******************************************************************************/
AeroExecutiveSimpleLiftDrag::AeroExecutiveSimpleLiftDrag( 
    AeroInterfaceOutput & output_ref,
    const double & dynamic_pressure_in,
    const double (&T_inrtl_traj_in)[3][3],
    const double (&T_inrtl_struc_in)[3][3],
    const double & mass_in)
  :
  AeroExecutiveBase( output_ref),
  dynamic_pressure( dynamic_pressure_in),
  T_inrtl_traj( T_inrtl_traj_in),
  T_inrtl_struc( T_inrtl_struc_in),
  mass( mass_in),
  aero_type( constant_mag),
  T_traj_wind(),
  bank_angle(0.0),
  LoD(0.0),
  BC(1.0),
  CL(0.0),
  CD(0.0),
  lift_mag(0.0),
  drag_mag(0.0),
  bc_epsilon(1E-6)
{
  jeod::Matrix3x3::identity(T_traj_wind);
}

/*******************************************************************************
initialize
Purpose: (Initializes the executive.)
*******************************************************************************/
void
AeroExecutiveSimpleLiftDrag::initialize()
{

  // Compute the trajectory-to-wind transformation matrix
  // Note that since bank is constant, this transformation is also constant
  T_traj_wind[1][1] =  std::cos(bank_angle);
  T_traj_wind[1][2] =  std::sin(bank_angle);
  T_traj_wind[2][1] = -std::sin(bank_angle);
  T_traj_wind[2][2] =  std::cos(bank_angle);

  AeroExecutiveBase::initialize();
}

/*******************************************************************************
update
Purpose: (Calculates the drag force.)
*******************************************************************************/
void AeroExecutiveSimpleLiftDrag::update()
{
  if (!active) {
    return;
  }
  // Generate the magnitude of the drag force (unless it is constant)
  switch (aero_type) {
  case ballistic_coeff_and_LoD:
    // div-0 check. But BC can be arbitrarily small for a small mass,
    // so it is most appropriate to check the ratio between the value of.
    // BC and the dynamic-pressure
    if (std::abs(dynamic_pressure * bc_epsilon) >= BC) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid settings\n",
        "The value for BC (", BC, ") is unusually small.\nGiven the current "
        "dynamic_pressure (", dynamic_pressure, "), the resulting acceleration (P / BC) would\n"
        "be excessively large (or undefined in the case of (0.0 / 0.0)).\n"
        "Disabling Aerodynamics.\n");
      lift_mag = 0.0;
      drag_mag = 0.0;
      disable();
      break;
    }
    lift_mag = dynamic_pressure * mass * LoD / BC;
    drag_mag = dynamic_pressure * mass / BC;
    break;
  case lift_drag_coeff:
    lift_mag = dynamic_pressure * Aref * CL;
    drag_mag = dynamic_pressure * Aref * CD;
    break;
  default: // no action;
    break;
  }

  // Compute inertial-to-wind transformation matrix
  double T_inrtl_wind[3][3]; /* (--) Inertial-to-wind transformation */
  jeod::Matrix3x3::product( T_traj_wind,
                      T_inrtl_traj,
                      T_inrtl_wind);

  // Update the aerodynamic force vector
  output.force[0] = -drag_mag;
  output.force[1] = 0.0;
  output.force[2] = -lift_mag;
  jeod::Vector3::transform_transpose(T_inrtl_wind, output.force);
  jeod::Vector3::transform(T_inrtl_struc, output.force);

}

/*******************************************************************************
set_aero_type
Purpose: (Sets the protected aero_type value.)
*******************************************************************************/
void
AeroExecutiveSimpleLiftDrag::set_aero_type(AeroType aero_type_in)
{
  if (aero_type_in == aero_type) {
    return;
  }

  switch (aero_type_in) {
  case ballistic_coeff_and_LoD:
    if (BC <= 0.0) {
      CMLMessage::fail(
        __FILE__, __LINE__, "Simple Lift/Drag Aerodynamics Setup error.\n",
        "Selected Ballistic Coefficient as the method for computing the aero-"
        "drag\nforce but the value for BC (", BC, ") is illegal.\n");
    }
    break;
  case lift_drag_coeff:
    if (MathUtils::is_near_equal(CD, 0.0)) {
      CMLMessage::warn(
        __FILE__, __LINE__, "Simple Lift/Drag Aerodynamics Setup error.\n",
        "Selected Coefficient of drag as the method for computing the aero-"
        "drag\nforce but CD = 0.0, which means there will be no drag force.\n");
    }
    if (MathUtils::is_near_equal(Aref, 0.0)) {
      CMLMessage::warn(
        __FILE__, __LINE__, "Simple Lift/Drag Aerodynamics Setup error.\n",
        "Selected Coefficient of lift/drag as the method for computing the aero"
        "\nforce but Aref = 0.0, which means there will be no lift or drag force.\n");
    }
    break;
  case constant_mag:
    if (MathUtils::is_near_equal(drag_mag, 0.0)) {
      CMLMessage::warn(
        __FILE__, __LINE__, "Simple Lift/Drag Aerodynamics Setup error.\n",
        "Selected constant-lift/drag as the method for computing the aero"
        "\nforce but specified 0.0 for the drag force magnitude (drag_mag).\n");
    }
    break;
  }
  aero_type = aero_type_in;
}
