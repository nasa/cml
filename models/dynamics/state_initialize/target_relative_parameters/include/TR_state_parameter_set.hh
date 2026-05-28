/*******************************************************************************
PURPOSE:
   (Define the 7 variables of the target-relative parameter set for use in
   either of the TargetRelativeStateParameter implementations.)

PROGRAMMERS:
   (
    ((Jeremy Rea) (NASA) (May 2017) (Initial implementation))
   )

*******************************************************************************/
#ifndef CML_TARGET_RELATIVE_PARAMETERS_STATE_SET_HH
#define CML_TARGET_RELATIVE_PARAMETERS_STATE_SET_HH

/*****************************************************************************
TR_ParameterSet
Purpose:A POD class providing the 7 target-relative parameters.
*****************************************************************************/
class TR_ParameterSet {
 public:
  double  altitude;  /* (m)     Altitude                                    */
  double  theta_Rng; /* (rad)   Position range angle from target            */
  double  theta_Rot; /* (rad)   Position rotation angle about target vector */
  double  phi_Cross; /* (rad)   Position cross angle                        */
  double  Vmag;      /* (m/s)   Inertial velocity magnitude                 */
  double  gamma;     /* (rad)   Inertial topocentric flight path angle      */
  double  Lambda;    /* (rad)   Inertial lateral angle                      */

  TR_ParameterSet() { zero_params();}

  void zero_params() {
    altitude  =
    theta_Rng =
    theta_Rot =
    phi_Cross =
    Vmag      =
    gamma     =
    Lambda    = 0.0;
  }

};
#endif
