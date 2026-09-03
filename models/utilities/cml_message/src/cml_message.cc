/*******************************************************************************

Purpose:
  (An updated message-handler system; largely based on defaults developed
  in JEOD.)

LIBRARY DEPENDENCIES:
  ((cml/models/utilities/cml_message/src/cml_message.cc))

Programmers:
  (((Gary Turner) (OSR) (March 2023) (ANTARES) (Initial version))
   ((Nino Tarantino) (CACI) (April 2026) (CML) (Refactor for unit testing and mocking))
  )
*******************************************************************************/
#include "../include/cml_message.hh"
#include "cml/models/utilities/color_string/include/color_string.hh"

#ifdef TRICK_VER
#include "trick/exec_proto.h"
#endif

#include <iostream>
#include <string>

namespace {

CMLMessage::PublishLevel publish_level = CMLMessage::Warning;

void terminate(const std::string & file,
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

}

namespace CMLMessage {

void set_publish_level(const PublishLevel new_level)
{
  publish_level = new_level;
}

PublishLevel get_publish_level()
{
  return publish_level;
}

void publish(PublishLevel msg_level,
             const std::string & file,
             int         line,
             const std::string & text)
{
  if (msg_level > publish_level) return;

  // Colorize the string:
  std::string preamble;
  std::string message;

  switch (msg_level) {
  case Fail:
    preamble= ColorString::bright_yellow( ColorString::red_background(
                                                         "Critical Failure"));
    message = ColorString::bright_yellow( ColorString::red_background(text));
    break;
  case Error:
    preamble = ColorString::red("Non-critical Error");
    message =  ColorString::red( text);
    break;
  case Warning:
    preamble= ColorString::magenta("Anomaly Warning");
    message = ColorString::magenta( text);
    break;
  case Inform:
    preamble= ColorString::green("Informational Notice");
    message = ColorString::green( text);
    break;
  case Status:
    preamble= ColorString::green("Status Report:");
    message = ColorString::green( text);
    break;
  case Debug:
    [[fallthrough]];
  default:
    preamble= "Debug Point";
    message = text;
  };

  if (msg_level == Status) {
      std::cout << "\n" << preamble <<
                   #ifdef TRICK_VER
                   " at Trick Sim-time: " << exec_get_sim_time() <<
                   #endif
                   "\nMessage: " << message << "\n";
  } else {
      std::cout << "\n" << preamble << " detected at" <<
                   #ifdef TRICK_VER
                   "\nTrick Sim-time: " << exec_get_sim_time() <<
                   #endif
                   "\nFile: " << file <<
                   "\nLine: " << line <<
                   "\nMessage: " << message << "\n";
  }

  if (msg_level == Fail) {
    terminate( file, line, message);
  }
}

}
