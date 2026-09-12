/*******************************************************************************
PURPOSE:
   (Specific implementations of the WatchValuesBase base-class)

PROGRAMMERS:
   (((Gary Turner) (OSR) (October 2014) (Antares))
 ******************************************************************************/

#ifndef CML_WATCH_VALUES_SPECIFIC_HH
#define CML_WATCH_VALUES_SPECIFIC_HH

#include <list>
#include <string>
#include "cml/models/utilities/trick_logging/include/trick_logging.hh"

#include "watch_values_base.hh"
#include "event_action.hh"
// Template: copy this (6 lines)
/*****************************************************************************
class WatchValues  : public WatchValuesBase {
 public:
 protected:
  virtual bool specific_execution() override {}
}
*****************************************************************************/

// These SWIG templates are to inform SWIG about the template instantiations
// that are used.
//
// When a regular class inherits from a template, create a %template of the
// appropriate type
//    class A : public TClass<#type> needs
//      %template(name) TClass<#type>
// When a class-template inherits from a template, create a %template for the
// base template class for every unique instance of the derived template
//    template <varT> class TA : public TClass<varT> needs
//      %template(name) TClass<double>
//      %template(name) TClass<int>
//    if TA is instantiated as TA<double> and TA<int>
//
#ifdef SWIG
#ifndef CML_WATCH_VALUES_SWIG_TEMPLATES
#define CML_WATCH_VALUES_SWIG_TEMPLATES
%template(WatchValuesBase_bool) WatchValuesBase<bool>;
%template(WatchValuesBase_int) WatchValuesBase<int>;
%template(WatchValuesBase_double) WatchValuesBase<double>;
%template(WatchValuesBase_unsigned_int) WatchValuesBase<unsigned int>;
#endif
#endif

/***************************************************************************/
// Stops the sim when a particular variable condition is satisfied
template <typename varT>
class WatchValuesSimStopVar : public WatchValuesBase<varT> {
 public:
  EventActionSimStopNow event_action;
  WatchValuesSimStopVar()
  {
    this->add_action(event_action);
  }
  WatchValuesSimStopVar (const WatchValuesSimStopVar& rhs) = delete;
  WatchValuesSimStopVar& operator = (const WatchValuesSimStopVar& rhs) = delete;
};

/***************************************************************************/
//  Note - this event can monitor a list of the "translational_dynamics" flags
//  of all the vehicles, and shut the sim down when all vehicles are finished.
//  It requires that the flags be loaded onto this event's list,
//  trans_dyn_flags.
class WatchValuesSimStop : public WatchValuesBase<bool> {
 public:
  EventActionSimStopDyn event_action;
  std::list<bool*> & trans_dyn_flags;
  WatchValuesSimStop()
    :
    trans_dyn_flags( event_action.trans_dyn_flags)
  {
    this->add_action(event_action);
  }
  WatchValuesSimStop (const WatchValuesSimStop& rhs) = delete;
  WatchValuesSimStop& operator = (const WatchValuesSimStop& rhs) = delete;
};

/***************************************************************************/
// Turns off all jobs in a sim object
class WatchValuesDeactivateSimObject : public WatchValuesBase<double> {
 public:
   EventActionDeactivateSimObject event_action;
   std::string & simobj_name;
   WatchValuesDeactivateSimObject():
     simobj_name(event_action.simobj_name)
  {
    this->add_action(event_action);
  }
  WatchValuesDeactivateSimObject (const WatchValuesDeactivateSimObject& rhs) = delete;
  WatchValuesDeactivateSimObject& operator = (const WatchValuesDeactivateSimObject& rhs) = delete;
};

/***************************************************************************/
// Adjust logging rate
template <typename varT>
class WatchValuesAdjustLogging : public WatchValuesBase<varT> {
 public:
  EventActionAdjustLogging event_action;
  double & log_cycle; /* (s) New logging frequency */
  bool   & log_now;   /* (--) log and reset the logging schedule immediately.*/
  WatchValuesAdjustLogging(TrickLogging & logging_in)
    :
    event_action( logging_in),
    log_cycle(    event_action.log_cycle),
    log_now(      event_action.log_now)
  {
    this->add_action(event_action);
  }
  WatchValuesAdjustLogging (const WatchValuesAdjustLogging& rhs) = delete;
  WatchValuesAdjustLogging& operator = (const WatchValuesAdjustLogging& rhs) = delete;
};
/***************************************************************************/
// Adjust logging rate and activity for a specific group
template <typename varT>
class WatchValuesAdjustLoggingGroup : public WatchValuesBase<varT> {
 public:
  EventActionAdjustLoggingGroup event_action;
  std::string & group_name; /* (--)
    Reference to event_action.group_name.*/
  bool enable_group{false}; /* (--)
    Reference to event_action.enable_group.*/
  double & log_cycle; /* (s)
    Reference to event_action.log_cycle.*/
  bool   & log_now;   /* (--)
    Reference to event_action.log_now.*/
  WatchValuesAdjustLoggingGroup(TrickLogging & logging_in)
    :
    event_action( logging_in),
    group_name(   event_action.group_name),
    enable_group( event_action.enable_group),
    log_cycle(    event_action.log_cycle),
    log_now(      event_action.log_now)
  {
    this->add_action(event_action);
  }
  WatchValuesAdjustLoggingGroup (
                             const WatchValuesAdjustLoggingGroup& rhs) = delete;
  WatchValuesAdjustLoggingGroup& operator = (
                             const WatchValuesAdjustLoggingGroup& rhs) = delete;
};

/*************************************************************************
 Log data the instant the event triggers, before applying any
 event-induced changes.*/
template <typename varT>
class WatchValuesLogNow : public WatchValuesBase<varT> {
 public:
  WatchValuesLogNow(TrickLogging & logging_in)
     :
     logging(logging_in){}
  WatchValuesLogNow (const WatchValuesLogNow& rhs) = delete;
  WatchValuesLogNow& operator = (const WatchValuesLogNow& rhs) = delete;
 protected:
  TrickLogging & logging;
  bool specific_execution() override {logging.log_now();
                                     return false;}
};
/**************************************************************************
 Log data immediately after event modifications are applied
 in response to event triggering.
 Difference is:
 - WatchValuesLogNow overwrites the specific_execution() method to call
   log_now, so log_now() is executed before apply_complementary_changes().
 - WatchValuesLogNowPost adds the log_now() call as an event-action, so it
   is called at the end of the apply_complementary_changes() execution.
**************************************************************************/
template <typename varT>
class WatchValuesLogNowPost : public WatchValuesBase<varT> {
 public:
  EventActionLogNow event_action;
  WatchValuesLogNowPost(TrickLogging & logging_in)
    :
    event_action( logging_in)
  {
    this->add_action(event_action);
  }
  WatchValuesLogNowPost (const WatchValuesLogNowPost& rhs) = delete;
  WatchValuesLogNowPost& operator = (const WatchValuesLogNowPost& rhs) = delete;
};
#endif
