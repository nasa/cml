/*******************************************************************************

PURPOSE:
   (Defines a single constraint for checking against a sim variable)

LIBRARY DEPENDENCIES:
  ((cml/models/utilities/cml_message/src/cml_message.cc))

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/

#include "../include/constraint.hh"
#include "../include/constraint_enum.hh"
#include "../include/constraint_test.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include <algorithm>
#include <cstddef>


/*****************************************************************************
Constructor
*****************************************************************************/
Constraint::Constraint( size_t num_tests_)
  :
  num_tests(num_tests_)
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
  bool new_violation_status = false; // local temporary measure of constraint status
  // If configured with "Any", trip on the first test that is violated;
  // record its index.
  if (violate_on_any_test) {
    // Step through by index because we want to record which test violated.
    for (size_t ii = 0; ii < num_tests; ii++) {
      if (test_list[ii]->get_violation()) {
        test_violated_index = ii;
        test_violated_time_limit = test_list[ii]->get_time_limit();
        test_violated_threshold = test_list[ii]->get_threshold();
        new_violation_status = true;
        /* increment the count if this violation is not a holdover from the
           previous cycle (violated is the copy held over from the previous
           cycle). */
        violation_count += static_cast<int>(!violated);
        break;
      }
    }
  }

  // If configured with "All", require all test violations to trip the
  // constraint violation. Start with an assumption that there is a
  // violation, and correct that assumption as necessary:
  else {
    new_violation_status = std::all_of(test_list.begin(), test_list.end(),
      [](const auto& test) {return test->get_violation();});
    /* increment the count if this violation is not a holdover from the
       previous cycle (violated is the copy held over from the previous
       cycle). */
    violation_count += static_cast<int>(new_violation_status && !violated);
  }
  violated = new_violation_status;
}

/*****************************************************************************
Name: activate
Purpose: Activates the constraint and all of its enabled constraint-tests
*****************************************************************************/
void
Constraint::activate()
{
  if (!enabled || !initialized) {return;}
  for (auto* test: test_list) {
    test->activate();
  }
  active = true;
}