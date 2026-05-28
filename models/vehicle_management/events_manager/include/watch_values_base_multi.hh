/*******************************TRICK HEADER******************************
PURPOSE: (Provides a 2-condition event structure.
          For more than 2 conditionals, use a WatchValuesSet)

PROGRAMMERS:
  (((Gary Turner) (OSR) (January 2020) (Antares) (enhancement)))
**********************************************************************/
#ifndef CML_WATCH_VALUES_BASE_MULTI_HH
#define CML_WATCH_VALUES_BASE_MULTI_HH

#include "watch_values_base.hh"

template <typename A, typename B>
class WatchValuesBase2 : public WatchValuesBaseCore
{
 public:
  WatchValuesBase<A> watch_a; /* (--)
    One of the two sub-events / triggers to test.*/
  WatchValuesBase<B> watch_b; /* (--)
    The other of the two sub-events / triggers to test.*/
  bool require_all; /* (--)
    Flag indicating how to combine watch_a and watch_b.
    True  -- this event triggers when watch_a AND watch_b are satisfied.
    False -- this event triggers when watch_a OR watch_b are satsified.*/

  WatchValuesBase2()
    :
    watch_a(),
    watch_b(),
    require_all(true)
  {
    // Keep both perpetually active; we don't want one deactivating while
    // waiting for the other to trigger.
    watch_a.multi_shot = true;
    watch_b.multi_shot = true;
    // Block these watch-structures from loading directly onto the
    // manager's list of active watches;
    // their only access should be through this class.
    watch_a.add_self_to_manager_active_list = false;
    watch_b.add_self_to_manager_active_list = false;
  }

/*****************************************************************************
intialize
Purpose:(Because watch_a and watch_b are not loaded directly onto the manager,
         this class must call their respective initializations )
*****************************************************************************/
  void initialize( std::list<WatchValuesBaseCore *> * active_watch_in)
  {
    watch_a.initialize( active_watch_in);
    watch_b.initialize( active_watch_in);
    WatchValuesBaseCore::initialize( active_watch_in);
  }


/*****************************************************************************
test_crossing
Purpose:(The logic to execute the individual test_crossing implementations
         and combine to give the overall result.)
*****************************************************************************/
  bool test_crossing()
  {
    bool a_triggered = watch_a.test_crossing();
    bool b_triggered = watch_b.test_crossing();
    event_triggered = (require_all && a_triggered && b_triggered) ||
                      (!require_all && (a_triggered || b_triggered));
    if (event_triggered) {
      if (!multi_shot) {
        deactivate();
      }
    }
    int_event_triggered = event_triggered ? 1 : 0;
    return event_triggered;
  }

 protected:
/*****************************************************************************
specific_execution
Purpose:( inheriting classes use this to implement their own specific
          execution.  It is called from test_crossing, which is NOT virtual.
          In the base class, nothing more needs doing.
*****************************************************************************/
  virtual bool specific_execution() {return false;};


/*****************************************************************************
activate
Purpose:(Need to activate the watch_a and watch_b instances so that their
         test_crossing methods can execute.  Note that the manager's
         active_watch list will get this class added to it, but not the
         watch_a and watch_b instances.
*****************************************************************************/
  virtual void activate()
  {
    watch_a.subscribe();
    watch_b.subscribe();
    active = true;
    active_watches->push_back(this);
  }

/*****************************************************************************
deactivate
Purpose:(Unsubscribe from watch_a and watch_b to deactivate them.
         This is important so that a subsequent activation
         will re-activate them, enabling the "relative to activation" behavior.
         Do not need to be concerned about removing this instance from the
         manager's active_watch list; it will remove it on the next pass when
         it identifies this watch as being inactive.
*****************************************************************************/
  virtual void deactivate()
  {
    watch_a.unsubscribe();
    watch_b.unsubscribe();
    active = false;
  }
 private:
  WatchValuesBase2 (const WatchValuesBase2& rhs);
  WatchValuesBase2& operator = (const WatchValuesBase2& rhs);
};
#endif

