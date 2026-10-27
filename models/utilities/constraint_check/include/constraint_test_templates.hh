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
#ifndef CML_CONSTRAINT_TEST_TEMPLATES_HH
#define CML_CONSTRAINT_TEST_TEMPLATES_HH

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
   ConstraintTest. They do not include timer capabilities.
 - To the extent possible, as much common content has been placed in one of
   ConstraintTest and ConstraintTestTimed. Inheritance from these two
   classes into the constraint-test template-classes has to be duplicated to
   avoid complications of inheriting from a class-template (which Trick-SWIG
   does not support well). Consequently, there is significant similarity
   between this file and constraint_test_timed_templates.hh (which defines
   the class-templates inheriting from ConstraintTestTimed).
*****************************************************************************/

/**
 * A constraint test which evaluates a variable against a threshold value
 *
 * Configured by selecting the specific comparison behavior from @ref ConstraintEnum,
 * such as greater-than, less-than, equal-to, etc.
 *
 * @tparam DataType The data type of the threshold
 */
template <typename DataType>
class ConstraintTest_Threshold : public ConstraintTest
{
 public:
  DataType threshold {}; /**< (--)
    The threshold value that, if crossed, will trigger a domain violation. */

  /**
   * Constructor
   */
  ConstraintTest_Threshold() = default;

  /**
   * Verify that the model has been configured correctly
   *
   * @return Always `true`
   */
  bool initialize() override
  {
    initialize_threshold();
    return true;
  }

  /**
   * Evaluate whether the constraint has been violated
   *
   * @param variable[in] The value being tested
   * @return `true` if the constraint was violated
   *
   * @note If the model is not @ref active, always returns `false` even if the constraint
   *       is technically violated.
   */
  bool test_violation (DataType variable)
  {
    if (!active) { return false;}

    violation = test_threshold_violation(
      variable, threshold, violation_condition, equality_threshold);
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
 *
 * Configured by selecting the specific comparison behavior from
 * @ref ConstraintEnum::IntervalBounds, such as a closed-closed interval, closed-open
 * interval, etc.
 *
 * @tparam DataType The data type of the interval
 */
template <typename DataType>
class ConstraintTest_Interval : public ConstraintTest
{
 public:
  DataType lower_bound {0}; /**< (--)
    The lower bound of the interval that, if crossed, will trigger a domain
    violation. */
  DataType upper_bound {0}; /**< (--)
    The upper bound of the interval that, if crossed, will trigger a domain
    violation. */
  ConstraintEnum::IntervalBounds interval_bounds{ConstraintEnum::ClosedClosed}; /**<
    Option for how to treat a value at the edges of the interval. */

  /**
   * Constructor
   */
  ConstraintTest_Interval() = default;

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
        lower_bound," and ",upper_bound," with equality threshold\n",
        equality_threshold,". Initialization failed.\n");
      return false;
    }
    initialize_interval();
    return true;
  }

  /**
   * Evaluate whether the constraint has been violated
   *
   * Checks for whether @ref variable is included within the specified upper and lower
   * bounds. This check is combined with the configuration for what determines a violation
   * (i.e., whether being inside or outside the interval constitutes a violation) to
   * determine the violation status.
   *
   * @param variable[in] The value being tested
   *
   * @return `true` if the constraint was violated
   *
   * @note If the model is not @ref active, always returns `false` even if the constraint
   *       is technically violated.
   */
  bool test_violation (DataType variable)
  {
    if (!active) { return false;}

    const bool in_interval = test_interval_violation(
      variable, lower_bound, upper_bound, interval_bounds);
    violation = (in_interval == (violation_condition == ConstraintEnum::In));
    count_violations();
    return violation;
  }
};

/**
 * A constraint test which evaluates whether a value is contained within a set of
 * predetermined values
 *
 * @tparam DataType The data type of the set of values to test against
 */
template <typename DataType>
class ConstraintTest_ValSet : public ConstraintTest
{
 public:
  std::list<DataType> values; /**< (--)
    The values against which the variable under test is compared against. */

  /**
   * Constructor
   */
  ConstraintTest_ValSet() = default;

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
   * @param variable[in] The value being tested
   *
   * @return `true` if the constraint was violated
   *
   * @note If the model is not @ref active, always returns `false` even if the constraint
   *       is technically violated.
   */
  bool test_violation (DataType variable)
  {
    if (!active) { return false;}

    const bool in_set =
      std::any_of( values.begin(), values.end(),
        [&](const DataType & val_) {return query_equals(variable, val_, equality_threshold);});

    violation = (in_set == (violation_condition == ConstraintEnum::In));
    count_violations();
    return violation;
  }
};
#endif
