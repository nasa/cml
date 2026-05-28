#ifndef MATH_UTILS_PRIVATE_HH
#define MATH_UTILS_PRIVATE_HH

#include <cassert>
#define _USE_MATH_DEFINES // M_PI
#include <cmath>   // abs, isnan, isinf
#include <algorithm> // std::max, min
#include <fenv.h>  // fp exception
#include <limits>  // std::numeric_limits::min, epsilon

#include "cml/models/utilities/cml_message/include/cml_message.hh"

class MathUtilsPrivate {

 protected:
  /***************************************************************************
  check_equal
  PURPOSE:(
   This mechanism is compatible with the IEEE standard. "ulp" means the unit
   in the last place when expressing a float-point in IEEE-754 binary format,
   and its value is 2e-52. It's kind of the concept of relative error.
   For the value of "1.0", it's actually the absolute error also.
   In the code, parameter ulp means how many ulp units to use as the gate
   value for the difference of the input compared numbers.

   For the zero value of val1 or val2:
    ulp=0.5 means only when exactly be zero, return true.
    ulp=1.0 means even when non-zero value is far from zero
        by DBL_MIN*DBL_EPSILON, return true.
    ulp=2.0 means even when non-zero value is far from zero
        by 2*DBL_MIN*DBL_EPSILON, return true.)
  ***************************************************************************/
  template <typename T>
  static bool check_equal( const T val1,
                           const T val2,
                           const T ulp)
  {
    //temporary disable fp exceptions
    const int fe_prev = fedisableexcept(FE_ALL_EXCEPT);
    assert(-1 != fe_prev);

    const T abs_val1 = std::abs(val1);
    const T abs_val2 = std::abs(val2);

    bool res = false;

    if (std::min(abs_val1, abs_val2) <= (T)(0.0)) { //for the zero case
      res = (std::max(abs_val1, abs_val2) <=
                                    ulp*std::numeric_limits<T>::min() *
                                    std::numeric_limits<T>::epsilon());
    }
    else {
     const T dist = std::abs(val1-val2);
     res = (
            (dist < ulp * std::max(abs_val1, abs_val2)
                        * std::numeric_limits<T>::epsilon()) //for normal number
           ||
            (dist <= ulp * std::numeric_limits<T>::min()
                         * std::numeric_limits<T>::epsilon()) //for subnormal number
           );
    }

    feenableexcept(fe_prev); // restore the previous settings of fp exceptions
    return res;
  }

  /*****************************************************************************
  div_fp
  PURPOSE: (Allows for the protected division of float or double values.)
  *****************************************************************************/
  template <typename T>
  static T div_fp ( const T dividend,
                    const T divisor,
                    const T failed_val,
                    const bool failed_flag)
  {
    // Temporary disable fp exceptions, storing the
    // set of previously configured exceptions.
    const int fe_prev = fedisableexcept(FE_ALL_EXCEPT);
    assert(-1 != fe_prev); // If -1, there was a failure

    T res = dividend/divisor;

    //check div-0, invalid ops and overflow
    if (std::isnan(res) || std::isinf(res)) {
      if (failed_flag) {
        CMLMessage::fail( __FILE__, __LINE__,
         "Divided by zero or overflow.\n"
          "The division of ",
          CMLMessage::printf_fmt("%8.6e", dividend), " / ",
          CMLMessage::printf_fmt("%8.6e", divisor), " is div-0 or overflow.\n"
          "Terminating execution.");
      }
      else {
        CMLMessage::warn( __FILE__, __LINE__,
          "Divided by zero or overflow.\n"
          "The division of ",
          CMLMessage::printf_fmt("%8.6e", dividend), " / ",
          CMLMessage::printf_fmt("%8.6e", divisor), " is div-0 or overflow,\n"
          " and the result is set as ", failed_val, ".");
      }
      res = failed_val;
    }

    feenableexcept(fe_prev); // restore the previous settings of fp exceptions
    return res;
  }

  /*****************************************************************************
  acos_fp
  PURPOSE: (Protected acos for a float or double value.)
  *****************************************************************************/
  template <typename T>
  static T acos_fp( const T val)
  {
    if ((T)(-1.0) >= val) {
     return M_PI;
    }
    else if ((T)(1.0) <= val) {
     return (T)(0.0);
    }

    return std::acos(val);
  }

  /*****************************************************************************
  asin_fp
  PURPOSE: (Protected asin for a float or a double value.)
  *****************************************************************************/
  template <typename T>
  static T asin_fp( const T val)
  {
    if ((T)(-1.0) >= val) {
     return -M_PI_2;
    }
    else if ((T)(1.0) <= val) {
     return M_PI_2;
    }

    return std::asin(val);
  }

  /*****************************************************************************
  sqrt_fp
  PURPOSE: (Protected square-root for a float or a double value.)
  *****************************************************************************/
  template <typename T>
  static T sqrt_fp( const T val)
  {
    if (val < 0.0) {
     CMLMessage::error(__FILE__,__LINE__,
      "Cannot take square root of a negative value.\n"
      "Returning 0.0.\n");
     return 0.0;
    }
    return std::sqrt(val);
  }

  /*****************************************************************************
  log_fp
  PURPOSE: (Protected natural log for a float or a double value.)
  *****************************************************************************/
  template <typename T>
  static T log_fp( const T val,
                   const T failed_val,
                   const bool failed_flag,
                   const bool base10)
  {
    // Temporary disable fp exceptions, storing the set of
    // previously configured exceptions.
    const int fe_prev = fedisableexcept(FE_ALL_EXCEPT);
    assert(-1 != fe_prev); // If -1, there was a failure

    T res;
    res = (base10)? std::log10(val) : std::log(val);

    if (std::isnan(res) || std::isinf(res)) {
      if (failed_flag) {
        CMLMessage::fail( __FILE__, __LINE__,
          "Log of negative number.\n"
          "The log of ", val, " is undefined.\n"
          "Terminating execution.");
       }
      else {
        CMLMessage::warn( __FILE__, __LINE__,
          "Log of negative number.\n",
          "The log of ", val, " is undefined,\n"
          "and the result is set as ", failed_val, ".");
        res = failed_val;
      }
    }

    feenableexcept(fe_prev); // restore the previous settings of fp exceptions
    return res;
  }
};
#endif //MATH_UTILS_PRIVATE_HH
