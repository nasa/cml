/*############################################################################
PURPOSE:
  (Methods for an individual variable)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/

#include "../include/independent_variable.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
FaultFunctionIndependentVariable::FaultFunctionIndependentVariable() :
  relative_value(true),
  initialized(false),
  variable(nullptr),
  relative_value_init(0.0)
{}


/*****************************************************************************
Destructor
*****************************************************************************/
FaultFunctionIndependentVariable::~FaultFunctionIndependentVariable() {
  if (variable != nullptr) {
    delete variable;
  }
}

/*****************************************************************************
reset
Purpose:(Sets the reference value when the independent variable is using a
         relative-value)
*****************************************************************************/
void FaultFunctionIndependentVariable::reset() {
  if (!initialized) {
    return; // Protects against variable being nullptr.
  }
  if (relative_value) {
    relative_value_init =  variable->get_value();
  }
}


/*******************************************************************************
get_value
Purpose:(Returns either:
         - the current value of the independent variable or
         - the difference between the current value and the initial value.
         depending on the setting of the relative_value flag.)
*******************************************************************************/
double FaultFunctionIndependentVariable::get_value() {
  if (!initialized) {
    return 0.0; // Protects against Variable being nullptr.
  }

  double absolute = variable->get_value();

  return relative_value ? absolute - relative_value_init : absolute;
}
