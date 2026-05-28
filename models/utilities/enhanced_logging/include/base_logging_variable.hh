/*******************************TRICK HEADER******************************
PURPOSE:
  (Provides the abstract base-class for summary-variables and
   serial-variables.
   Note -- not to be confused with EnhancedLogging_Variable, aka
           proto-logging-variable, which provides a staging platform for
           construction of summary-variables and serial-variables.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
***********************************************************************/
#ifndef CML_ENHANCED_LOGGING_BASE_VARIABLE_HH
#define CML_ENHANCED_LOGGING_BASE_VARIABLE_HH

#include <string>
#include <fstream>

#include "summary_logging_condition.hh"

/*****************************************************************************
EnhancedLogging_BaseVariable
Purpose:
  Abstract base class for the class-template EnhancedLogging_BaseVariable.
  Provides the name, alias, and units of a proto-EnhancedLogging_BaseVariable.
*****************************************************************************/
class EnhancedLogging_BaseVariable
{
 public:
  std::string alias; /* (--)
    Alias of the variable being logged, this is the name that will be
    associated with the data in the output file.*/
  std::string units; /* (--)
    Units associated with the variable being logged.*/
  std::string name; /* (--)
    Name of the variable being logged.
    This is typically the address of the sim variable, but depending on
    construction method may be just a copy of the alias.
    It is used for messaging/debugging purposes only*/
  int field_width; /* (--)
    When outputting formatted CSV, this is the number of columns to leave to
    accommodate the output of a variable of type T.*/

  EnhancedLogging_BaseVariable( std::string alias_,
                                std::string units_ = "",
                                std::string name_ = "")
    :
    alias( std::move(alias_)),
    units( std::move(units_)),
    name(  std::move(name_)),
    field_width(1)
  {
    if (units.empty()) { units = "--";}
    if (name.empty()) { name = alias;}
  }

  virtual ~EnhancedLogging_BaseVariable(){};

  virtual void initialize(){};

  virtual void log_value(std::ofstream&) const = 0;
  virtual void log_value_fmt(std::ofstream&) const = 0;

  // Simple method to distinguish between a serial-variable (default) and a
  // summary-variable
  virtual bool is_summary() {return false;}

  // These methods only makes sense for a summary-variable, but are
  // accessed from the base-variable pointer.
  virtual void initialize(size_t) {initialize();}
  virtual void log_value(size_t, std::ofstream&) const = 0;
  virtual void log_value_fmt(size_t, std::ofstream& stream) const = 0;
  virtual void populate_condition( size_t,
                  EnhancedLogging_SummaryConditionBase::LoggingSpecification){};
  virtual void reset_summary_val(size_t){};
};
#endif
