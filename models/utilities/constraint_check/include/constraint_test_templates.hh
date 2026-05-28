/*******************************************************************************

PURPOSE:
   (Defines specific constraint tests for checking a sim variable value against
    a domain specification and duration constraint.)

LIBRARY DEPENDENCY:
  ((../src/constraint_test_templates.cc)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/
#ifndef CML_CONSTRAINT_TEST_INSTANT_TEMPLATE_HH
#define CML_CONSTRAINT_TEST_INSTANT_TEMPLATE_HH

#include <list>
#include <algorithm> // any_of

#include "constraint_enum.hh"
#include "constraint_test.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
Notes:
 - The classes defined in here are class-template extensions of
   ConstraintTest. They do not include timer capabilities.
 - To the extent possible, as much common content has been placed in one of
   ConstraintTest and ConstraintTestTimed. Inheritance from these two
   classes into the constraint-test template-classes has to be duplicated to
   avoid complications of inheriting from a class-template (which Trick-SWIG
   does not support well). Consequently, there is significant ismilarity
   between this file and constraint_test_timed_templates.hh (which defines
   the class-templates inheriting from ConstraintTestTimed).
*****************************************************************************/


/*****************************************************************************
Name: ConstraintTest_Threshold
Purpose:
  A constraint test evaluating the variable against a threshold
  This is the most common type of constraint-test.
*****************************************************************************/
template <typename T>
class ConstraintTest_Threshold : public ConstraintTest
{
 public:
  T threshold; /* (--)
    The threshold value that, if crossed, will trigger a domain violation.*/

  /***************************************************************************
  * Constructor/Destructor
  ***************************************************************************/
  ConstraintTest_Threshold()
    :
    ConstraintTest(),
    threshold()
  {}
  virtual ~ConstraintTest_Threshold(){};

  /***************************************************************************
  * Name: initialize
  * Purpose: checks for configuration.
  ***************************************************************************/
  bool initialize() override
  {
    initialize_threshold();
    return true;
  }

  /****************************************************************************
  * test_violation
  * Purpose: evaluates for violation of the constraint.
  ****************************************************************************/
  bool test_violation (T variable)
  {
    if (!active) { return false;}

    switch (violation_condition) {
    case ConstraintEnum::EQ:
      violation = query_equals(variable);
      break;
    case ConstraintEnum::NEQ:
      violation = !query_equals(variable);
      break;
    case ConstraintEnum::GT:
      violation = (variable >  threshold);
      break;
    case ConstraintEnum::GE:
      violation = (variable >= threshold);
      break;
    case ConstraintEnum::LT:
      violation = (variable <  threshold);
      break;
    case ConstraintEnum::LE:
      violation = (variable <= threshold);
      break;
    default:
      CMLMessage::error( __FILE__,__LINE__,
        "Invalid setting of numerical_violation_condition: ",
        violation_condition,"\nDeactivating condition test.\n");
      active = false;
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
template<> bool ConstraintTest_Threshold<double>::query_equals(double);
template<> bool ConstraintTest_Threshold<float>::query_equals(float);


/*****************************************************************************
Name: ConstraintTest_Interval
Purpose:
  Evaluates the variable for being within (or without) a specified interval.
*****************************************************************************/
template <typename T>
class ConstraintTest_Interval : public ConstraintTest
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
  ConstraintTest_Interval()
    :
    ConstraintTest(),
    lower_bound(0),
    upper_bound(0),
    interval_bounds( ConstraintEnum::ClosedClosed)
  {}
  virtual ~ConstraintTest_Interval(){};

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
    return true;
  }

  /****************************************************************************
  * test_violation
  * Purpose: evaluates for violation of the constraint.
  ****************************************************************************/
  bool test_violation (T variable)
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
    violation = (in_interval == (violation_condition == ConstraintEnum::In));
    return violation;
  }
};


/*****************************************************************************
Name:ConstraintTest_ValSet
Purpose:
  Evaluates the variable against a set of allowable (or non-allowable) values
*****************************************************************************/
template <typename T>
class ConstraintTest_ValSet : public ConstraintTest
{
 public:
  std::list<T> values; /* (--)
    The */

  /***************************************************************************
  * Constructor/Destructor
  ***************************************************************************/
  ConstraintTest_ValSet()
    :
    ConstraintTest(),
    values()
  {}
  virtual ~ConstraintTest_ValSet(){};

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
  bool test_violation (T variable)
  {
    if (!active) { return false;}

    /* Check each of the "values" list elements until a match is found using
     * any_of. If no match is found, in_set is set to false.*/
    bool in_set =
      std::any_of( values.begin(), values.end(),
        [variable](T & val_) {return (variable == val_);});

    violation = (in_set == (violation_condition == ConstraintEnum::In));
    return violation;
  }
};
#endif
