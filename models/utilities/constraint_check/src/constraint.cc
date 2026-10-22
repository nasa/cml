/*******************************************************************************

PURPOSE:
   (Defines a single constraint for checking against a sim variable)

LIBRARY DEPENDENCIES:
  ((cml/models/utilities/cml_message/src/cml_message.cc))

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES) (Initial version))
 )
*******************************************************************************/
#include "../include/constraint.hh"
#include "../include/constraint_enum.hh"
#include "../include/constraint_test.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include <algorithm>
#include <iterator>

// Verify that the model was configured correctly
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

// Determine the overall violation status of the constraint based on the violation
// status of its constituent constraint-tests
void
Constraint::post_update()
{
  bool new_violation_status = false; // local temporary measure of constraint status
  auto constraint_violated = [](const ConstraintTest* test) {return test->get_violation();};

  if (violate_on_any_test) {
    // If configured with "Any", trip on the first test that is violated;
    // record its index.
    const auto failed_test = std::find_if(test_list.begin(), test_list.end(), constraint_violated);
    if (failed_test != test_list.end()) {
      test_violated_index = std::distance(test_list.begin(), failed_test);
      test_violated_time_limit = (*failed_test)->get_time_limit();
      test_violated_threshold = (*failed_test)->get_threshold();
      new_violation_status = true;
    }
  }
  else {
    // If configured with "All", require all test violations to trip the
    // constraint violation.
    new_violation_status = std::all_of(test_list.begin(), test_list.end(), constraint_violated);
  }

  // Increment the count if this violation is not a holdover from the previous cycle.
  if (new_violation_status && !violated) {
    ++violation_count;
  }
  violated = new_violation_status;
}

// Activate the constraint and all of its enabled constraint-tests
void
Constraint::activate()
{
  if (!enabled || !initialized) {return;}
  for (auto* test: test_list) {
    test->activate();
  }
  active = true;
}
