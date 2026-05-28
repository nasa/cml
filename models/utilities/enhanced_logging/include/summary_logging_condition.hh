/*******************************TRICK HEADER******************************
PURPOSE: (
  A EnhancedLogging_SummaryCondition represents an extension to the concept
  of a compound-event -- see CML compound-event model.
  The event identifies whether the arming and action conditions have been
  satisfied.
  This extension provides additional information on how the logging variables
  should proceed when the event conditions have been satisfied.

  For example, when altitude < ceiling, record the maximum temperature
  and pressure:
    - the event would monitor the event-condition, altitude < ceiling
    - this extension would identify that the maximum value is to be recorded
      for as long as that event-condition is satisfied.
    - a summary-logging-group would add the variables (pressure and temperature)
      and a condition monitoring the event with a MAX configuration.
    - The two EnhancedLogging_SummaryVariables will each have a data slot
      to record their values in response to this condition.
    - At execution, the condition will be updated, which will evaluate the
      event: recognizing that altitude < ceiling, the event will be marked
      as triggered, then the variables will compare their current values
      against the recorded values in their memory slot associated with this
      condition. If the current values exceed the recorded values, the
      recorded values will be overwritten.
  )

LIBRARY DEPENDENCY:
 ((../src/summary_logging_condition.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
**********************************************************************/
#ifndef CML_ENHANCED_LOGGING_SUMMARY_CONDITION_HH
#define CML_ENHANCED_LOGGING_SUMMARY_CONDITION_HH

#include "cml/models/vehicle_management/compound_events/include/compound_event.hh"
/*****************************************************************************
EnhancedLogging_SummaryConditionBase
Purpose: Base class implementation of a logging-condition.
Notes:
-  Baseline response is that while a group is active, its active
   basic-conditions will be processing the group's data.
-  More complex responses -- e.g. process only when some flag is flipped, or
   when some event is satisfied -- are provided by the derived classes below.
*****************************************************************************/
class EnhancedLogging_SummaryConditionBase
{
 public:
  enum LoggingSpecification {
   LATEST =0,  /* Records/overwrites the value when conditions are satisfied,
                  resulting in logging the last value for which the conditions 
                  were satisfied. To be used with e.g. MAX_CONDITIONAL to get a
                  time-consistent snapshot of when a logging condition was at
                  its sim-global maximum. */
   SINGLE =1,  /* Record the value that is FIRST encountered when the conditions
                  to evaluate the recorded values are FIRST satisfied.*/
   DISCRETE=1, /* Alias of SINGLE */
   MAX,        /* Record the maximum value that each variable acquires
                  while the group is active. Generates a single
                  set of values that are not time-consistent
                  with each other.*/
   MIN,        /* See MAX, but records the minimum value.*/
   ABS_MAX,    /* See MAX, but records the value with maximum
                  absolute value.*/
   ABS_MIN,    /* See ABS_MAX, but for minimum. */
   ABS_MAX_RECORD_ABS,    /* See ABS_MAX, but records the absolute-value not
                  the actual-value.*/
   ABS_MIN_RECORD_ABS,    /* See ABS_MAX_RECORD_ABS, but for minimum. */
   AVG,        /* Maintains a running average value that each variable acquires
                  while the condition is satisfied. The values recorded
                  may not be representative of any acquired value.*/
   ABS_AVG,    /* See AVG, but averages the absolute value
                  of the variable's values.*/
   NZ_AVG      /* See AVG; zero values are excluded from the
                  averaging.*/
  } record_type; /* (--)
    Specification of what the recorded values represent for the data slot
    associated with this condition.*/

  bool initialized; /* (--)
    Flag indicating the condition's initialize() method has been run.*/
  std::string name; /* (--)
    Name of the conditional, used for tracking which data got recorded
    in response to which conditions.*/

  EnhancedLogging_SummaryConditionBase();
  virtual ~EnhancedLogging_SummaryConditionBase(){};

  virtual void initialize();

  virtual bool update();
};

/*****************************************************************************
EnhancedLogging_SummaryConditionBool
Purpose:
  A type of EnhancedLogging_SummaryCondition that houses a reference only to
  the boolean event_triggered flag housed by a compound event somewhere.
  This, the simplest of the EnhancedLogging_SummaryCondition types simply uses
  the output of an event that is defined and executed elsewhere.
*****************************************************************************/
class EnhancedLogging_SummaryConditionBool :
                                   public EnhancedLogging_SummaryConditionBase
{
 protected:
  const bool & event_triggered; /* (--)
    A reference to the output of an event defined and updated from elsewhere.*/

 public:
  explicit EnhancedLogging_SummaryConditionBool(const bool & event_triggered_);
  virtual ~EnhancedLogging_SummaryConditionBool(){};
  bool update() override;

 private: // copy-construcotr, operator= not implemented and private.
  EnhancedLogging_SummaryConditionBool(
                                const EnhancedLogging_SummaryConditionBool &);
  EnhancedLogging_SummaryConditionBool& operator=(
                                const EnhancedLogging_SummaryConditionBool &);
};

