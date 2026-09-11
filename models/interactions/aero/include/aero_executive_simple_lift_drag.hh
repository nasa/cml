/*******************************************************************************
PURPOSE:
  (Header file for AeroExecutiveSimpleLiftDrag C++ class)

LIBRARY DEPENDENCY:
  ((../src/aero_executive_simple_lift_drag.cc))

PROGRAMMERS:
  (((Jeremy Rea) (NASA) (March 2025) (Antares) (initial version, based on AeroExecutiveSimple)))
*******************************************************************************/

#ifndef CML_AERO_EXECUTIVE_SIMPLE_LIFT_DRAG_HH
#define CML_AERO_EXECUTIVE_SIMPLE_LIFT_DRAG_HH

#include "aero_executive_base.hh"

namespace cml {

class AeroInterfaceOutput;

/*******************************************************************************
AeroExecutiveSimpleLiftDrag
Purpose:(Provides a simple aerodynamic executive to use with the AeroExec.)
*******************************************************************************/
class AeroExecutiveSimpleLiftDrag : public AeroExecutiveBase
{
public:
  enum AeroType {
    ballistic_coeff_and_LoD = 0,  // Use Ballistic Coefficient and L/D

    lift_drag_coeff         = 1,  // Use Coefficient of lift and drag

    constant_mag            = 2   // Use a constant drag and lift magnitude
  };

protected:
  /***************************************************************************/
  // Additional references to external models, used as inputs
  /***************************************************************************/
  const double & dynamic_pressure;     /* (N/m2) Dynamic pressure */
  const double (&T_inrtl_traj)[3][3]; /* (--)
                inertial-to-trajectory transformation matrix. */
  const double (&T_inrtl_struc)[3][3]; /* (--)
                inertial-to-structure transformation matrix. */
  const double & mass;                 /* (kg)   Vehicle mass. */

  /***************************************************************************/
  // internal values
  /***************************************************************************/
  AeroType aero_type{constant_mag}; /* (--) Specifies which model to use. */
  double   T_traj_wind[3][3]{};     /* (--) Trajectory-to-wind transformation */

public:
  double bank_angle{0.0};  /* (rad)   Bank angle. */
  double LoD{0.0};         /* (--)    Lift-to-drag ratio. */
  double BC{1.0};          /* (kg/m2) Ballistic Coefficient. */
  double CL{0.0};          /* (--)    Coefficient of lift. */
  double CD{0.0};          /* (--)    Coefficient of drag. */
  double lift_mag{0.0};    /* (N)     Magnitude of lift force. */
  double drag_mag{0.0};    /* (N)     Magnitude of drag force. */
  double bc_epsilon{1E-6}; /* (s2/m)
          A test-value for the validity of BC, ensuring that
          (dyn-pressure / BC) < (1/bc_epsilon).
          It is effectively 1/(the largest acceleration for which the
          BC approximation is valid). Defaults to 1E-6; That makes a
          maximum acceleration of 1E6 m/s^2. Can be adjusted to accommodate
          extreme cases. */

  AeroExecutiveSimpleLiftDrag( AeroInterfaceOutput & output_ref,
                       const double & dynamic_pressure_in,
                       const double (&T_inrtl_traj_in)[3][3],
                       const double (&T_inrtl_struc_in)[3][3],
                       const double & mass_in);
  ~AeroExecutiveSimpleLiftDrag() override = default;
  AeroExecutiveSimpleLiftDrag (const AeroExecutiveSimpleLiftDrag &) = delete;
  AeroExecutiveSimpleLiftDrag & operator = (const AeroExecutiveSimpleLiftDrag &) = delete;

  void set_aero_type(AeroType aero_type_in);

  void initialize() override;
  void update() override;
};


} // namespace cml

#endif