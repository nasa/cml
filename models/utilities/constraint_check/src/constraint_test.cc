/*******************************************************************************

PURPOSE:
   (Defines specific constraint tests for checking a sim variable value against
    a domain specification and duration constraint.)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/

#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils
#include "../include/constraint_test.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
ConstraintTest::ConstraintTest()
  :
  enabled(true),
  initialized(false),
  active(false),
  violation_condition( ConstraintEnum::Undefined),
  violation(false),
  prev_violation(false),
  violation_count(0),
  equality_threshold(1e-6)
{};
// ***************************************************************************
ConstraintTestTimed::ConstraintTestTimed()
  :
  ConstraintTest(),
  use_timer(true),
  time_limit (0.0),
  gap_time_limit (0.0),
  recover_quiet_violation (false),
  numerical_violation(false),
  existing_numerical_violation (false),
  checking_gap_timer(false),
  violation_timer (0.0),
  gap_timer (0.0),
  accumulated_violation_timer (0.0)
{};

/*****************************************************************************
Name: set_violation_condition
Purpose:
  Sets the violation-condition enumeration, with protection for adjusting
  post-initialization.
*****************************************************************************/
void
ConstraintTest::set_violation_condition(
  ConstraintEnum::ViolationCondition cond)
{
  violation_condition = cond;
  if (initialized) {
    initialized = false;
    initialize();
  }
}

/*****************************************************************************
Name: count_violations
Purpose: Counts the number of unique violations
*****************************************************************************/
void
ConstraintTest::count_violations()
{
  if (violation != prev_violation) {
    violation_count += violation;
    prev_violation = violation;
  }
}

/***************************************************************************
Name: initialize_threshold
Purpose: checks for configuration for threshold-based tests.
***************************************************************************/
void
ConstraintTest::initialize_threshold()
{
  if (violation_condition == ConstraintEnum::Undefined ||
      violation_condition == ConstraintEnum::In ||
      violation_condition == ConstraintEnum::Out) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot initialize a threshold constraint-test without a valid "
      "violation-condition.\n"
      "Defaulting to GT (greater than):\n"
      "  violation when variable > threshold.\n");
    violation_condition = ConstraintEnum::GT;
  }
  initialized = true;
}

/***************************************************************************
Name: initialize_interval
Purpose: checks for configuration for interval-based tests.
***************************************************************************/
void
ConstraintTest::initialize_interval()
{
  if (violation_condition != ConstraintEnum::In &&
      violation_condition != ConstraintEnum::Out) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot initialize an interval constraint-test with a \n"
      "violation-condition of ",violation_condition,".\n"
      "Defaulting to In:\n"
      "  violation when variable is between lower_bound and upper_bound.\n");
    violation_condition = ConstraintEnum::In;
  }
  initialized = true;
}

/***************************************************************************
Name: initialize_val_set
Purpose: checks for configuration for val-set-based tests.
***************************************************************************/
void
ConstraintTest::initialize_val_set()
{
  if (violation_condition != ConstraintEnum::In &&
      violation_condition != ConstraintEnum::Out) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot initialize a value-set constraint-test with a \n"
      "violation-condition of ",violation_condition,".\n"
      "Defaulting to In:\n"
      "  violation when variable is one of values in values-list.\n");
    violation_condition = ConstraintEnum::In;
  }
  initialized = true;
}

/*****************************************************************************
Name: initialize_gap_timer
Purpose:
  Sets the gap-time-limit equal to the time-limit if it has not been
  set independently.
*****************************************************************************/
void
ConstraintTestTimed::initialize_gap_time()
{
  if ( recover_quiet_violation &&
       !MathUtils::has_changed_from( gap_time_limit, 0.0)) {
    gap_time_limit = time_limit;
  }
}

/*****************************************************************************
Name: update_timer
Purpose:
  updates the violation timer
Notes:
 - a numerical-violation will result in checking the current timestamp
   against that at the start of the numerical-violation to identify whether
   the numerical-violation has been persistent for sufficient time. The
   violation flag is set accordingly.
   - (violation = false) will be retained even if there is a
     numerical-violation until the persistence check has passed.
- The violation flag will be set to false automatically if there is no
  numerical-violation; there is no comparable "retention" of a violated
  status for any sort of persistence.
  - However, the "existing-numerical-violation" flag does have a persistence
    check on it; it will mode down only after the gap-time requirement has
    been satisfied. A numerical violation that momentarily modes back into
    an acceptable range will not reset the violation-timer until this
    gap-time reset is reached.
Example:
  a constraint violation requires that the numerical-violation be persistent
  for 3 counts, and cleared after 2 counts of numerical-satisfaction.
  t: numerical-violation  violation:   notes
  1:       present,        no      lack of persistence.
  2:       present,        no      lack of persistence.
  3:       present,        yes     3x persistent
  4:       present,        yes     >3x persistent
  5:       absent,         no      persistence timer not reset
  6:       present,        yes     >3x persistence (retained)
  7:       absent,         no      persistence timer not reset
  8:       absent,         no      persistence timer reset
  9:       present,        no      lack of persistence.
*****************************************************************************/
void
ConstraintTestTimed::update_timer( double delta_time)
{
  /* Note: using an incremental accumulation of delta-time rather than
   * differencing current-time from a start-time because we do not want
   * to count short gaps as violation-times, but may not want to reset the
   * violation-time clock if the gap is short.*/

  if (numerical_violation) {
    if (existing_numerical_violation) { // advance the timer
      violation_timer += delta_time;
      accumulated_violation_timer += delta_time;
      gap_timer = 0.0;
    }
    else { // new violation, start the violation-timer.
      existing_numerical_violation = true;
      violation_timer = delta_time;
      accumulated_violation_timer += delta_time;
    }

    violation = (violation_timer >= (time_limit - 1E-10));
  }

  else {
    violation = false;
    if (existing_numerical_violation) { // (but no active numerical-violation)
      // Relaxing from a previous numerical-violation, checking the gap-timer:
      if (checking_gap_timer) {
        gap_timer += delta_time;
      }
      /* If there is an "existing-numerical-violation" and gap-timer is not
         getting checked, this is the first cycle after the previous
         numerical-violation ended:*/
      else {
        // reset the gap-timer
        checking_gap_timer = true;
        gap_timer = delta_time;
      }

      /* if violation has been stopped long enough, reset the violation-
         persistence timer by resetting the existing_numerical_violation flag.*/
      if (gap_timer > (gap_time_limit - 1E-10)) {
        existing_numerical_violation = false;
        checking_gap_timer = false;
        violation_timer = 0.0;
      }
    }

    else { // no active violation; no recent violation.
      gap_timer += delta_time;
    }
  }
}


