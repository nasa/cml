/********************************* TRICK HEADER *******************************
PURPOSE: (Disperses the state directly.  Allows the application of dispersions
          specified in one coordinate system with nominal state values specified
          in another.
          Supports translational state dispersion only;
          addition of rotational state dispersion is simple.)


LIBRARY DEPENDENCY:
    ((../src/monte_carlo_state_dispersion.cc))

PROGRAMMERS:
    (((Gary Turner) (OSR) (Jan 2019) (Antares) (initial)))
    )
******************************************************************************/

#ifndef ANTARES_MONTE_CARLO_STATE_DISPERSION_HH
#define ANTARES_MONTE_CARLO_STATE_DISPERSION_HH

#include "jeod/models/dynamics/body_action/include/dyn_body_init_trans_state.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils::*

class MonteCarloStateDispersion {
 protected: // external reference
  jeod::DynBodyInitTransState & trans_init; /* (--) reference to StateInitialize's
                                                    DynBodyInitTransState instance.*/

 public:
  enum DispersionFrame{
    Inertial = 0, /* Dispersion is in Inertial   */
    Lvlh     = 1, /* Dispersion is in LVLH */
    UVW      = 2, /* Dispersion is in UVW */
  };

  DispersionFrame frame; /* (--) specifies the frame in which the
                                 position-velocity dispersion values are to be
                                 interpreted.*/
  bool disperse_pv;      /* (--) Flag indicating whether to apply dispersions.*/
  double position_dispersion[3]; /* (m) Input values by which the initial
                                        position vector is to be dispersed */
  double velocity_dispersion[3]; /* (m/s) Input values by which the initial
                                        velocity vector is to be dispersed */


 protected: // working variables
  double position_dispersion_inertial[3]; /* (m) position_dispersion values
                                                 transformed to inertial
                                                 reference frame.*/
  double velocity_dispersion_inertial[3]; /* (m/s) velocity_dispersion values
                                                 transformed to inertial
                                                 reference frame.*/

 public:
  explicit MonteCarloStateDispersion(jeod::DynBodyInitTransState & trans_init);
  virtual ~MonteCarloStateDispersion(){};

  void disperse_trans_state();

 private:
  // Copy constructor, operator= not implemented.
  MonteCarloStateDispersion (const MonteCarloStateDispersion&);
  MonteCarloStateDispersion & operator = (const MonteCarloStateDispersion&);
};
#endif
