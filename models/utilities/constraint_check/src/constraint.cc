/*******************************************************************************

PURPOSE:
   (Defines a single constraint for checking against a sim variable)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/

#include "../include/constraint.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
Constraint::Constraint( size_t num_tests_)
  :
  violation_condition( ConstraintEnum::Undefined),
  name(),
  violated(false),
  violation_count(0),
  violate_on_any_test(true),
  enabled(true),
  active(false),
  initialized(false),
  prev_violated(false),
  num_tests(num_tests_),
  test_list(),
  test_violated_index(0),
  test_violated_time_limit(0.0)
{}

/*****************************************************************************
Name: initialize
Purpose:
  Default sanity checks
*****************************************************************************/
void
Constraint::initialize()
{
  if (test_list.empty()) {
    CMLMessage::error( __FILE__,__LINE__,
      "Failure to initialize a constraint:\n"
      "Constraint requires at least one constraint-test.\n"
      "Aborting initialization.\n");
    return;
  }

  for(ConstraintTest * test:test_list) {
    if (test->get_violation_condition() == ConstraintEnum::Undefined) {
      test->set_violation_condition(violation_condition);
    }
    if (!test->initialize()) {
      CMLMessage::error( __FILE__,__LINE__,
        "Failure to initialize a constraint-test means the constraint\n"
        "cannot be initialized.\n"
        "Aborting initialization.\n");
      return;
    }
  }
  initialized = true;
}

/*****************************************************************************
Name: post_update
Purpose:
  Runs after the constraint-tests have been evaluated. Uses the status of
  the ocnstraint-tests to set the status of the constraint.
*****************************************************************************/
void
Constraint::post_update()
{
  // If configured with "Any", trip on the first test that is violated;
  // record its index.
  if (violate_on_any_test) {
    bool violated_ = false;
    // Step through by index because we want to record which test violated.
    for (size_t ii = 0; ii < num_tests; ii++) {
      if (test_list[ii]->get_violation()) {
        test_violated_index = ii;
        test_violated_time_limit = test_list[ii]->get_time_limit();
        violated_ = true;
        violated = true;
        count_violations();
        break;
      }
    }
    violated = violated_; // in case violated_ is still false.
  }

  // If configured with "All", require all test violations to trip the
  // constraint violation:
  else {
    bool violated_ = true;
    // Don't need to record which test violated, so don't need indices.
    for (ConstraintTest * test : test_list) {
      if (!test->get_violation()) {
        violated_ = false;
        break;
      }
    }
    violated = violated_;
    if (violated) {
      count_violations();
    }
  }
  prev_violated = violated;
}

/*****************************************************************************
Name: count_violations
Purpose: Counts the number of unique violations
*****************************************************************************/
void
Constraint::count_violations()
{
  if (violated != prev_violated) {
    violation_count += violated;
    prev_violated = violated;
  }
}

/*****************************************************************************
Name: activate
Purpose: Activates the constraint and all of its enabled constraint-tests
*****************************************************************************/
void
Constraint::activate()
{
  if (!enabled || !initialized) {return;}
  for (auto test: test_list) {
    test->activate();
  }
  active = true;
}
