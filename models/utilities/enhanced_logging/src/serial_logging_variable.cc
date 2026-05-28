/*******************************TRICK HEADER******************************
PURPOSE: (
  Provides the class tempalte for the data-type-specific implementation
  of a serial-logging-variable.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/
#include "../include/serial_logging_variable.hh"

template<> void EnhancedLogging_SerialVariableT<std::string>::set_field_width()
  { field_width = 20;}
/* FIXME Turner 2024/06
     Setting the field-width to 20 by default. Is there a better way of
     setting it to a conditional length, appropriate to the situation?
     The problem is that this is for formatted-output, intended to present
     a uniform appearance from line-to-line, but the string-length can vary
     unpredictably. What happens if the string-length exceeds the asigned
     field-width?
*/
