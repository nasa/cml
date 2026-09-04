/*############################################################################
PURPOSE:
  (Manages injection of a random-walk fault.)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_RANDOM_WALK_HH
#define CML_FAULT_RANDOM_WALK_HH

#include "fault.hh"
#include "rand_number.hh"

/*******************************************************************************
FaultRandomWalk
Purpose:(Manages injection of a random-walk fault.)
*******************************************************************************/
template <typename T> class FaultRandomWalk : public Fault {
  public :
    explicit FaultRandomWalk(T& var);
    ~FaultRandomWalk() override = default;
    FaultRandomWalk(const FaultRandomWalk&) = delete;
    FaultRandomWalk& operator = (const FaultRandomWalk&) = delete;

    void overwrite_value() override;

    bool set_param(const std::string& param_name, double value, bool b) override;

    void reset() override { random_walk_value = 0.0;
                            Fault::reset();}

    FaultRandNumber rand; /* (--) Random number generator. */

  private:
    T& variable; /* (--) The variable to fault. */
    T random_walk_value; /* (--)
      The value that is added to the variable when it's faulted. */
};


template<typename T>
FaultRandomWalk<T>::FaultRandomWalk(T& var) :
  variable(var),
  random_walk_value(0.0)
{}


/*******************************************************************************
overwrite_value
Purpose:(Injects the fault.)
*******************************************************************************/
template<typename T> void FaultRandomWalk<T>::overwrite_value() {
  // Get the new random walk value
  random_walk_value += rand.get_random_number();

  variable += random_walk_value;
}


/*******************************************************************************
set_param
Purpose:(Generic method for setting fault parameters. For this type of fault,
         can be used to set parameters of the random number generator.)
*******************************************************************************/
template<typename T>
bool FaultRandomWalk<T>::set_param(const std::string& param_name, double value, bool) {
  if (param_name == "mean") {
    rand.mean = value;
  } else if (param_name == "std_dev") {
    rand.std_dev = value;
  } else if (param_name == "min") {
    rand.lower_limit = value;
  } else if (param_name == "max") {
    rand.upper_limit = value;
  } else if (param_name == "seed") {
    rand.seed = static_cast<unsigned long>(value);
  } else {
    return Fault::set_param(param_name, value);
  }

  return true;
}

#endif
