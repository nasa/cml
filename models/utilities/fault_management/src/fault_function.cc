/*############################################################################
PURPOSE:
  (Function style faults)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/

#include "../include/fault_function.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils


/*****************************************************************************
Constructor
*****************************************************************************/
FaultFunctionBase::FaultFunctionBase() :
  type(Linear),
  frequency(),
  amplitude(),
  phase_offset(),
  freq_int(0.0),
  ind_prev(0.0),
  freq_prev(0.0)
{}


/*******************************************************************************
initialize
Purpose:(Initializes the fault.)
*******************************************************************************/
void FaultFunctionBase::initialize() {
  if (!ind_variable.is_initialized()) {
    CMLMessage::error(__FILE__, __LINE__,
      "Fault Management Error\n",
      "The function fault <", name, "> has not been initialized with an independent "
      "variable.\n");
  } else {
    Fault::initialize();
  }
}


/*******************************************************************************
reset
Purpose:(Resets all the independent variables and
         initiatializes any internally-integrated values.)
*******************************************************************************/
void FaultFunctionBase::reset() {
  // Note -- if the frequency, amplitude, phase_offset components do not have
  // a dependency on an independent variable, the reset of each respective
  // variable returns with no operation.
  ind_variable.reset();
  frequency.ind_variable.reset();
  amplitude.ind_variable.reset();
  phase_offset.ind_variable.reset();

  ind_prev = ind_variable.get_value();
  freq_prev = frequency.get_value();
  freq_int = 0;

  Fault::reset();
}


/*******************************************************************************
set_param
Purpose:(Generic method for setting fault parameters. For this type of fault,
         can be used to set function parameters.)
*******************************************************************************/
bool FaultFunctionBase::set_param( std::string param_name,
                                   double      value,
                                   bool        modify_nominal_with_rate)
{
  if (param_name.compare("initial") == 0 ||
      param_name.compare("nominal") == 0 ) {
    nominal = value;
  } else if (param_name.compare("rate") == 0) {
    if (modify_nominal_with_rate) {
      reset_nominal_with_new_rate( value);
    }
    rate = value;
  } else if (param_name.compare("frequency") == 0) {
    frequency.nominal = value;
  } else if (param_name.compare("amplitude") == 0) {
    amplitude.nominal = value;
  } else if (param_name.compare("phase_offset") == 0) {
    phase_offset.nominal = value;
  } else if (param_name.compare("frequency_rate") == 0) {
    if (modify_nominal_with_rate) {
      frequency.reset_nominal_with_new_rate( value);
    }
    frequency.rate = value;
  } else if (param_name.compare("amplitude_rate") == 0) {
    if (modify_nominal_with_rate) {
      amplitude.reset_nominal_with_new_rate( value);
    }
    amplitude.rate = value;
  } else if (param_name.compare("phase_offset_rate") == 0) {
    if (modify_nominal_with_rate) {
      phase_offset.reset_nominal_with_new_rate( value);
    }
    phase_offset.rate = value;
  } else {
    return Fault::set_param(param_name, value);
  }

  return true;
}