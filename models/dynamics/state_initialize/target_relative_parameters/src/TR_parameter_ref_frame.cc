/*******************************************************************************
PURPOSE:
   (Provide the coordinate axes of a reference frame defined by 2 vectors and
   used in multiple places in the TargetRelative_StateParameter class)

PROGRAMMERS:
   (
    ((Jeremy Rea) (NASA) (May 2017) (Initial implementation))
    ((Gary Turner) (OSR) (March 2021) (V&V))
   )

*******************************************************************************/
#include "../include/TR_parameter_ref_frame.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils::*
#include "jeod/models/utils/math/include/vector3.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
TR_Parameter_RefFrame::TR_Parameter_RefFrame(
    std::string name_)
  :
  u_hat(),
  n_hat(),
  p_hat(),
  name(name_)
{}



/*****************************************************************************
compute_frame
Purpose:
  Generates the u-hat, n_hat, p_hat vectors from the specified vectors.
  The three generated vectors form the orthonormal axes of a frame;
  returns true if the frame is well defined, false otherwise.
*****************************************************************************/
bool TR_Parameter_RefFrame::compute_frame(
    const double vec_A[3],
    const double vec_B[3])
{
  if (vec_A == NULL || vec_B == NULL) {
    CMLMessage::fail( __FILE__,__LINE__,
      "Error in generating frame ",name,"\n"
      "NULL vectors cannot be converted into target-reference frames.\n");
  }


  jeod::Vector3::normalize( vec_A,
                      u_hat);
  double vec_B_hat[3];
  jeod::Vector3::normalize( vec_B,
                      vec_B_hat);

  double cos_theta = std::fabs(jeod::Vector3::dot( u_hat, vec_B_hat));
  if (cos_theta > cos_0_01_deg) { // i.e. theta < 0.01 degrees
    CMLMessage::error( __FILE__,__LINE__,
      "Error in generating frame '",name,"'\n"
      "The two input vectors are nearly aligned.\n"
      "The rest of the frame cannot be constructed.\n"
      "u_hat is set to the shared unit-vector\n"
      "p-hat and n-hat are set to zero-vectors.\n");
    jeod::Vector3::initialize(n_hat);
    jeod::Vector3::initialize(p_hat);
    return false;
  }
  jeod::Vector3::normalize( vec_A,
                      u_hat);

  jeod::Vector3::cross( vec_A,
                  vec_B,
                  n_hat);
  jeod::Vector3::normalize( n_hat);

  jeod::Vector3::cross( n_hat,
                  u_hat,
                  p_hat);
  jeod::Vector3::normalize( p_hat);
  return true;
}
