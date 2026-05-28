/*******************************TRICK HEADER************************************
PURPOSE: (Provides a simple non-rotating aerodynamic drag model.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (March 2015) (Antares) (initial version))
   ((Gary Turner) (OSR) (June 2016) (Antares)
      (Refactor to integrate simple into the same interface as table and API))
   ((Bingquan Wang) (OSR) (April 2017) (Antares)
      (Fixed the compilation warning of float-point number equality comparison))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#include <cstring>  // NULL
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/aero_executive_simple.hh"

/*******************************************************************************
Constructor:
*******************************************************************************/
AeroExecutiveSimple::AeroExecutiveSimple( 
    AeroInterfaceOutput & output_ref,
    const double & dynamic_pressure_in,
    const double * const free_stream_vel_unit_in,
    const double (&T_inrtl_struc_in)[3][3],
    const double & mass_in)
  :
  AeroExecutiveBase( output_ref),
  dynamic_pressure( dynamic_pressure_in),
  T_inrtl_struc( T_inrtl_struc_in),
  mass( mass_in),
  free_stream_vel_unit( free_stream_vel_unit_in),
  drag_type( constant_vec),
  BC(1.0),
  CD(0.0),
  drag_mag(0.0),
  bc_epsilon(1E-6)
{
  if (free_stream_vel_unit == NULL) {
   CMLMessage::warn(
     __FILE__, __LINE__, "Simple Aerodynamics Construction limitation\n",
     "The free stream velocity is required for any computations of the\n"
     "aero forces and it is NULL.\n"
     "Options are limited to constant vector implementation\n");
  }
}

/*******************************************************************************
initialize
Purpose: (Initializes the executive.)
*******************************************************************************/
void
AeroExecutiveSimple::initialize()
{
  if (drag_type == constant_vec) {
    drag_mag = jeod::Vector3::vmag(output.force);
  }
  AeroExecutiveBase::initialize();
}

/*******************************************************************************
update
Purpose: (Calculates the drag force.)
*******************************************************************************/
void AeroExecutiveSimple::update()
{
  if (!active) {
    return;
  }
  // Generate the magnitude of the drag force (unless it is constant)
  switch (drag_type) {
  case ballistic_coeff:
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
      drag_mag = 0.0;
      disable();
      break;
    }
    drag_mag = dynamic_pressure * mass / BC;
    break;
  case drag_coeff:
    drag_mag = dynamic_pressure * Aref * CD;
    break;
  case constant_vec:
    return; // don't update the drag vector
  default: // no action;
    break;
  }

  // Update the drag vector
  jeod::Vector3::scale(free_stream_vel_unit, -drag_mag, output.force);
  jeod::Vector3::transform(T_inrtl_struc, output.force);
}

/*******************************************************************************
set_drag_type
Purpose: (Sets the protected drag_type value.)
*******************************************************************************/
void
AeroExecutiveSimple::set_drag_type(DragType drag_type_in)
{
  if (drag_type_in == drag_type) {
    return;
  }

  if (free_stream_vel_unit == NULL) {
    CMLMessage::error(
      __FILE__, __LINE__, "Simple Aerodynamics Construction error.\n",
      "The free stream velocity is required for computing the aero drag\n"
      "and it is NULL.\nRemaining with the constant-vector implementation.\n");
    return;
  }

  switch (drag_type_in) {
  case ballistic_coeff:
    if (BC <= 0.0) {
      CMLMessage::fail(
        __FILE__, __LINE__, "Simple Aerodynamics Setup error.\n",
        "Selected Ballistic Coefficient as the method for computing the aero-"
        "drag\nforce but the value for BC (", BC, ") is illegal.\n");
    }
    break;
  case drag_coeff:
    if (MathUtils::is_near_equal(CD, 0.0)) {
      CMLMessage::warn(
        __FILE__, __LINE__, "Simple Aerodynamics Setup error.\n",
        "Selected Coefficient of drag as the method for computing the aero-"
        "drag\nforce but CD = 0.0, which means there will be no drag force.\n");
    }
    if (MathUtils::is_near_equal(Aref, 0.0)) {
      CMLMessage::warn(
        __FILE__, __LINE__, "Simple Aerodynamics Setup error.\n",
        "Selected Coefficient of drag as the method for computing the aero-"
        "drag\nforce but Aref = 0.0, which means there will be no drag force.\n");
    }
    break;
  case constant_mag:
    if (MathUtils::is_near_equal(drag_mag, 0.0)) {
      CMLMessage::warn(
        __FILE__, __LINE__, "Simple Aerodynamics Setup error.\n",
        "Selected constant-drag as the method for computing the aero-"
        "drag\nforce but specified 0.0 for the force magnitude (drag_mag).\n");
    }
    break;
  case constant_vec:
    drag_mag = jeod::Vector3::vmag(output.force);
    if (MathUtils::is_near_equal(drag_mag, 0.0)) {
      CMLMessage::warn(
        __FILE__, __LINE__, "Simple Aerodynamics Setup error.\n",
        "Selected constant-drag-vector as the method for specifying the aero-"
        "drag\nforce but the magnitude of the specified force is 0.0.\n");
    }
    break;
  }
  drag_type = drag_type_in;
}