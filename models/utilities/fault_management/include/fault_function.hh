/*############################################################################
PURPOSE:
  (Manages injection of a function-based fault.)

LIBRARY DEPENDENCY:
  ((../src/fault_function.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_FUNCTION_HH
#define CML_FAULT_FUNCTION_HH

#define _USE_MATH_DEFINES // for M_PI
#include <cmath> // M_PI
#include "fault.hh"
#include "fault_function_parameter.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*******************************************************************************
FaultFunctionBase
Purpose:(Allows access to most members of FaultFunction without knowing the
         type of the variable being faulted.)
*******************************************************************************/
class FaultFunctionBase : public Fault, public FaultFunctionParameter {
  public:
    enum FunctionType {
      Linear,
      Sinewave,
      Squarewave,
      Trianglewave
    };

    FaultFunctionBase();
    virtual ~FaultFunctionBase(){}

    void initialize() override;
    void reset() override;
    virtual void overwrite_value() override = 0;
    bool set_param(std::string param_name,
                   double value,
                   bool modify_nominal_with_rate = false);

    FunctionType type; /* (--)
      The type of function to add to the fault variable. */

    // Periodic function parameters
    FaultFunctionParameter frequency; /* (--)
      The frequency of the periodic functions as a linear function of some
      independent variable. Note: the unit of the value returned by
      get_value() is the inverse of the unit of the independent variable. */
    FaultFunctionParameter amplitude; /* (--)
      The amplitude of the periodic functions as a linear function of some
      independent variable. Note: the unit of the value returned by get_value()
      is the same as the unit of the fault variable. */
    FaultFunctionParameter phase_offset; /* (--)
      The phase offset of the periodic functions as a linear function of some
      independent variable. Note: the model treats phase such that the model
      is cyclic with a phase shift of 1.0. The phase-offset represents the
      fraction of a period, not a measure of radians or degrees.*/

  protected:
    double freq_int; /* (--)
      Double the integral of the frequency with respect to the independent
      variable. Used internally as part of the independent variable of periodic
      functions. */
    double ind_prev; /* (--) The previous value of the independent variable. */
    double freq_prev; /* (--) The previous value of the frequency. */

  private:
    FaultFunctionBase(const FaultFunctionBase&);
    FaultFunctionBase& operator = (const FaultFunctionBase&);
};


/*******************************************************************************
FaultFunction
Purpose:(Manages injection of a function-based fault.)
*******************************************************************************/
template<typename T> class FaultFunction : public FaultFunctionBase {
  public :

    explicit FaultFunction(T& var) : variable(var) {}
    virtual ~FaultFunction(){};

    void overwrite_value() override;

  private :
    T& variable; /* (--) The variable to fault. */

    FaultFunction(const FaultFunction&);
    FaultFunction& operator = (const FaultFunction&);
};



/*******************************************************************************
overwrite_value
Purpose:(Injects the fault.)
*******************************************************************************/
template<typename T> void FaultFunction<T>::overwrite_value() {
  double ind = ind_variable.get_value();
  if (type == Linear) {
    variable += nominal + rate * ind;
  } else {
    double freq = frequency.get_value();
    // phase = (phase offset) + integral(frequency d(ind_variable))
    // Use a trapezoidal approximation for integration.
    // See documentation for derivation.
    // In documentation, freq_int is represented by F_i
    freq_int += (freq + freq_prev) * (ind - ind_prev) / 2;
    ind_prev = ind;
    freq_prev = freq;

    double A = amplitude.get_value();
    double psi = phase_offset.get_value();
    double phi = psi + freq_int;
    switch (type) {
      case Sinewave:
        variable += A * std::sin(2 * M_PI * phi);
        break;
      case Squarewave:
        variable += (phi - round(phi)) < 0.0 ? -A : A;
        break;
      case Trianglewave: {
        variable += A * (4.0 * std::abs(phi + 0.25 - round( phi + 0.25)) - 1);
        break;
      }
      default:
        CMLMessage::error(__FILE__, __LINE__,
          "Fault Management Error\n",
          "Invalid FunctionType enumeration in fault <", name, "> \n"
          "Disabling the fault.\n");
        initialized = false;
    }
  }
}
#endif