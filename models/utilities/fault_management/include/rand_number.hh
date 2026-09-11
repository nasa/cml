/*############################################################################
PURPOSE:
  (Random number generation.)

LIBRARY DEPENDENCY:
  ((../src/rand_number.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_RAND_NUMBER_HH
#define CML_RAND_NUMBER_HH

#include <random>

namespace cml {

/*******************************************************************************
FaultRandNumber
Purpose:(Generates random numbers using a Gaussian or uniform distribution.)
*******************************************************************************/
class FaultRandNumber {
  public :

    enum Distribution_enm {
      GAUSSIAN = 0,
      FLAT = 1
    };

    FaultRandNumber() = default;
    virtual ~FaultRandNumber() = default;
    FaultRandNumber(const FaultRandNumber&) = delete;
    FaultRandNumber& operator = (const FaultRandNumber&) = delete;

    void initialize();

    void initialize_gaussian( double mean_value,
                              double std_dev_value,
                              unsigned long seed_value = 0);

    void initialize_flat( double lower_limit_value,
                          double upper_limit_value,
                          unsigned long seed_value = 0);

    double get_random_number();

    Distribution_enm distribution_type{GAUSSIAN}; /* (--)
      The type of distribution used to generate random numbers. */
    double mean{0.0}; /* (--) The mean of the Gaussian distribution. */
    double std_dev{0.0}; /* (--)
      The standard deviation of the Gaussian distribution. */
    double lower_limit{0.0}; /* (--) The lower limit of the uniform distribution. */
    double upper_limit{0.0}; /* (--) The upper limit of the uniform distribution. */
    unsigned long seed{0}; /* (--) The seed for the random number generator. */

  private :
    bool seeded{false}; /* (--)
      Whether the random number generator has been given a seed. */
    std::mt19937 rng; /* (--) Random number generator. */
};



} // namespace cml

#endif