/*******************************************************************************

PURPOSE:
   (Defines specific constraint tests for checking a sim variable value against
    a domain specification and duration constraint.)

LIBRARY DEPENDENCY:
  (
   (cml/models/utilities/cml_message/src/cml_message.cc)
  )

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES) (Initial version))
 )
*******************************************************************************/
#ifndef CML_CONSTRAINT_TEST_TIMED_TEMPLATES_HH
#define CML_CONSTRAINT_TEST_TIMED_TEMPLATES_HH

#include <list>
#include <algorithm>

#include "constraint_enum.hh"
#include "constraint_test.hh"
#include "constraint_util.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

/*****************************************************************************
Notes:
 - The classes defined in here are class-template extensions of
   ConstraintTestTimed, and so include timer capabilities.
 - To the extent possible, as much common content has been placed in one of
   ConstraintTest and ConstraintTestTimed. Inheritance from these two
   classes into the constraint-test template-classes has to be duplicated to
   avoid complications of inheriting from a class-template (which Trick-SWIG
   does not support well). Consequently, there is significant similarity
   between this file and constraint_test_templates.hh (which defines
   the class-templates inheriting from ConstraintTest).
 - By setting the flag "use_timer" to false, the class-templates defined
   herein will function identically to those found in
   constraint_test_templates.hh, making those somewhat redundant. However,
   these templates carry significant additional overhead so it was decided
   to keep both timed and non-timed constraint-tests defined to provide a
   low-overhead implementation for situations in which timing is not needed.
*****************************************************************************/

/**
 * A constraint test which evaluates a variable against a threshold value
 * over a period of time
 *
 * Configured by selecting the specific comparison behavior from @ref ConstraintEnum,
 * such as greater-than, less-than, equal-to, etc.
 *
 * @tparam DataType The data type of the threshold
 */
template <typename DataType>
class ConstraintTest_ThresholdTimed : public ConstraintTestTimed
{
 public:
  DataType threshold {}; /**< (--)
    The threshold value that, if crossed, will trigger a domain violation. */

  /**
   * Constructor
   */
  ConstraintTest_ThresholdTimed() = default;

  /**
   * Verify that the model has been configured correctly
   *
   * @return Always `true`
   */
  bool initialize() override
  {
    initialize_threshold();
    initialize_gap_time();
    return true;
  }

  /**
   * Evaluate whether the constraint has been violated
   *
   * @param[in] variable The value being tested
   * @param[in] delta_time Simulation job execution period
   *
   * @return `true` if the constraint was violated
   *
   * @note If the model is not @ref active, always returns `false` even if the constraint
   *       is technically violated.
   */
  bool test_violation (DataType variable,
                       double delta_time)
  {
    if (!active) { return false;}

    numerical_violation = test_threshold_violation(
      variable, threshold, violation_condition, equality_threshold);

    if (use_timer) {
      update_timer(delta_time);
    } else {
      violation = numerical_violation;
    }
    count_violations();
    return violation;
  }

  /**
   * Return the threshold being tested against
   *
   * @return @ref threshold casted to `double`
   * @pre The data type of @ref threshold must be convertible to `double`
   */
  double get_threshold() const override
  {
    return static_cast<double>(threshold);
  }
};

/**
 * A constraint test which evaluates whether a value is contained within an interval
 * over a period of time
 *
 * Configured by selecting the specific comparison behavior from
 * @ref ConstraintEnum::IntervalBounds, such as a closed-closed interval, closed-open
 * interval, etc.
 *
 * @tparam DataType The data type of the interval
 */
template <typename DataType>
class ConstraintTest_IntervalTimed : public ConstraintTestTimed
{
 public:
  DataType lower_bound {0}; /**< (--)
    The lower bound of the interval that, if crossed, will trigger a domain
    violation. */
  DataType upper_bound {0}; /**< (--)
    The upper bound of the interval that, if crossed, will trigger a domain
    violation. */
  ConstraintEnum::IntervalBounds interval_bounds{ConstraintEnum::ClosedClosed}; /**< (--)
    Option for how to treat a value at the edges of the interval. */

