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
#ifndef CML_VALSET_CONSTRAINT_HH
#define CML_VALSET_CONSTRAINT_HH

#include "cml/models/utilities/constraint_check/include/constraint_enum.hh"
#include "constraint.hh"
#include "constraint_test_templates.hh"
#include "constraint_test_timed_templates.hh"
#include "constraint_set.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include <cstddef>

/* TODO Turner 2023/03
 *   Investigate: Can Trick support doubly-templated classes?
 *      I.e. can we use a solidified class-template as the data type for
 *      a template parameter, such as:
 *       InstantConstraint< ConstraintTest_ValSet<double>, double, 6>
 *   Rationale:
 *      The threshold, interval, and val-set constraint files are very
 *      similar, with the primary difference being that they have arrays of
 *      different types of Constraint-test. If that can be **reliably**
 *      templated away, these three files can be merged.
*/

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
 * @tparam DataType Data type of the variable being monitored
 * @tparam NumValSets Number of value-set constraint tests which constitute
 *                    this constraint
 *
 * @pre @p NumValSets cannot be zero.
 */
template<typename DataType, std::size_t NumValSets>
class ValSetInstantConstraint : public Constraint
{
  static_assert(NumValSets > 0, "A constraint with zero tests is not useful.");

 protected:
  const DataType & variable; /**< (--)
    The variable being monitored for violation of the constraints. */
 public:
  ConstraintTest_ValSet<DataType> tests[NumValSets]; /**< (--)
    The single Constraint-test for this constraint.
    Note -- cannot use a std::array here because Trick's checkpoint-restart
    prevents use of STL-arrays of templated instances. */

  /**
   * Construct the constraint and register its tests with the base class
   *
   * @param[in] var Reference to the variable being monitored
   */
  explicit ValSetInstantConstraint (const DataType & var)
    :
    variable(var)
  {
    register_tests(tests);
  }

  /**
   * Copy constructor deleted
   */
  ValSetInstantConstraint( const ValSetInstantConstraint &) = delete;

  /**
   * Copy assignment operator deleted
   */
  ValSetInstantConstraint & operator=( const ValSetInstantConstraint &) = delete;

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
        (NumValSets > 1) &&
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
 * @tparam DataType Data type of the variable being monitored
 * @tparam NumValSets Number of value-set constraint tests which constitute
 *                    this constraint
 *
 * @pre @p NumValSets cannot be zero.
 */
template<typename DataType, std::size_t NumValSets>
class ValSetTimedConstraint : public Constraint
{
  static_assert(NumValSets > 0, "A constraint with zero tests is not useful.");

 protected:
  const DataType & variable; /**< (--)
    The variable being monitored for violation of the constraints. */
  const double & delta_time; /**< (s)
    Reference to the time-elapsed between calls to update this constraint. */
 public:
  ConstraintTest_ValSetTimed<DataType> tests[NumValSets]; /**< (--)
    The single Constraint-test for this constraint.
    Note -- cannot use a std::array here because Trick's checkpoint-restart
    prevents use of STL-arrays of templated instances. */

  /**
   * Construct the constraint and register its tests with the base class
   *
   * @param[in] variable_ Reference to the variable being monitored
   * @param[in] delta_time_ Reference to the update period of this model
   *
   * @warning The lifetime of @p delta_time_ must be at least as long as
   *          this object. A reference to it is stored internally.
   */
  ValSetTimedConstraint (const DataType & variable_,
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
   * @param[in] variable_ Reference to the variable being monitored
   * @param[in] delta_time_ Reference to the update period of this class
   * @param[in,out] set Constraint set which manages this constraint
   *
   * @warning The lifetime of @p delta_time_ must be at least as long as
   *          this object. A reference to it is stored internally.
   */
  ValSetTimedConstraint (const DataType & variable_,
                         const double & delta_time_,
                         ConstraintSet & set)
    :
    ValSetTimedConstraint (variable_,
                           delta_time_)
  {
    set.constraints.push_back(this);
  }

  /**
   * Copy constructor deleted
   */
  ValSetTimedConstraint( const ValSetTimedConstraint &) = delete;

  /**
   * Copy assignment operator deleted
   */
  ValSetTimedConstraint & operator=( const ValSetTimedConstraint &) = delete;

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
        (NumValSets > 1) &&
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
