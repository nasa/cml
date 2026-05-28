/*############################################################################
PURPOSE:
  (Individual variable class)

LIBRARY DEPENDENCY:
  ((../src/independent_variable.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_FUNCTION_INDEPENDENT_VARIABLE_HH
#define CML_FAULT_FUNCTION_INDEPENDENT_VARIABLE_HH

#include "untyped_variable.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*******************************************************************************
FaultFunctionIndependentVariable
Purpose:(An independent variable for a function. Can be absolute or relative to
         a reference value taken at some specific time.)
*******************************************************************************/
class FaultFunctionIndependentVariable {
  public :
    FaultFunctionIndependentVariable();
    ~FaultFunctionIndependentVariable();

    template<typename T> void initialize(T& variable);

    void reset();

    double get_value();

    bool is_initialized() { return initialized; }

    bool relative_value; /* (--)
      If true (default), reports the value of the variable minus its reference
      value. If false, reports the value of the variable. */

  private :
    bool initialized; /* (--)
      Whether the model has been successfully initialized. */
    UntypedVariableBase* variable; /* (--) The variable being evaluated. */
    double relative_value_init; /* (--) The reference value. */

    FaultFunctionIndependentVariable(const FaultFunctionIndependentVariable&);
    FaultFunctionIndependentVariable& operator = (
                                   const FaultFunctionIndependentVariable&);
};


/*******************************************************************************
initialize
Purpose:(Provides the variable to be evaluated.)
*******************************************************************************/
template<typename T> void FaultFunctionIndependentVariable::initialize(T& var) {
  if (initialized) {
    CMLMessage::error(__FILE__, __LINE__,
      "Fault Management Error\n",
      "Tried to initialize a FaultFunctionIndependentVariable twice!");
      return;
  }
  variable = new UntypedVariable<T>(var);
  initialized = true;
}
#endif