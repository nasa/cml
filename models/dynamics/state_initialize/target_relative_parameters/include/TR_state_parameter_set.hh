/*******************************************************************************
PURPOSE:
   (Define the 7 variables of the target-relative parameter set for use in
   either of the TargetRelativeStateParameter implementations.)

PROGRAMMERS:
   (
    ((Jeremy Rea) (NASA) (May 2017) (Initial implementation))
   )

*******************************************************************************/
#ifndef CML_TR_STATE_PARAMETER_SET_HH
#define CML_TR_STATE_PARAMETER_SET_HH

/*****************************************************************************
TR_ParameterSet
Purpose:A POD class providing the 7 target-relative parameters.
*****************************************************************************/
class TR_ParameterSet {
 public:
  double  altitude {0.0};  /* (m)     Altitude                                    */
  double  theta_Rng {0.0}; /* (rad)   Position range angle from target            */
  double  theta_Rot {0.0}; /* (rad)   Position rotation angle about target vector */
  double  phi_Cross {0.0}; /* (rad)   Position cross angle                        */
  double  Vmag {0.0};      /* (m/s)   Inertial velocity magnitude                 */
  double  gamma {0.0};     /* (rad)   Inertial topocentric flight path angle      */
  double  Lambda {0.0};    /* (rad)   Inertial lateral angle                      */

  TR_ParameterSet() = default;

  void zero_params() {
    *this = TR_ParameterSet{};
  }

};
#endif
