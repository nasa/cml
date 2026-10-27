/*******************************************************************************

PURPOSE:
   (Define constraints for checking against sim variables)

LIBRARY DEPENDENCY:
   (
    (cml/models/utilities/cml_message/src/cml_message.cc)
   )

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES) (Initial version))
 )
*******************************************************************************/
#ifndef CML_INTERVAL_CONSTRAINT_HH
#define CML_INTERVAL_CONSTRAINT_HH

#include "cml/models/utilities/constraint_check/include/constraint_enum.hh"
#include "constraint.hh"
#include "constraint_test_templates.hh"
#include "constraint_test_timed_templates.hh"
#include "constraint_set.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include <cstddef>

/**
 * Uses the interval constraint tests, which are violated when the variable
 * is either within or without a specified interval
 *
 * Combining these tests typically results in a constraint violation when
 * the variable is either:
 * - within any of a set of forbidden intervals, or
 * - without all of a set of permissible intervals
 *
 * The tests used in this constraint are set to be marked as violated
 * immediately upon the variable violating the specified interval bounds.
 *
 * @pre @ref NumIntervals cannot be zero.
 */
template<typename DataType, std::size_t NumIntervals>
class IntervalInstantConstraint : public Constraint
{
  static_assert(NumIntervals > 0, "A constraint with zero tests is not useful.");

 protected:
  const DataType & variable; /**< (--)
    The variable being monitored for violation of the constraints. */
 public:
  ConstraintTest_Interval<DataType> tests[NumIntervals]; /**< (--)
    The single Constraint-test for this constraint.
    Note -- cannot use a std::array here because Trick's checkpoint-restart
    prevents use of STL-arrays of templated instances.*/

  /**
   * Construct the constraint and register its tests with the base class
   *
   * @param var[in] Reference to the variable being monitored
   */
  explicit IntervalInstantConstraint (const DataType & var)
    :
    variable(var)
  {
    register_tests(tests);
  }

  /**
   * Construct the constraint, register its tests with the base class, and register
   * the constraint with a constraint-set
   *
   * @param var[in] Reference to the variable being monitored
   * @param set[out] Constraint set which manages this constraint
   */
  IntervalInstantConstraint (const DataType & var,
                             ConstraintSet & set)
    :
    IntervalInstantConstraint(var)
  {
    set.constraints.push_back(this);
  }

  /**
   * Copy constructor deleted
   */
  IntervalInstantConstraint( const IntervalInstantConstraint &) = delete;

  /**
   * Copy assignment operator deleted
   */
  IntervalInstantConstraint & operator=( const IntervalInstantConstraint &) = delete;

  /**
   * Perform sanity checks
   */
  void initialize() override
  {
    Constraint::initialize();
    /* If configuration is set to trip a violation when variable is:
     * - OUTSIDE ANY interval, or
     * - INSIDE ALL intervals,
     * there's probably something wrong.*/
    if (initialized &&
        (NumIntervals > 1) &&
        (violate_on_any_test ==
             (tests[0].get_violation_condition() == ConstraintEnum::Out))) {
      CMLMessage::warn( __FILE__,__LINE__,
        "Possible mismatch on constraint ",name,
        "\nbetween the violate_on_any_test flag (", violate_on_any_test,
        ")\n and the violation_condition setting (",violation_condition,
        ").\nCheck configuration.");
    }
  }

  /**
   * Check each constituent constraint-test for violations
   */
  void update() override
  {
    if (!active) {return;}

    for (auto & test : tests) {
      test.test_violation(variable);
    }
    post_update(); // inherited.
  }
};

/**
 * Uses the interval constraint tests, which are violated when the variable
 * is either within or without a specified interval
 *
 * Combining these tests typically results in a constraint violation when
 * the variable is either:
 * - within any of a set of forbidden intervals, or
 * - without all of a set of permissible intervals
 *
 * The tests used in this constraint are set to be marked as violated
 * when the variable persistently violates the specified interval bounds for
 * an assigned period of time.
 *
 * @pre @ref NumIntervals cannot be zero.
 */
template<typename DataType, std::size_t NumIntervals>
class IntervalTimedConstraint : public Constraint
{
  static_assert(NumIntervals > 0, "A constraint with zero tests is not useful.");

 protected:
  const DataType & variable; /**< (--)
    The variable being monitored for violation of the constraints. */
  const double & delta_time; /**< (s)
    Reference to the time-elapsed between calls to update this constraint. */
 public:
  ConstraintTest_IntervalTimed<DataType> tests[NumIntervals]; /**< (--)
    The single Constraint-test for this constraint.
    Note -- cannot use a std::array here because Trick's checkpoint-restart
    prevents use of STL-arrays of templated instances. */


  /**
   * Construct the constraint and register its tests with the base class
   *
   * @param variable_[in] Reference to the variable being monitored
   * @param delta_time_[in] Reference to the update period of this model
   */
  IntervalTimedConstraint (const DataType & variable_,
                           const double & delta_time_)
    :
    variable(variable_),
    delta_time(delta_time_)
  {
    register_tests(tests);
  }

  /**
   * Construct the constraint, register its tests with the base class, and register
   * the constraint with a constraint-set
   *
   * @param variable_[in] Reference to the variable being monitored
   * @param delta_time_[in] Reference to the update period of this class
   * @param set[out] Constraint set which manages this constraint
   */
  IntervalTimedConstraint (const DataType & variable_,
                           const double & delta_time_,
                           ConstraintSet & set)
    :
    IntervalTimedConstraint (variable_,
                             delta_time_)
  {
    set.constraints.push_back(this);
  }

  /**
   * Copy constructor deleted
   */
  IntervalTimedConstraint( const IntervalTimedConstraint &) = delete;

  /**
   * Copy assignment operator deleted
   */
  IntervalTimedConstraint & operator=( const IntervalTimedConstraint &) = delete;

  /**
   * Perform sanity checks
   */
  void initialize() override
  {
    Constraint::initialize();
    /* If configuration is set to trip a violation when variable is:
     * - OUTSIDE ANY interval, or
     * - INSIDE ALL intervals,
     * there's probably something wrong.*/
    if (initialized &&
        (NumIntervals > 1) &&
        (violate_on_any_test ==
             (tests[0].get_violation_condition() == ConstraintEnum::Out))) {
      CMLMessage::warn( __FILE__,__LINE__,
        "Possible mismatch on constraint ",name,
        "\nbetween the violate_on_any_test flag (", violate_on_any_test,
        ")\n and the violation_condition setting (",violation_condition,
        ").\nCheck configuration.");
    }
  }

  /**
   * Check each constituent constraint-test for violations
   */
  void update() override
  {
    if (!active) {return;}

    for (auto & test : tests) {
      test.test_violation(variable, delta_time);
    }
    post_update(); // inherited.
  }
};
#endif
