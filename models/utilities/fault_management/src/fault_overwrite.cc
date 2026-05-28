/*############################################################################
PURPOSE:
  (Manages injection of a fixed-value fault.)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#include "../include/fault_overwrite.hh"

/*****************************************************************************
Name: set_param
Purpose:
  Template specialization of the class-template method set_param:
  - T=bool : instead of setting the faulted value to the specified double,
      or the static-cast of double-to-bool, set to:
      - true for all values > 0
      - false for all values <= 0
      Note -- this supports passing in bool arguments which will be
              cast to 0.0 and 1.0
*****************************************************************************/
template<>
bool FaultOverwrite<bool>::set_param( std::string param_name,
                                      double val_,
                                      bool)
{
  if (param_name == "value") {
    faulted_value = (val_ > 0);
    return true;
  }
  // else
  return Fault::set_param(param_name, val_);
}
