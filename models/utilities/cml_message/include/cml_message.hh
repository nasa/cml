/*******************************************************************************
Purpose:
  (An updated message-handler system; largely based on defaults developed
  in JEOD.)

Library Dependency:
 ((../src/cml_message.cc))

PYTHON_MODULE: (CMLMessage)

Programmers:
  (((Gary Turner) (OSR) (March 2023) (ANTARES) (Initial version))
   ((Nino Tarantino) (CACI) (April 2026) (CML) (Refactor for unit testing and mocking))
  )
*******************************************************************************/
#ifndef CML_MESSAGE_HANDLER_HH
#define CML_MESSAGE_HANDLER_HH

// System includes
#include <cstdio> // snprintf
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utility>

#include "jeod/models/utils/named_item/include/named_item.hh"
#include "cml/models/utilities/color_string/include/color_string.hh"

namespace CMLMessage {

enum PublishLevel {
  Fail = 0,
  Error,
  Warning,
  Status,
  Inform,
  Debug
};

// Keep these methods up here to expose them to SWIG:
void set_publish_level( PublishLevel new_level);
PublishLevel get_publish_level();

// Block everything else from SWIG.
#ifndef SWIG

// Combine arguments into a single string
template <typename... Args>
std::string collate_args(Args&&... args)
{
  std::ostringstream text;
  (text << ... << std::forward<Args>(args));
  return text.str();
}

void publish(PublishLevel msg_level,
             const std::string & file,
             int                 line,
             const std::string & text);

template<typename... Args>
void publish(PublishLevel msg_level,
             const std::string & file,
             int                 line,
             Args&&...           args)
{
  const std::string text = collate_args(std::forward<Args>(args)...);
  publish(msg_level, file, line, text);
}

// Alias methods to match with JEOD MessageHandler signatures.
template<typename... Args>
void fail( const std::string & file,
                  int         line,
                  Args&&...   args)
{
  const std::string text = collate_args(std::forward<Args>(args)...);
  publish(PublishLevel::Fail, file, line, text);
}

template<typename... Args>
void error( const std::string & file,
                   int         line,
                   Args&&...   args)
{
  const std::string text = collate_args(std::forward<Args>(args)...);
  publish(PublishLevel::Error, file, line, text);
}

template<typename... Args>
void warn( const std::string & file,
                  int         line,
                  Args&&...   args)
{
  const std::string text = collate_args(std::forward<Args>(args)...);
  publish(PublishLevel::Warning, file, line, text);
}

template<typename... Args>
void inform( const std::string & file,
                    int         line,
                    Args&&...   args)
{
  const std::string text = collate_args(std::forward<Args>(args)...);
  publish(PublishLevel::Inform, file, line, text);
}

template<typename... Args>
void debug( const std::string & file,
                   int         line,
                   Args&&...   args)
{
  const std::string text = collate_args(std::forward<Args>(args)...);
  publish(PublishLevel::Debug, file, line, text);
}

// status is different from the others, it results in a dedicated process to
// produce an abbreviated output.
template<typename... Args>
void status( Args&&...   args)
{
  // The file and line number will not be printed for a status message, so just
  // use the current file and line number.
  const std::string text = collate_args(std::forward<Args>(args)...);
  publish(PublishLevel::Status, __FILE__, __LINE__, text);
}

// Method for printing output formatted like a printf statement.
template <typename... Args>
std::string printf_fmt( const std::string & format, Args&&... args)
{
  /* Dry run using snprintf to make sure printf will produce a valid result
   * and measure its length before allocating the char[] buffer for the
   * real output. Note-- the +1 is for the null-termination character.*/
  int char_len = std::snprintf( nullptr, 0, format.c_str(), std::forward<Args>(args)...) + 1;
  if (char_len < 1) {
    return ColorString::red(" [**ERROR: invalid printf with format \""+format+
                            "\" **] ");
  }
  /* Allocate the char array to take the printf output, and write to it:
   * Use snprintf again just because it is safer, but we do not need the
   * return value this time.*/
  char* out_char = new char[char_len];
  std::snprintf(out_char, char_len, format.c_str(), std::forward<Args>(args)...);
  // assign to a STL-string, and return it.
  std::string out_str = std::string(out_char);
  delete[] out_char;
  return out_str;
}

template <typename T>
std::string set_prec( T var, int precision)
{
  std::ostringstream out;
  out << std::setprecision(precision) << var;
  return out.str();
}
#endif // SWIG

}

#endif
