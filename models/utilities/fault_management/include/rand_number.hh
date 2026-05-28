/*############################################################################
PURPOSE:
  (Random number generation.)

LIBRARY DEPENDENCY:
  ((../src/rand_number.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_RAND_NUMBER_HH
#define CML_FAULT_RAND_NUMBER_HH

#include <random> // std::mt19937, std::*_distribution

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

    FaultRandNumber();
    virtual ~FaultRandNumber(){}

    void initialize();

    void initialize_gaussian( double mean_value,
                              double std_dev_value,
                              unsigned long seed_value = 0);

    void initialize_flat( double lower_limit_value,
                          double upper_limit_value,
                          unsigned long seed_value = 0);

    double get_random_number();

    Distribution_enm distribution_type; /* (--)
      The type of distribution used to generate random numbers. */
    double mean; /* (--) The mean of the Gaussian distribution. */
    double std_dev; /* (--)
      The standard deviation of the Gaussian distribution. */
    double lower_limit; /* (--) The lower limit of the uniform distribution. */
    double upper_limit; /* (--) The upper limit of the uniform distribution. */
    unsigned long seed; /* (--) The seed for the random number generator. */

  private :
    bool seeded; /* (--)
      Whether the random number generator has been given a seed. */
    std::mt19937 rng; /* (--) Random number generator. */

    FaultRandNumber(const FaultRandNumber&);
    FaultRandNumber& operator = (const FaultRandNumber&);
};

#endif
