/*******************************************************************************

PURPOSE:
   (Defines the abstract basis of a single constraint for checking against
    a sim variable. A constraint may include multiple constraint-tests;
    these configuratons are implemented in the derived classes)

LIBRARY DEPENDENCY:
  ((../src/constraint.cc)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/
#ifndef CML_CONSTRAINT_BASE_HH
#define CML_CONSTRAINT_BASE_HH

#include <vector>
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "constraint_enum.hh"
#include "constraint_test.hh"

/*****************************************************************************
Constraint
Purpose:
  An abstract class providing basic functionality to the template classes
  InstantConstraint, TimedConstraint, and others TBD.
*****************************************************************************/
class Constraint
{
 public:
  ConstraintEnum::ViolationCondition violation_condition; /* (--)
    The default violation-condition to be appllied to all constraint-tests
    within this cosntraint, unless they are configured separately.*/
  std::string name; /* (--)
    Name of constraint; optional, used for debugging. */
  bool violated; /* (--)
    Flag indicating that after evaluating the associated ConstraintTest
    instances, at least one was found with a violation. */
  unsigned int violation_count; /* (--)
    Count of the number of violations encountered of this constraint.*/
  bool violate_on_any_test; /* (--)
    For constraints with multiple tests, this flag identifies whether the
    constraint is violated when ANY or ALL tests indicate a violation.
    Default: true (ANY).*/
  bool enabled; /* (--)
    Flag indicating this constraint is intended to be evaluated.
    Constraints may be disabled, which prevents them from being activated.
    An active constraint gets processed.*/
 protected:
  bool active; /* (--)
    Flag indicating this constraint is going to be checked by the next cycle
    of the associated constraint-set.*/
  bool initialized; /* (--)
    Flag indicating this constraint-checker has passed sanity checks
    and is ready to be used.*/
  bool prev_violated; /* (--)
    Copy of previous value of violated.*/
  const size_t num_tests; /* (--)
    Record of the number of tests in the constraint.*/

  std::vector<ConstraintTest*> test_list; /* (--)
    List of pointers to constraint-tests, used for baseline functionality.*/
  unsigned int test_violated_index; /* (--)
    Record of which test caused the constraint to be violated. */
  double test_violated_time_limit; /* (--)
    The time-limit -- where applicable -- of (one of) the test(s) that
    resulted in a violation.
    If multiple tests result in a violation, the value recorded here is that
    of the lowest indexed test.
    If the tests do not include time limits, this value will remain 0.0*/

 public:
  Constraint( size_t num_specs = 1);
  virtual ~Constraint(){};

  virtual void initialize();
  virtual void update() = 0;

  void activate();
  void deactivate() {active = false;}
 protected:
  void count_violations();
  void post_update();



 private: // not implemented; not copyable
  Constraint( const Constraint &);
  Constraint & operator=( const Constraint &);
};
#endif
