/*############################################################################
PURPOSE:
  (Manages injection of a bias fault.)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_BIAS_HH
#define CML_FAULT_BIAS_HH

#include "fault.hh"

/*******************************************************************************
FaultBias
Purpose:(Manages injection of a bias fault.)
*******************************************************************************/
template<typename T> class FaultBias : public Fault {
  public :

    explicit FaultBias(T& var);
    virtual ~FaultBias(){}

    void overwrite_value() override;

    bool set_param(std::string param_name, double value, bool b) override;

    T bias; /* (--) The bias to apply when the variable is faulted. */

  private:
    T& variable; /* (--) The variable to fault. */

    FaultBias(const FaultBias&);
    FaultBias& operator = (const FaultBias&);
};


template<typename T> FaultBias<T>::FaultBias(T& var) :
  bias(0.0),
  variable(var)
{}


/*******************************************************************************
overwrite_value
Purpose:(Injects the fault.)
*******************************************************************************/
template<typename T> void FaultBias<T>::overwrite_value() {
  variable += bias;
}


/*******************************************************************************
set_param
Purpose:(Generic method for setting fault parameters. For this type of fault,
         can be used to set the bias.)
*******************************************************************************/
template<typename T>
bool FaultBias<T>::set_param(std::string param_name, double value, bool) {
  if (param_name.compare("bias") == 0) {
    bias = value;
  } else {
    return Fault::set_param(param_name, value);
  }

  return true;
}

#endif
