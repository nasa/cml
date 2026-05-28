/*******************************TRICK HEADER******************************
PURPOSE: (Provides a multi-conditional event structure.)

LIBRARY DEPENDENCY:
  ((../src/watch_values_set.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2023) (Antares) (enhancement)))
**********************************************************************/
#ifndef CML_WATCH_VALUES_SET_HH
#define CML_WATCH_VALUES_SET_HH

#include <vector>
#include "watch_values_base.hh"


/*****************************************************************************
WatchValuesSet
Purpose:
  A WatchValuesSet is an event that is triggered by combining multiple other
  events. These other events are accessed via a vector of pointers.
  The flag require_all indicates whether this event triggers when ANY of the
  sub-events trigger, or whether it requires ALL of the sub-events to trigger.

Notes:
- Each of the sub-events must be individually and indepently instantiated and
  configured; trying to do this from a central location would be
  overwhelmingly complex to manage all possible behaviors.
- Making a sub-event single-shot makes it "sticky" -- once triggered it will
  remain triggered for all time. This supports the logic of
  "trigger when all of these events have occurred".
- Making a sub-event multi-shot means its status can go in and out of
  "triggered". This supports the logic of "trigger when all of these
  conditions are satisfied simultaneously".
*****************************************************************************/
class WatchValuesSet : public WatchValuesBaseCore
{
 protected:
  std::vector< WatchValuesBaseCore *> triggers;
  std::string trigger_name; /* (--)
    The name of the first trigger in the triggers vector that is satisfied
    whenever the set has been triggered. If the set is not triggered, this
    remains empty.
    Assigned only when the record_trigger_name flag is set.*/
 public:
  bool require_all; /* (--)
    Flag indicating how to interpret the status of the individual triggers.
    True  -- this event triggers when ALL triggers are satisfied.
    False -- this event triggers when ANY trigger is satsified.*/
  bool record_trigger_name; /* (--)
    An optional flag that allows the recording of which of the set of
    triggers was responsible for triggering the overall set.
    The name of the trigger is copied into trigger_name.
    If multiple triggers have been triggered, only the first one
    encountered in the triggers vector will be recorded.*/

  WatchValuesSet();
  virtual ~WatchValuesSet(){};

  void add_trigger( WatchValuesBaseCore & new_trigger);
  void initialize( std::list<WatchValuesBaseCore *> * active_watch_in) override;
  bool test_crossing() override;
  size_t get_num_triggers() {return triggers.size();}
  std::string & get_trigger_name() {return trigger_name;}

 protected:
  virtual void activate();
  virtual void deactivate();
 private:
  WatchValuesSet (const WatchValuesSet&);
  WatchValuesSet& operator = (const WatchValuesSet&);
};
#endif
