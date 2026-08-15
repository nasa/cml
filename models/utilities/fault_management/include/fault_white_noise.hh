/*############################################################################
PURPOSE:
  (Manages injection of a white-noise fault.)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_WHITE_NOISE_HH
#define CML_FAULT_WHITE_NOISE_HH

#include "fault.hh"
#include "rand_number.hh"

/*******************************************************************************
FaultWhiteNoise
Purpose:(Manages injection of a white-noise fault.)
*******************************************************************************/
template<typename T> class FaultWhiteNoise : public Fault {
  public :
    explicit FaultWhiteNoise(T& variable);
    ~FaultWhiteNoise() override = default;

    void overwrite_value() override;

    bool set_param(std::string param_name, double value, bool b) override;

    FaultRandNumber noise; /* (--) Random number generator. */

  private:
    T& variable; /* (--) The variable to fault. */

    FaultWhiteNoise(const FaultWhiteNoise&);
    FaultWhiteNoise& operator = (const FaultWhiteNoise&);
};


template<typename T> FaultWhiteNoise<T>::FaultWhiteNoise(T& var) :
  variable(var)
{}


/*******************************************************************************
overwrite_value
Purpose:(Injects the fault.)
*******************************************************************************/
template<typename T> void FaultWhiteNoise<T>::overwrite_value() {
  variable += noise.get_random_number();
}


/*******************************************************************************
set_param
Purpose:(Generic method for setting fault parameters. For this type of fault,
         can be used to set parameters of the random number generator.)
*******************************************************************************/
template<typename T>
bool FaultWhiteNoise<T>::set_param(std::string param_name, double value, bool) {
  if (param_name == "mean") {
    noise.mean = value;
  } else if (param_name == "std_dev") {
    noise.std_dev = value;
  } else if (param_name == "min") {
    noise.lower_limit = value;
  } else if (param_name == "max") {
    noise.upper_limit = value;
  } else if (param_name == "seed") {
    noise.seed = value;
  } else {
    return Fault::set_param(param_name, value);
  }

  return true;
}

#endif
