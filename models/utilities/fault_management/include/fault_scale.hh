/*############################################################################
PURPOSE:
  (Manages injection of a scale-factor fault.)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_SCALE_HH
#define CML_FAULT_SCALE_HH

#include "fault.hh"

/*******************************************************************************
FaultScale
Purpose:(Manages injection of a scale-factor fault.)
*******************************************************************************/
template<typename T> class FaultScale : public Fault {
  public :
    explicit FaultScale(T& var);
    virtual ~FaultScale(){}

    void overwrite_value() override;

    bool set_param(std::string param_name, double value, bool) override;

    T scale_factor; /* (--)
      The scale value to apply to the variable when it's faulted. */

  private:
    T& variable; /* (--) The variable to fault. */

    FaultScale(const FaultScale&);
    FaultScale& operator = (const FaultScale&);
};


template<typename T>
FaultScale<T>::FaultScale(T& var) :
  scale_factor(0.0),
  variable(var)
{}


/*******************************************************************************
overwrite_value
Purpose:(Injects the fault.)
*******************************************************************************/
template<typename T> void FaultScale<T>::overwrite_value() {
  variable *= scale_factor;
}


/*******************************************************************************
set_param
Purpose:(Generic method for setting parameters. For this type of fault, can be
         used to set the scale value.)
*******************************************************************************/
template<typename T>
bool FaultScale<T>::set_param(std::string param_name, double value, bool) {
  if (param_name.compare("scale_factor") == 0) {
    scale_factor = value;
  } else {
    return Fault::set_param(param_name, value);
  }

  return true;
}

#endif
