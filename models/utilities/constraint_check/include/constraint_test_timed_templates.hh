/*******************************************************************************

PURPOSE:
   (Defines specific constraint tests for checking a sim variable value against
    a domain specification and duration constraint.)

LIBRARY DEPENDENCY:
  ((../src/constraint_test_timed_templates.cc)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/
#ifndef CML_CONSTRAINT_TEST_TIMED_TEMPLATE_HH
#define CML_CONSTRAINT_TEST_TIMED_TEMPLATE_HH

#include <list>
#include <algorithm> // any_of

#include "constraint_enum.hh"
#include "constraint_test.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
Notes:
 - The classes defined in here are class-template extensions of
   ConstraintTestTimed, and so include timer capabilities.
 - To the extent possible, as much common content has been placed in one of
   ConstraintTest and ConstraintTestTimed. Inheritance from these two
   classes into the constraint-test template-classes has to be duplicated to
   avoid complications of inheriting from a class-template (which Trick-SWIG
   does not suppoort well). Consequently, there is significant ismilarity
   between this file and constraint_test_templates.hh (which defines
   the class-templates inheriting from ConstraintTest).
 - By settting the flag "use_timer" to false, the class-templates defined
   herein will function identically to those found in
   constraint_test_templates.hh, making those somewhat redundant. However,
   these templates carry significant additional overhead so  it was decided
   to keep both timed and non-timed constraint-tests defined to provide a
   low-overhead implementation for situations in which timing is not needed.
*****************************************************************************/


/*****************************************************************************
Name: ConstraintTest_ThresholdTimed
Purpose:
  A constraint test evaluating the variable against a threshold
  with violation occurring when the threshold has been exceeded for a
  specified interval of time.
*****************************************************************************/
template <typename T>
class ConstraintTest_ThresholdTimed : public ConstraintTestTimed
{
 public:
  T threshold; /* (--)
    The threshold value that, if crossed, will trigger a domain violation.*/

  /***************************************************************************
  * Constructor/Destructor
  ***************************************************************************/
  ConstraintTest_ThresholdTimed()
    :
    ConstraintTestTimed(),
    threshold()
  {}
  virtual ~ConstraintTest_ThresholdTimed(){};

  /***************************************************************************
  * Name: initialize
  * Purpose: checks for configuration.
  ***************************************************************************/
  bool initialize() override
  {
    initialize_threshold();
    initialize_gap_time();
    return true;
  }

  /****************************************************************************
  * test_violation
  * Purpose: evaluates for violation of the constraint.
  ****************************************************************************/
  bool test_violation (T variable,
                       double delta_time)
  {
    if (!active) { return false;}

    switch (violation_condition) {
    case ConstraintEnum::EQ:
      numerical_violation = query_equals(variable);
      break;
    case ConstraintEnum::NEQ:
      numerical_violation = !query_equals(variable);
      break;
    case ConstraintEnum::GT:
      numerical_violation = (variable >  threshold);
      break;
    case ConstraintEnum::GE:
      numerical_violation = (variable >= threshold);
      break;
    case ConstraintEnum::LT:
      numerical_violation = (variable <  threshold);
      break;
    case ConstraintEnum::LE:
      numerical_violation = (variable <= threshold);
      break;
    default:
      CMLMessage::error( __FILE__,__LINE__,
        "Invalid setting of numerical_violation_condition: ",
        violation_condition,"\nDeactivating condition test.\n");
      active = false;
    }
    if (use_timer) {
      update_timer(delta_time);
    } else {
      violation = numerical_violation;
    }
    return violation;
  }
  /****************************************************************************
  * query_equals
  * Purpose: evaluates (variable == threshold)
  * Note: This method is specialized for datatypes float and double to avoid
  *       testing exact equality of two floating-point numbers.
  *       Template specializations have to go into a src file or Trick gets
  *       confused.
  ****************************************************************************/
 private:
  bool query_equals( T variable)
  {
    return (variable == threshold);
  }
};
// Forward declaration of template specializations:
template<> bool ConstraintTest_ThresholdTimed<double>::query_equals(double);
template<> bool ConstraintTest_ThresholdTimed<float>::query_equals(float);


/*****************************************************************************
Name: ConstraintTest_IntervalTimed
Purpose:
  Evaluates the variable for being within (or without) a specified interval.
  with violation occurring when the threshold has been exceeded for a
  specified interval of time.
*****************************************************************************/
template <typename T>
class ConstraintTest_IntervalTimed : public ConstraintTestTimed
{
 public:
  T lower_bound; /* (--)
    The threshold value that, if crossed, will trigger a domain violation.*/
  T upper_bound; /* (--)
  */
  ConstraintEnum::IntervalBounds interval_bounds;