  /**
   * Constructor
   */
  ConstraintTest_IntervalTimed() = default;

  /**
   * Verify that the model has been configured correctly
   *
   * If the @ref upper_bound and @ref lower_bound are within the @ref equality_threshold
   * (i.e., too close together), the model is not configured properly.
   *
   * @return `true` if the configuration is valid
   */
  bool initialize() override
  {
    if (upper_bound < lower_bound) {
      std::swap(upper_bound, lower_bound);
    }

    if (MathUtils::is_within_abs_tolerance(upper_bound, lower_bound, static_cast<DataType>(equality_threshold))) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot initialize an interval constraint-test with a\n"
        "zero-width interval. Bounds are set to:\n",
        lower_bound," and ",upper_bound,".\n"
        "Initialization failed.\n");
      return false;
    }
    initialize_interval();
    initialize_gap_time();
    return true;
  }

  /**
   * Evaluate whether the constraint has been violated
   *
   * Checks for whether @p variable is included within the specified upper and lower
   * bounds. This check is combined with the configuration for what determines a violation
   * (i.e., whether being inside or outside the interval constitutes a violation) to
   * determine the violation status.
   *
   * @param[in] variable The value being tested
   * @param[in] delta_time Simulation job execution period
   *
   * @return `true` if the constraint was violated
   *
   * @note If the model is not @ref active, always returns `false` even if the constraint
   *       is technically violated.
   */
  bool test_violation (DataType variable,
                       double delta_time)
  {
    if (!active) { return false;}

    const bool in_interval = test_interval_violation(
      variable, lower_bound, upper_bound, interval_bounds);

    if (use_timer) {
      numerical_violation =
                  (in_interval == (violation_condition == ConstraintEnum::In));
      update_timer(delta_time);
    } else {
      violation = (in_interval == (violation_condition == ConstraintEnum::In));
    }
    count_violations();
    return violation;
  }
};

/**
 * A constraint test which evaluates whether a value is contained within a set of
 * predetermined values over a period of time
 *
 * @tparam DataType The data type of the set of values to test against
 */
template <typename DataType>
class ConstraintTest_ValSetTimed : public ConstraintTestTimed
{
 public:
  std::list<DataType> values; /**< (--)
    The values against which the variable under test is compared against. */

  /**
   * Constructor
   */
  ConstraintTest_ValSetTimed() = default;

  /**
   * Verify that the model has been configured correctly
   *
   * If no values have been defined to test against in @ref values, the configuration
   * is invalid.
   *
   * @return `true` if the configuration is valid
   */
  bool initialize() override
  {
    if (values.empty()) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot initialize a value-set constraint-test with an empty\n"
        "set of values to test against.\nInitialization failed.\n");
      return false;
    }
    initialize_val_set();
    initialize_gap_time();
    return true;
  }

  /**
   * Evaluate whether the constraint has been violated
   *
   * Checks for whether variable is equal to one of the values in @ref values. The
   * result of this test is combined with the configuration for what constitutes a
   * violation (i.e., violation if the value is contained in the list or if the
   * value is not contained in the list) to determine the violation status.
   *
   * @param[in] variable The value being tested
   * @param[in] delta_time Simulation job execution period
   *
   * @return `true` if the constraint was violated
   *
   * @note If the model is not @ref active, always returns `false` even if the constraint
   *       is technically violated.
   */
  bool test_violation (DataType variable,
                       double delta_time)
  {
    if (!active) { return false;}

    const bool in_set =
      std::any_of( values.begin(), values.end(),
        [&](const DataType & val_) {return query_equals(variable, val_, equality_threshold);});

    if (use_timer) {
      numerical_violation =
                       (in_set == (violation_condition == ConstraintEnum::In));
      update_timer(delta_time);
    } else {
      violation = (in_set == (violation_condition == ConstraintEnum::In));
    }
    count_violations();
    return violation;
  }
};
#endif
