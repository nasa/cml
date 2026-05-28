/*******************************************************************************
PURPOSE:
  (Utility to convert strings to various number formats.)

PROGRAMMERS:
  (((Daniel Ghan)(OSR)(October 2021)(Antares)(Initial version)))
*******************************************************************************/

#ifndef CML_CONVERT_STRING_HH
#define CML_CONVERT_STRING_HH

#include <cstdlib> // strtol, strtoul, strtof, strtod, strtoll, strtoull
#include <cstring> // strcmp
#include <string>
#include <type_traits> // is_arithmetic
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*******************************************************************************
ConvertString
Purpose:(Utility to convert strings to various formats.)
*******************************************************************************/
class ConvertString {
 public:
  template<typename T> static T convert(const std::string& str);
  template<typename T> static T convert(const char* str);
 private:
  template<typename T> static T convert_numeric(const char* str);
};


/*******************************************************************************
convert
Purpose:(Performs validity checks and converts strings into the desired format.)
*******************************************************************************/
template<typename T> T ConvertString::convert(const std::string& str) {
  return convert<T>(str.c_str());
}
template<typename T> T ConvertString::convert(const char* str) {
  if (!str) {
    CMLMessage::error(__FILE__, __LINE__, "Bad Pointer\n",
      "A null pointer was passed into ConvertString::convert.\n");
    return 0;
  }

  T out = 0;
  if (std::is_arithmetic<T>::value) {
    out = convert_numeric<T>(str);
  } else {
    // Unreachable code. The convert_numeric<T>(str) method is defined for
    // the following types: 
    // - unsigned long, float, double, long long, unsigned long long, bool
    // - default (i.e. none of the above)
    // Of these types:
    // - the specific types all pass the is_arithmetic<T> test
    // - the default calls strtol, which returns a "long"; in order to
    //   compile, there must be a natural conversion from "long" to T, but all
    //   forms of T that are legitimate conversions from "long" also pass
    //   is_arithmetic<T>.
    // So to get here, T must be non-arithmetic => the default
    // convert_numeric<T> must be compiled => there is a legal conversion from
    // the resulting long to T => T is arithmetic, which is a contradiction.
    // Result is that this else block is never even compiled, and will not be
    // unless somebody finds a data type that can be converted from long and
    // is non-arithmetic, or somebody redefines strtol to return a value that
    // can be converted into a non-arithmetic T.
    CMLMessage::error(__FILE__, __LINE__, "Invalid Type\n",
      "ConvertString::convert only supports numeric types.\n");
  }

  return out;
}


/*******************************************************************************
convert_numeric
Purpose:(Converts strings into the desired numerical format.)
*******************************************************************************/
// Default: use strtol (string to long integer). This should cover all the
// shorter integer types (char, short, int).
template <typename T> inline
T ConvertString::convert_numeric(const char* str) {
  return strtol(str, nullptr, 10);
}
template <> inline
unsigned long ConvertString::convert_numeric<unsigned long>(const char* str) {
  return strtoul(str, nullptr, 10);
}
template <> inline
float ConvertString::convert_numeric<float>(const char* str) {
  return strtof(str, nullptr);
}
template <> inline
double ConvertString::convert_numeric<double>(const char* str) {
  return strtod(str, nullptr);
}
template <> inline
long long ConvertString::convert_numeric<long long>(const char* str) {
  return strtoll(str, nullptr, 10);
}
template <> inline
unsigned long long ConvertString::convert_numeric<unsigned long long>(const char* str) {
  return strtoull(str, nullptr, 10);
}
template <> inline
bool ConvertString::convert_numeric<bool>(const char* str) {
  if (strcmp(str, "true") == 0 || strcmp(str, "True") == 0 ||
      strcmp(str, "TRUE") == 0)
  {
    return true;
  }
  // otherwise return false, but first check for comparison against "false"
  // and drop an error message if neither "true" nor "false" is identified.
  else if (!(strcmp(str, "false") == 0 || strcmp(str, "False") == 0 ||
               strcmp(str, "FALSE") == 0))
  {
    CMLMessage::error(
      __FILE__, __LINE__, "Unrecognized Boolean String\n",
      "The string ", str, " did not match \"true\" or \"false\" or equivalent.\n"
      "Conversion defaulting to false.\n");
  }

  return false;
}

#endif