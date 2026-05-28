/*******************************************************************************
PURPOSE:
   (Provide the coordinate axes of a reference frame defined by 2 vectors and
   used in multiple places in the TargetRelative_StateParameter class)

LIBRARY DEPENDENCIES:
   (
    (../src/TR_parameter_ref_frame.cc)
   )

PROGRAMMERS:
   (
    ((Jeremy Rea) (NASA) (May 2017) (Initial implementation))
    ((Gary Turner) (OSR) (March 2021) (V&V))
   )

*******************************************************************************/
#ifndef CML_TARGET_RELATIVE_PARAMETERS_REFFRAME_HH
#define CML_TARGET_RELATIVE_PARAMETERS_REFFRAME_HH

#include <string>
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
TR_Parameter_RefFrame
Purpose:A class that specifies the 3 basis vectors of a frame defined by 2
vectors, A and B:
u_hat : A
n_hat: A x B
p_hat: n_hat x A (tangent at A to the great circle containing A and B, in
       the direction of B)
All vectors are expressed in the planet-centered-planet-fixed (PCPF) frame.
*****************************************************************************/
class TR_Parameter_RefFrame {
  public:
    double u_hat[3]; /* (--) unit-vector defining frame axis.*/
    double n_hat[3]; /* (--) unit-vector defining frame axis.*/
    double p_hat[3]; /* (--) unit-vector defining frame axis.*/

  protected:
    std::string name; /* (--)
      name of instance for error reporting.*/
    const double cos_0_01_deg = 0.999999984769129; /* (--)
      Alignment-tolerance is defined by minimum range angle of 0.01 deg
      i.e: cos( 0.01 deg) = 0.999999984769129*/


  public:
    explicit TR_Parameter_RefFrame(std::string name);
    virtual ~TR_Parameter_RefFrame() {};

    bool compute_frame( const double vec_A[3],
                        const double vec_B[3]);
};
#endif
