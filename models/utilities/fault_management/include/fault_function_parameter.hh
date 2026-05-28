/*############################################################################
PURPOSE:
  (Defines the parameters used for evaluating fault-functions when the end
  result is a linear function of an independent variable.)

LIBRARY DEPENDENCY:
  ((../src/fault_function_parameter.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_FUNCTION_PARAMETER_HH
#define CML_FAULT_FUNCTION_PARAMETER_HH

#include "independent_variable.hh"

/*******************************************************************************
FaultFunctionParameter
Purpose:(Returns a parameter that is a linear function of some arbitrary
         variable.)
*******************************************************************************/
class FaultFunctionParameter {

  public:
    FaultFunctionParameter();
    virtual ~FaultFunctionParameter(){};

    double get_value();
    void reset_nominal_with_new_rate(double new_rate);

    FaultFunctionIndependentVariable ind_variable; /* (--)
      The parameter's independent variable. */

    double rate; /* (--) The slope of the linear function. */
    double nominal; /* (--)
      The value of the parameter when the independent variable is 0. If the
      independent variable is relative, this is the initial value of the
      parameter. */

  private:
    FaultFunctionParameter(const FaultFunctionParameter&);
    FaultFunctionParameter& operator = (const FaultFunctionParameter&);
};
#endif