/*****************************************************************************
EnhancedLogging_SummaryConditionEvent
Purpose:
  A type of EnhancedLogging_SummaryCondition that houses a compound event
  uniquely configurable to the conditions needed.
*****************************************************************************/
class EnhancedLogging_SummaryConditionEvent :
                                    public EnhancedLogging_SummaryConditionBase
{
 public:
  CompoundEvent event; /* (--)
    An actual event tied specifically to this condition. This event can be
    configured uniquely to meet the specific requirements associated with
    this condition.*/

  explicit EnhancedLogging_SummaryConditionEvent(const double & dyn_time);
  virtual ~EnhancedLogging_SummaryConditionEvent(){};
  void initialize() override;
  bool update() override;
 private: // copy-construcotr, operator= not implemented and private.
  EnhancedLogging_SummaryConditionEvent(
                                const EnhancedLogging_SummaryConditionEvent &);
  EnhancedLogging_SummaryConditionEvent& operator=(
                                const EnhancedLogging_SummaryConditionEvent &);
};


/*****************************************************************************
EnhancedLogging_SummaryConditionEventRef
Purpose:
  A type of EnhancedLogging_SummaryCondition that houses a reference to a compound
  event defined elsewhere. This type of configuration supports
  batch-configuration of multiple events and assignment out to
  logging-conditions. This logging-conditional is still responsible for
  evaluating the status of the compound-event, although that might be
  unnecessary if the event is used by multiple conditionals.
*****************************************************************************/
class EnhancedLogging_SummaryConditionEventRef :
                                   public EnhancedLogging_SummaryConditionBase
{
 protected:
  CompoundEvent & event; /* (--)
    An actual event tied specifically to this condition. This event can be
    configured uniquely to meet the specific requirements associated with
    this condition.*/
 public:
  explicit EnhancedLogging_SummaryConditionEventRef( CompoundEvent & event_);
  virtual ~EnhancedLogging_SummaryConditionEventRef(){};
  void initialize() override;
  bool update() override;
 private: // copy-constructor, operator= not implemented and private.
  EnhancedLogging_SummaryConditionEventRef(
                            const EnhancedLogging_SummaryConditionEventRef &);
  EnhancedLogging_SummaryConditionEventRef& operator=(
                            const EnhancedLogging_SummaryConditionEventRef &);
};


/*****************************************************************************
EnhancedLogging_SummaryCondition_GroupData
Purpose:
  Provides an interface between the EnhancedLogging_SummaryCondition and the
  EnhancedLogging_SummaryGroup.
  An EnhancedLogging_SummaryGroup contains a set of these instances.
  Each group-data instance provides access (pointer) to a specific
  EnhancedLogging_SummaryCondition instance, and
  information regarding how that condition has been used within that group.
*****************************************************************************/
class EnhancedLogging_SummaryCondition_GroupData
{
 friend class SummaryLoggingGroup;
 protected:
  EnhancedLogging_SummaryConditionBase * condition; /* (--)
    Polymorphic base-class reference to the condition being described in this
    instance.
    TODO Turner 2024/02
          This has to be a pointer, not a reference, because
          Trick-checkpointing of a STL-container requires an operator=
          and EnhancedLogging_SummaryGroup has a vector of instances of
          this type.
          Replace with a reference if this Trick limitation gets resolved.*/
  double first_time; /* (s)
    Value taken from the group's time variable corresponding to when the group
    first identified this condition as satisfied.*/
  double last_time; /* (s)
    Value taken from the group's time variable corresponding to when the group
    last identified this condition as satisfied.*/
  unsigned int count; /* (count)
    Count of the number of times this group has identified this
    condition as satisfied.*/
  unsigned int ID; /* (--)
    An identiifcation number, assigned by the group for the associated
    condition. This is the index within the logging-variables array of
    recorded-values which will be used to record values associated with this
    condition.*/
 public:
  bool active; /* (--)
    Controlling flag, allowing conditions to be made selectively active
    for periods in response to external events. Use of SubscriptionBase to
    manage activity seems unnecessary and undesirable because putting this
    active flag in the interface allows the activity status of each condition
    to be independently managed by each group accessing it.*/

  EnhancedLogging_SummaryCondition_GroupData (
                           EnhancedLogging_SummaryConditionBase & condition_,
                           unsigned int ID_);
  virtual ~EnhancedLogging_SummaryCondition_GroupData(){};

  void initialize();
  bool update(double time);
//  int find_condition_ID( const std::string & name);
  double get_first_time() const { return first_time;}
  double get_last_time() const { return last_time;}
  unsigned int get_count() const { return count;}
  unsigned int get_ID()  const {return ID;}
  EnhancedLogging_SummaryConditionBase::LoggingSpecification
                     get_record_type() const { return condition->record_type;}
  const std::string & get_name() const {return condition->name;}
};
#endif