  /***************************************************************************
  * Constructor/Destructor
  ***************************************************************************/
  ConstraintTest_IntervalTimed()
    :
    ConstraintTestTimed(),
    lower_bound(0),
    upper_bound(0),
    interval_bounds( ConstraintEnum::ClosedClosed)
  {}
  virtual ~ConstraintTest_IntervalTimed(){};

  /***************************************************************************
  * Name: initialize
  * Purpose: checks for configuration.
  ***************************************************************************/
  bool initialize() override
  {
    /* Note: cannot genericize the upper/lower bounds checks because they
     * require the templated data types.*/
    if (upper_bound < lower_bound) {
      T temporary = upper_bound;
      upper_bound = lower_bound;
      lower_bound = temporary;
    }

    if (upper_bound == lower_bound) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot initialize an interval constraint-test with a \n"
        "zero-width interval. Bounds are set to:\n",
        lower_bound," and ",upper_bound,".\n"
        "Initialization failed.\n");
      return false;
    }
    initialize_interval();
    initialize_gap_time();
    return true;
  }

  /****************************************************************************
  * test_violation
  * Purpose: evaluates for violation of the constraint.
  ****************************************************************************/
  bool test_violation (T variable,
                       double delta_time)
  {
    if (!active) { return false;}

    bool in_interval;
    switch (interval_bounds) {
    case ConstraintEnum::ClosedClosed:
      in_interval = (variable >= lower_bound) && (variable <= upper_bound);
      break;
    case ConstraintEnum::ClosedOpen:
      in_interval = (variable >= lower_bound) && (variable < upper_bound);
      break;
    case ConstraintEnum::OpenClosed:
      in_interval = (variable > lower_bound) && (variable <= upper_bound);
      break;
    case ConstraintEnum::OpenOpen:
      in_interval = (variable > lower_bound) && (variable < upper_bound);
      break;
    }

    if (use_timer) {
      numerical_violation =
                  (in_interval == (violation_condition == ConstraintEnum::In));
      update_timer(delta_time);
    } else {
      violation = (in_interval == (violation_condition == ConstraintEnum::In));
    }
    return violation;
  }
};


/*****************************************************************************
Name: ConstraintTest_ValSetTimed
Purpose:
  Evaluates the variable against a set of allowable (or non-allowable) values
  with violation occurring when the threshold has been exceeded for a
  specified interval of time.
*****************************************************************************/
template <typename T>
class ConstraintTest_ValSetTimed : public ConstraintTestTimed
{
 public:
  std::list<T> values; /* (--)
    The */

  /***************************************************************************
  * Constructor/Destructor
  ***************************************************************************/
  ConstraintTest_ValSetTimed()
    :
    ConstraintTestTimed(),
    values()
  {}
  virtual ~ConstraintTest_ValSetTimed(){};

  /***************************************************************************
  * Name: initialize
  * Purpose: checks for configuration.
  ***************************************************************************/
  bool initialize() override
  {
    if (values.empty()) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot initialize a value-set constraint-test with an empty \n"
        "set of values to test against.\nInitialization failed.\n");
      return false;
    }
    initialize_val_set();
    initialize_gap_time();
    return true;
  }

  /****************************************************************************
  * test_violation
  * Purpose:
  *   evaluates for violation of the constraint. Checks for whether the
  *   variable (provided in the argument) value is equal to one of the values
  *   in the "values" list. The result of this test is combined with the
  *   configuration for what constitutes a violation (i.e. violation if it is
  *   in the list vs. violation if it is not) to determine whether the
  *   provided values constitutes a violation.
  ****************************************************************************/
  bool test_violation (T variable,
                       double delta_time)
  {
    if (!active) { return false;}

    /* Check each of the "values" list elements until a match is found using
     * any_of. If no match is found, in_set is set to false.*/
    bool in_set =
      std::any_of( values.begin(), values.end(),
        [variable](T & val_) {return (variable == val_);});

    if (use_timer) {
      numerical_violation =
                       (in_set == (violation_condition == ConstraintEnum::In));
      update_timer(delta_time);
    } else {
      violation = (in_set == (violation_condition == ConstraintEnum::In));
    }
    return violation;
  }
};
#endif
