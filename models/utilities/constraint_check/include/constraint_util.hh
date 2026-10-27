/*******************************************************************************
PURPOSE:
    (Common utility functions for Constraint Check models)

LIBRARY DEPENDENCY:
    (
     (cml/models/utilities/cml_message/src/cml_message.cc)
    )

PROGRAMMERS:
    (
     ((Nino Tarantino) (NASA) (October 2026) (Split out common logic))
    )
*******************************************************************************/
#ifndef CML_CONSTRAINT_UTIL_HH
#define CML_CONSTRAINT_UTIL_HH
#include "constraint_enum.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#ifndef SWIG
/**
 * Generic equality check
 *
 * @tparam DataType Data types being compared
 * @param lhs[in] First value to compare
 * @param rhs[in] Second value to compare
 * @param equality_threshold[in] Comparison tolerance for floating point types
 * @return `true` if the two values compare equal
 */
template <typename DataType>
bool query_equals(
    const DataType lhs,
    const DataType rhs,
    [[maybe_unused]] const double equality_threshold)
{
    return lhs == rhs;
}

// Specialization for doubles
template <>
inline bool query_equals<double>(
    const double lhs,
    const double rhs,
    const double equality_threshold)
{
    return MathUtils::is_within_abs_tolerance(lhs, rhs, equality_threshold);
}

// Specialization for floats
template <>
inline bool query_equals<float>(
    const float lhs,
    const float rhs,
    const double equality_threshold)
{
    return MathUtils::is_within_abs_tolerance(lhs, rhs, static_cast<float>(equality_threshold));
}
#endif

/**
 * Common logic for testing domain violations of a value against a specified threshold
 *
 * @tparam DataType Data types being compared
 * @param variable[in] The value being checked
 * @param threshold[in] The threshold being checked against
 * @param violation_condition[in] Comparison operator used
 * @param equality_threshold[in] Equality comparison tolerance for floating point types
 *
 * @return `true` if the threshold constraint is violated
 */
template <typename DataType>
bool test_threshold_violation(
    const DataType variable,
    const DataType threshold,
    const ConstraintEnum::ViolationCondition violation_condition,
    const double equality_threshold = 0.0)
{
    bool violation = false;

    switch (violation_condition) {
        case ConstraintEnum::EQ:
            violation = query_equals(variable, threshold, equality_threshold);
            break;
        case ConstraintEnum::NEQ:
            violation = !query_equals(variable, threshold, equality_threshold);
            break;
        case ConstraintEnum::GT:
            violation = variable > threshold;
            break;
        case ConstraintEnum::GE:
            violation = variable >= threshold;
            break;
        case ConstraintEnum::LT:
            violation = variable < threshold;
            break;
        case ConstraintEnum::LE:
            violation = variable <= threshold;
            break;
        case ConstraintEnum::In:
        case ConstraintEnum::Out:
        case ConstraintEnum::Undefined:
        default:
            CMLMessage::error( __FILE__,__LINE__,
              "Invalid setting of numerical_violation_condition: ",
              violation_condition,"\nMust be ==, !=, >, >=, <, or <=.\n");
    }

    return violation;
}

/**
 * Common logic for testing whether a value is included within a specified upper and
 * lower bounds
 *
 * @tparam DataType Data type of the value and interval bounds
 * @param variable[in] Value being checked
 * @param lower_bound[in] Lower bound of the interval
 * @param upper_bound[in] Upper bound of the interval
 * @param endpoint_options[in] How to treat the edges of the interval
 *
 * @return `true` if the interval contains the value specified
 */
template <typename DataType>
bool test_interval_violation(
    const DataType variable,
    const DataType lower_bound,
    const DataType upper_bound,
    const ConstraintEnum::IntervalBounds endpoint_options)
{
    bool in_interval = false;

    switch (endpoint_options) {
        case ConstraintEnum::ClosedClosed:
            in_interval = variable >= lower_bound && variable <= upper_bound;
            break;
        case ConstraintEnum::ClosedOpen:
            in_interval = variable >= lower_bound && variable < upper_bound;
            break;
        case ConstraintEnum::OpenClosed:
            in_interval = variable > lower_bound && variable <= upper_bound;
            break;
        case ConstraintEnum::OpenOpen:
            in_interval = variable > lower_bound && variable < upper_bound;
            break;
    }

    return in_interval;
}

#endif
