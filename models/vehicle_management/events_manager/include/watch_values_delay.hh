/*******************************TRICK HEADER******************************
PURPOSE: (Provides an extension to  WatchValuesBase that allows a delay
          be added between the satsifaction of the trigger and the execution
          of the consequences.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (May 2023) (ANTARES) (enhancement)))
**********************************************************************/
#ifndef CML_WATCH_VALUES_BASE_DELAY_HH
#define CML_WATCH_VALUES_BASE_DELAY_HH

#include "watch_values_base.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"
#ifdef SWIG
#ifndef CML_WATCH_VALUES_SWIG_TEMPLATES
#define CML_WATCH_VALUES_SWIG_TEMPLATES
%template(WatchValuesBase_bool) WatchValuesBase<bool>;
%template(WatchValuesBase_int) WatchValuesBase<int>;
%template(WatchValuesBase_double) WatchValuesBase<double>;
%template(WatchValuesBase_unsigned_int) WatchValuesBase<unsigned int>;
#endif
#endif


template <typename watchType>
class WatchValuesDelay : public WatchValuesBase<watchType>
{
 protected:
  const double & delay_ref; /* (--)
    Reference to the variable whose value will be monitored for evolution
    AFTER the trigger condition is satisfied. */
  double delay_ref_at_trigger; /* (--)
    Value of delay_ref at the instant the trigger condition was satisfied.*/
  bool event_triggered_pending_delay; /* (--)
    Flag indicating that the conditions have been satisfied and the delay
    is in progress leading to the triggering of the event.*/
 public:
  double delay_offset; /* (--)
    Extent of delay: this is the delta-value that delay_ref must change by
    after the trigger condition is satisfied before the consequences are
    applied.*/
  bool reset_delay; /* (--)
    Flag to reset the delay, set from another watch values delay typically.*/

  WatchValuesDelay( const double & delay_ref_)
    :
    WatchValuesBase<watchType>(),
    delay_ref( delay_ref_),
    delay_ref_at_trigger(0.0),
    event_triggered_pending_delay(false),
    delay_offset(0.0),
    reset_delay(false)
  {}
 virtual ~WatchValuesDelay(){};
/*****************************************************************************
test_crossing
Purpose: tests for the satisfaction of the trigger condition and the subsequent
         delay condition
*****************************************************************************/
  virtual bool test_crossing() {
    if (!SubscriptionBase::active) {return false;}

    /* Prevent triggers from being evaluated more than once per logging cycle,
     * because that can cause unexpected behavior for types like On Change. */
    if (WatchValuesBaseCore::locked) {
      return WatchValuesBaseCore::event_triggered;
    }

    /* If we are not already monitoring a delay, test the trigger condition.
     * Note -- don't retest if we are monitoring a delay, there's no need and
     * this runs the risk of reverting the detection of a trigger crossing if
     * that condition ceases to be satisfied while waiting for the delay.*/
    if (!event_triggered_pending_delay) {
      /* In the event that the trigger-condition is just satisfied,
       * WatchValuesBase::test_crossing will make some assignments.
       * If we have configured a non-zero delay, we need to revert those
       * changes and record the value of delay_ref.*/
      if ( WatchValuesBase<watchType>::test_crossing() &&
           (!MathUtils::is_near_equal(delay_offset, 0.0))) {
        event_triggered_pending_delay = true;
        // keep the event active until the delay is satisfied
        SubscriptionBase::active = true;
        WatchValuesBaseCore::event_triggered = false;
        WatchValuesBaseCore::int_event_triggered = 0;
        delay_ref_at_trigger = delay_ref;
      }
    }
    else if (reset_delay){
      delay_ref_at_trigger = delay_ref;
    }
    // Else, we need to test the delay-condition
    else if ( std::abs( delay_ref - delay_ref_at_trigger) >= delay_offset) {
      WatchValuesBaseCore::event_triggered = true;
      WatchValuesBaseCore::int_event_triggered = 1;
      event_triggered_pending_delay = false;
      SubscriptionBase::active = WatchValuesBaseCore::multi_shot;
    }
    reset_delay = false;

    /* Re-lock triggers once they've been evaluated, unless externally_managed
     * is set to false. */
    WatchValuesBaseCore::locked = WatchValuesBaseCore::externally_managed;

    return WatchValuesBaseCore::event_triggered;
  }

  void set_delay_offset(double delay_offset_) override {
    delay_offset = delay_offset_;
  }

 private:
  WatchValuesDelay (const WatchValuesDelay& rhs);
  WatchValuesDelay& operator = (const WatchValuesDelay& rhs);
};
#endif
