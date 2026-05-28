/*******************************************************************************
Purpose:
  (An updated message-handler system; largely based on defaults developed
  in JEOD.)

Library Dependency:
 ((../src/cml_message.cc))

Programmers:
  (((Gary Turner) (OSR) (March 2023) (ANTARES) (Initial version))
  )
*******************************************************************************/
#ifndef CML_MESSAGE_HANDLER_HH
#define CML_MESSAGE_HANDLER_HH

// System includes
#include <string>
#include <array>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "jeod/models/utils/named_item/include/named_item.hh"
#include "cml/models/utilities/color_string/include/color_string.hh"

#ifdef TRICK_VER
#include "trick/exec_proto.h"
#endif


class CMLMessage
{
 public:
  enum PublishLevel {
    Fail = 0,
    Error,
    Warning,
    Status,
    Inform,
    Debug
  };

  // Keep this method up here to expose it to SWIG:
  static void set_publish_level( PublishLevel new_level)
  {
    publish_level = new_level;
  }

  // Block everything else from SWIG.
 #ifndef SWIG
 protected:
  static PublishLevel publish_level;


  // Methods:
 public:
  CMLMessage (){};
  virtual ~CMLMessage(){};

  template<typename T>
  static const T* null_check( const T * in) {
    return ((in)? in : "NULL");
  }

  template<typename... Args>
  static void publish( PublishLevel msg_level,
                       const std::string & file,
                       int         line,
                       Args...     args)
  {
    // If message level has higher enumeration than the level to be published,
    // omit this message.
    if (msg_level > publish_level) return;

    std::ostringstream collate_args;
    // Use a dummy int array to parse through the varargs.
    int dummy_array[]{0, ((void)(collate_args << args),0) ...};
    // the dummy array does not get used. Cast it to void to avoid "unused
    // variable" warnings.
    static_cast<void> (dummy_array);

    // Colorize the string:
    std::string preamble;
    std::string message;

    switch (msg_level) {
    case Fail:
      preamble= ColorString::bright_yellow( ColorString::red_background(
                                                           "Critical Failure"));
      message = ColorString::bright_yellow( ColorString::red_background(
                                                           collate_args.str()));
      break;
    case Error:
      preamble = ColorString::red("Non-critical Error");
      message =  ColorString::red( collate_args.str());
      break;
    case Warning:
      preamble= ColorString::magenta("Anomaly Warning");
      message = ColorString::magenta( collate_args.str());
      break;
    case Inform:
      preamble= ColorString::green("Informational Notice");
      message = ColorString::green( collate_args.str());
      break;
    default:
      preamble= "Debug Point";
      message = collate_args.str();
    };

    std::cout << "\n" << preamble << " detected at" <<
                 #ifdef TRICK_VER
                 "\nTrick Sim-time: " << exec_get_sim_time() <<
                 #endif
                 "\nFile: " << file <<
                 "\nLine: " << line <<
                 "\nMessage: " << message << "\n";
    if (msg_level == Fail) {
      terminate( file, line, message);
    }
  }

  // Alias methods to match with JEOD MessageHandler signatures.
  template<typename... Args>
  static void fail( const std::string & file,
                    int         line,
                    Args...     args)
  {
    publish(PublishLevel::Fail, file, line, args...);
  }

  template<typename... Args>
  static void error( const std::string & file,
                     int         line,
                     Args...     args)
  {
    publish(PublishLevel::Error, file, line, args...);
  }

  template<typename... Args>
  static void warn( const std::string & file,
                    int         line,
                    Args...     args)
  {
    publish(PublishLevel::Warning, file, line, args...);
  }

  template<typename... Args>
  static void inform( const std::string & file,
                      int         line,
                      Args...     args)
  {
    publish(PublishLevel::Inform, file, line, args...);
  }

  template<typename... Args>
  static void debug( const std::string & file,
                     int         line,
                     Args...     args)
  {
    publish(PublishLevel::Debug, file, line, args...);
  }

  // status is different from the others, it results in a dedicated process to
  // produce an abbreviated output.
  template<typename... Args>
  static void status( Args...     args)
  {
    /* If publishing-level is restricted to lower level (more severe messages)
     * than Status level, omit this message.*/
    if (publish_level < Status) return;

    std::ostringstream collate_args;
    // Use a dummy int array to parse through the varargs.
    int dummy_array[]{0, ((void)(collate_args << args),0) ...};
    // the dummy array does not get used. Cast it to void to avoid "unused
    // variable" warnings.
    static_cast<void> (dummy_array);

    std::cout << "\n" << ColorString::green("Status Report:") <<
                 #ifdef TRICK_VER
                 " at Trick Sim-time: " << exec_get_sim_time() <<
                 #endif
                 "\nMessage: " << ColorString::green( collate_args.str()) << "\n";
  }

  // Method for printing output formatted like a printf statement.
  template <typename ... Args>
  static std::string printf_fmt( std::string format, Args... args)
  {
    /* Dry run using snprintf to make sure printf will produce a valid result
     * and measure its length before allocating the char[] buffer for the
     * real output. Note-- the +1 is for the null-termination character.*/
    int char_len = std::snprintf( nullptr, 0, format.c_str(), args...) + 1;
    if (char_len < 1) {
      return ColorString::red(" [**ERROR: invalid printf with format \""+format+
                              "\" **] ");
    }
    /* Allocate the char array to take the printf output, and write to it:
     * Use snprintf again just because it is safer, but we do not need the
     * return value this time.*/
    char* out_char = new char[char_len];
    std::snprintf(out_char, char_len, format.c_str(), args...);
    // assign to a STL-string, and return it.
    std::string out_str = std::string(out_char);
    delete[] out_char;
    return out_str;
  }

  template <typename T>
  static std::string set_prec( T var, int precision)
  {
    std::ostringstream out;
    out << std::setprecision(precision) << var;
    return out.str();
  }

 protected:
  static void terminate( const std::string & file,
                         int                 line,
                         const std::string & msg)
  {
    #ifdef TRICK_VER
    exec_terminate_with_return(1, file.c_str(), line, msg.c_str());
    #endif
    // Unreachable in a Trick environment because sim has terminated.
    std::cout << ColorString::red(
       "No instruction provided for terminating on a critical fault") << "\n";
  }
 #endif // End of ifndef SWIG
 private:
  // The copy constructor and assignment operator for this class are declared
  // private and are not implemented.
  CMLMessage (const CMLMessage &);
  CMLMessage & operator= (const CMLMessage &);
};

#ifndef SWIG
inline std::ostream& operator<<(std::ostream& os, const jeod::NamedItem& item) {
    return os << item.get_name();
}
#endif
#endif
