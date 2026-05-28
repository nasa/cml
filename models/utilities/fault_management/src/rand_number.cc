/*############################################################################
PURPOSE:
  (Random number generation.)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/


#include "../include/rand_number.hh"
#include <ctime> // time

/*****************************************************************************
Cosntructor
*****************************************************************************/
FaultRandNumber::FaultRandNumber() :
  distribution_type(GAUSSIAN),
  mean(0.0),
  std_dev(0.0),
  lower_limit(0.0),
  upper_limit(0.0),
  seed(0),
  seeded(false)
{}


/*******************************************************************************
initialize
Purpose:(Seeds the random number generator.)
*******************************************************************************/
void FaultRandNumber::initialize()
{
  // If the seed is not provided, get it from the system time.
  if (seed == 0) {
    seed = static_cast<unsigned long>(time(nullptr));
  }
  rng.seed(seed);
  seeded = true;
}


/*******************************************************************************
initialize_gaussian
Purpose:(Sets the Gaussian distribution parameters.)
*******************************************************************************/
void FaultRandNumber::initialize_gaussian( double mean_value,
                                           double std_dev_value,
                                           unsigned long seed_value)
{
  distribution_type = GAUSSIAN;
  mean = mean_value;
  std_dev = std_dev_value;
  seed = seed_value;

  initialize();
}


/*******************************************************************************
initialize_flat
Purpose:(Sets the uniform distribution parameters.)
*******************************************************************************/
void FaultRandNumber::initialize_flat( double lower_limit_value,
                                       double upper_limit_value,
                                       unsigned long seed_value)
{
  distribution_type = FLAT;
  lower_limit = lower_limit_value;
  upper_limit = upper_limit_value;
  seed = seed_value;

  initialize();
}


/*******************************************************************************
get_random_number
Purpose:(Returns a random number.)
*******************************************************************************/
double FaultRandNumber::get_random_number()
{
  if (!seeded) {
    initialize();
  }

  double rand_number = 0.0;
  switch (distribution_type) {
    case GAUSSIAN: {
      std::normal_distribution<double> rand_norm( mean,
                                                  std_dev);
      rand_number = rand_norm(rng);
      break;
    }
    case FLAT: {
      std::uniform_real_distribution<double> rand_flat( upper_limit,
                                                        lower_limit);
      rand_number = rand_flat(rng);
      break;
    }
    // Default (should never happen): rand_number remains at 0.
  }

  return rand_number;
}
