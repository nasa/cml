/*******************************************************************************

PURPOSE:
   (Defines the abstract basis of a single constraint for checking against
    a sim variable. A constraint may include multiple constraint-tests;
    these configurations are implemented in the derived classes)

LIBRARY DEPENDENCY:
  ((../src/constraint.cc)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES) (Initial version))
 )
*******************************************************************************/
#ifndef CML_CONSTRAINT_HH
#define CML_CONSTRAINT_HH

#include <algorithm>
#include <string>
#include <cstddef>
#include <iterator>
#include <vector>

#include "constraint_enum.hh"
#include "constraint_test.hh"

/**
 * An abstract class which holds an arbitrary number of constraint tests
 */
class Constraint
{
  public:
  // Configuration options
  ConstraintEnum::ViolationCondition violation_condition{ConstraintEnum::Undefined}; /**< (--)
    The default violation-condition to be applied to all constraint-tests
    within this constraint, unless they are configured separately. */
  std::string name; /**< (--)
    Name of constraint; optional, used for debugging. */
  bool violated{false}; /**< (--)
    Flag indicating that after evaluating the associated ConstraintTest
    instances, at least one was found with a violation. */
  unsigned int violation_count{0}; /**< (--)
    Count of the number of violations encountered of this constraint. */
  bool violate_on_any_test{true}; /**< (--)
    For constraints with multiple tests, this flag identifies whether the
    constraint is violated when ANY or ALL tests indicate a violation. */
  bool enabled{true}; /**< (--)
    Flag indicating this constraint is intended to be evaluated.
    Constraints may be disabled, which prevents them from being activated.
    An active constraint gets processed. */
    
  // Model outputs
  unsigned int test_violated_index{0}; /**< (--)
    Record of which test caused the constraint to be violated. */
  double test_violated_time_limit{0.0}; /**< (s)
    The time-limit -- where applicable -- of (one of) the test(s) that
    resulted in a violation.
    If multiple tests result in a violation, the value recorded here is that
    of the lowest indexed test.
    If the tests do not include time limits, this value will remain 0.0 */
  double test_violated_threshold{0.0}; /**< (--)
    The threshold  -- where applicable -- of (one of) the test(s) that
    resulted in a violation, cast to a double for logging purposes.
    If multiple tests result in a violation, the value recorded here is that
    of the lowest indexed test.
    If the tests do not include thresholds, this value will remain 0.0.
    Technically in the units of the threshold / test, which are unknown. */

 protected:
  bool active{false}; /**< (--)
    Flag indicating this constraint is going to be checked by the next cycle
    of the associated constraint-set. */
  bool initialized{false}; /**< (--)
    Flag indicating this constraint-checker has passed sanity checks
    and is ready to be used. */

  std::vector<ConstraintTest*> test_list; /**< (--)
    List of pointers to constraint-tests, used for baseline functionality. */

 public:
  /**
   * Constructor
   */
  Constraint() = default;

  /**
   * Virtual destructor
   */
  virtual ~Constraint() = default;

  /**
   * Copy constructor deleted
   */
  Constraint( const Constraint &) = delete;

  /**
   * Copy assignment operator deleted
   */
  Constraint & operator=( const Constraint &) = delete;

  /**
   * Verify that the model was configured correctly
   */
  virtual void initialize();

  /**
   * @return `true` if the model is initialized
   */
  bool is_initialized() const {return initialized;}

  /**
   * @return `true` if the model is active
   */
  bool is_active() const {return active;}

  /**
   * The model's main update routine and customization point for child classes
   * to implement their specific behaviors
   */
  virtual void update() = 0;

  /**
   * Add constraint tests to the internal list of tests which will execute during
   * model updates
   *
   * @tparam TestType The test type, which must be derived from @ref ConstraintTest
   * @tparam NumTests Size of the array
   * @param new_tests[in] Tests to register
   *
   * @warning Pointers to the contents of @ref new_tests are stored internally. That
   *          array's lifetime must be at least as long as this object's.
   */
  template <typename TestType, std::size_t NumTests>
  void register_tests(TestType (&new_tests)[NumTests]) {
    std::transform(std::begin(new_tests), std::end(new_tests), std::back_inserter(test_list),
      [](TestType& new_test){return &new_test;});
  }

  /**
   * Activate the constraint and all of its enabled constraint-tests
   */
  void activate();

  /**
   * Deactivate the constraint
   */
  void deactivate() {active = false;}

 protected:
  /**
   * Determine the overall violation status of the constraint based on the violation
   * status of its constituent constraint-tests
   *
   * Runs after all constraint-tests have been evaluated.
   */
  void post_update();
};
#endif
