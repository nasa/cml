/*******************************************************************************
PURPOSE:
  (Header file for AeroInterfaceOutput C++ class)

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2016) (Antares) (new))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_INTERFACE_OUTPUT_HH
#define CML_AERO_INTERFACE_OUTPUT_HH

#include "jeod/models/utils/math/include/vector3.hh"

namespace cml {

/*******************************************************************************
AeroInterfaceOutput
Purpose:(The data elements recognized as the primary outputs of the aero models.)
*******************************************************************************/
class AeroInterfaceOutput
{
public:
  double force[3]{};       /* (N)
         Output force applied to vehicle in vehicle structural frame.
         NOTE - in the simple aero case, this is the ONLY output. */

  double torque[3]{};      /* (N*m)
         Output torque applied to vehicle in vehicle structural frame. */
  double moment_mrc[3]{};  /* (N*m)
         Current aerodynamic moment vector about MRC in struc frame. */
  double drag_force{0.0};     /* (N)  Current aerodynamic drag force. */
  double side_force{0.0};     /* (N)  Current aerodynamic side force. */
  double lift_force{0.0};     /* (N)  Current aerodynamic lift force. */

  double LoD{0.0};            /* (--) Lift over Drag ratio. */
  double epsilon_CD_for_LoD{1.0e-10}; /* (--)
       Epsilon value for comparing value of CD before doing CL / CD. */

  AeroInterfaceOutput() = default;


  AeroInterfaceOutput (const AeroInterfaceOutput &) = delete;
  AeroInterfaceOutput & operator = (const AeroInterfaceOutput &) = delete;

  void zero_dsl() {
    drag_force = 0.0;
    lift_force = 0.0;
    side_force = 0.0;
    LoD = 0.0;
  }

  void zero_torque() {
    jeod::Vector3::initialize(torque);
    jeod::Vector3::initialize(moment_mrc);
  }

  void zero_everything() {
    jeod::Vector3::initialize(force);
    zero_dsl();
    zero_torque();
  }
};


} // namespace cml

#endif