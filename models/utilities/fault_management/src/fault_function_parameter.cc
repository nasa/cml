/*############################################################################
PURPOSE:
  (Defines the parameters used for evaluating periodic fault-functions)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/

#include "../include/fault_function_parameter.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
FaultFunctionParameter::FaultFunctionParameter() :
  rate(0.0),
  nominal(0.0)
{}


/*******************************************************************************
get_value
Purpose:(Gets the value of the parameter.)
*******************************************************************************/
double FaultFunctionParameter::get_value() {
  return ind_variable.is_initialized() ?
    nominal + ind_variable.get_value() * rate :
    nominal;
}

/*****************************************************************************
reset_nominal_with_new_rate
Purpose:(Resets the nominal value when the rate changes to ensure continuity
         in the current value)
*****************************************************************************/
void FaultFunctionParameter::reset_nominal_with_new_rate( double new_rate) {
  // If the independent variable is not yet assigned, the function cannot have
  // started, and there is nothing to do. However, we do not need to block on
  // this scenario because get_value() returns nominal, and
  // ind_variable.get_value() returns zero under this condition, so the
  // standard evaluation leaves nominal unchanged, as would be expected.
  nominal = get_value() - new_rate * ind_variable.get_value();
}
