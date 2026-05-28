/*############################################################################
PURPOSE:
  (Manages injection of a fixed-value fault.)

LIBRARY DEPENDENCY:
  (../src/fault_overwrite.cc)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef FaultOverwrite_H
#define FaultOverwrite_H

#include "fault.hh"

/*******************************************************************************
FaultOverwrite
Purpose:(Manages injection of a fixed-value fault.)
*******************************************************************************/
template <typename T>
class FaultOverwrite : public Fault {
 private:
  T& variable; /* (--) Reference to the variable to fault. */
 public:
  T faulted_value; /* (--) The faulted value of the variable. */

  explicit FaultOverwrite(T& var)
    :
    variable(var)
  {}
  virtual ~FaultOverwrite(){}

  void overwrite_value() override { variable = faulted_value; }

  bool set_param(std::string param_name, double val_, bool) override;

 private:
  FaultOverwrite(const FaultOverwrite&) = delete;
  FaultOverwrite& operator = (const FaultOverwrite&) = delete;
};


/*******************************************************************************
set_param
Purpose:(Generic method for setting fault parameters. For this type of fault,
         can be used to set the faulted value.)
*******************************************************************************/
template<typename T>
bool FaultOverwrite<T>::set_param(
  std::string param_name,
  double val_,
  bool)
{
  if (param_name == "value") {
    faulted_value = val_;
    return true;
  }
  // else
  return Fault::set_param(param_name, val_);
}
/******************************************************************************/
template<>
bool FaultOverwrite<bool>::set_param( std::string param_name,
                                      double val_,
                                      bool);
#endif
