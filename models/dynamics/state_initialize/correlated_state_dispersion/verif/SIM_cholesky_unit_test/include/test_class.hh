/********************************* TRICK HEADER *******************************
PURPOSE: (Tests the Cholesky algorithm)

PROGRAMMERS:
    (((Gary Turner) (OSR) (Initial Implementation))
     ((Dan Jordan ) (Jacobs) (11/2017) (Made test use common cholesky call)))
******************************************************************************/

#ifndef SIM_CHOLESKY_UNIT_TEST_TEST_CLASS
#define SIM_CHOLESKY_UNIT_TEST_TEST_CLASS

#include <cmath> //sqrt

#include "../../../include/correlated_state_dispersion.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

class TestCorrelatedStateDispersion : public CorrelatedStateDispersion  {
 public:
  double reconstruct[9][9];
  double delta[9][9];

  TestCorrelatedStateDispersion()
    :
    reconstruct()
    {
        // This is usually managed internally before cholesky_decomposition
        // runs, we must set it here for testing
        CorrelatedStateDispersion::dimension = 9;
    };

  // Public interface to protected method
  void cholesky_decomposition() {
      MathUtils::cholesky_decomposition( "test",
                                          &covariance[0][0],
                                          &sqrt_covariance[0][0],
                                          9);
      return;
  };
  void construct_covariance() {
    for (unsigned int jj = 0; jj < dimension; jj++) {
       for (unsigned int kk = 0; kk <= jj; kk++) {
        covariance[jj][kk] = ((kk+1) * 1.0) / ((jj+1) * 1.0);
       }
    }
  };

  void difference()
  {
    for (unsigned int ii = 0; ii < dimension; ii++) {
      for (unsigned int jj = 0; jj <= ii; jj++) {
        delta[ii][jj] = reconstruct[ii][jj] - covariance[ii][jj];
      }
    }
  };

  void reconstruct_mx()
  {
     for (unsigned int ii = 0; ii < dimension; ii++) {
        for (unsigned int jj = 0; jj <= ii; jj++) {
         reconstruct[ii][jj] = 0.0;
         for (unsigned int kk = 0; kk <= ii; kk++) {
           reconstruct[ii][jj]  += sqrt_covariance[ii][kk] *
                                   sqrt_covariance[jj][kk];
         }
       }
     }
  };

  private:
    TestCorrelatedStateDispersion (const TestCorrelatedStateDispersion&);
    TestCorrelatedStateDispersion & operator = (const TestCorrelatedStateDispersion&);
};
#endif
