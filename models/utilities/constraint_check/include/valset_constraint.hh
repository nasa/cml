/*******************************************************************************

PURPOSE:
   (Define constraints for checking against sim variables)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/
#ifndef CML_VALSET_CONSTRAINT_HH
#define CML_VALSET_CONSTRAINT_HH

#include "constraint.hh"
#include "constraint_test_templates.hh"
#include "constraint_test_timed_templates.hh"
#include "constraint_set.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"

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

/*****************************************************************************
Name: ValSetInstantConstraint
Purpose:
  Uses the interval constraint tests, which are violated when the variable
  is either within or without a specified interval.
  Combining these tests typically results in a constraint violation when
  the variable is either:
  - within any of a set of forbidden intervals, or
  - without all of a set of permissible intervals

  The tests used in this constraint are set to be marked as violated
  immediately upon the variable violating the specified interval bounds.
*****************************************************************************/
template<typename T, size_t NumValSets>
class ValSetInstantConstraint : public Constraint
{
 protected:
  const T & variable; /* (--)
    The variable being monitored for violation of the
    constraints. */
 public:
  ConstraintTest_ValSet<T> tests[NumValSets]; /* (--)
    The single Constraint-test for this constraint.
    Note -- cannot use a std::array here because Trick's checkpoint-restart
    prevents use of STL-arrays of templated instances.*/


  /*******************************************************************
  Constructor / Destructor
  ********************************************************************/
  ValSetInstantConstraint (const T & var)
    :
    Constraint(NumValSets),
    variable(var),
    tests()
  {
    for (auto & test : tests) {
      test_list.push_back(&test);
    }
  }
  virtual ~ValSetInstantConstraint(){};

  /***************************************************************************
  * Name: initialize
  * Purpose: sanity checks
  ***************************************************************************/
  void initialize()
  {
    Constraint::initialize();
    /* If configuration is set to trip a violation when variable is:
     * - OUTSIDE ANY interval, or
     * - INSIDE ALL intervals,
     * there's probably something wrong.*/
    if (initialized &&
        (num_tests > 1) &&
        (violate_on_any_test ==
             (tests[0].violation_condition == ConstraintEnum::Out))) {
      CMLMessage::warn( __FILE__,__LINE__,
        "Possible mismatch on constraint ",name,
        "\nbetween the violate_on_any_test flag (", violate_on_any_test,
        ")\n and the violation_condition setting (",violation_condition,
        ").\nCheck configuration.");
    }
  }
  /***************************************************************************
  * Name: update
  * Purpose: Main executable of this constraint.
  ***************************************************************************/
  void update()
  {
    if (!active) {return;}

    for (auto & test : tests) {
      test.test_violation(variable);
    }
    post_update(); // inherited.
  }

 private: // not implemented; not copyable
  ValSetInstantConstraint( const ValSetInstantConstraint &);
  ValSetInstantConstraint & operator=( const ValSetInstantConstraint &);
};




/*****************************************************************************
Name: ValSetTimedConstraint
Purpose:
  Uses the interval constraint tests, which are violated when the variable
  is either within or without a specified interval.
  Combining these tests typically results in a constraint violation when
  the variable is either:
  - within any of a set of forbidden intervals, or
  - without all of a set of permissible intervals

  The tests used in this constraint are set to be marked as violated
  when the variable persistently violates the specified interval bounds for
  an assigned period of time.
*****************************************************************************/
template<typename T, size_t NumValSets>
class ValSetTimedConstraint : public Constraint
{
 protected:
  const T & variable; /* (--)
    The variable being monitored for violation of the
    constraints. */
  const double & delta_time; /* (s)
    Reference to the time-elapsed between calls to update this constraint.*/
 public:
  ConstraintTest_ValSetTimed<T> tests[NumValSets]; /* (--)
    The single Constraint-test for this constraint.
    Note -- cannot use a std::array here because Trick's checkpoint-restart
    prevents use of STL-arrays of templated instances.*/


  /*******************************************************************
  Constructor / Destructor
  ********************************************************************/
  ValSetTimedConstraint (const T & variable_,
                         const double & delta_time_)
    :
    Constraint(NumValSets),
    variable(variable_),
    delta_time(delta_time_),
    tests()
  {
    for (auto & test : tests) {
      test_list.push_back(&test);
    }
  }
  /*******************************************************************/
  ValSetTimedConstraint (const T & variable,
                         const double & delta_time,
                         ConstraintSet & set)
    :
    ValSetTimedConstraint (variable,
                           delta_time)
  {
    set.constraints.push_back(this);
  }

  virtual ~ValSetTimedConstraint(){};

  /***************************************************************************
  * Name: initialize
  * Purpose: sanity checks
  ***************************************************************************/
  void initialize()
  {
    Constraint::initialize();
    /* If configuration is set to trip a violation when variable is:
     * - OUTSIDE ANY interval, or
     * - INSIDE ALL intervals,
     * there's probably something wrong.*/
    if (initialized &&
        (num_tests > 1) &&
        (violate_on_any_test ==
             (tests[0].get_violation_condition() == ConstraintEnum::Out))) {
      CMLMessage::warn( __FILE__,__LINE__,
        "Possible mismatch on constraint ",name,
        "\nbetween the violate_on_any_test flag (", violate_on_any_test,
        ")\n and the violation_condition setting (",violation_condition,
        ").\nCheck configuration.");
    }
  }
  /***************************************************************************
  * Name: update
  * Purpose: Main executable of this constraint.
  ***************************************************************************/
  void update()
  {
    if (!active) {return;}

    for (auto & test : tests) {
      test.test_violation(variable, delta_time);
    }
    post_update(); // inherited.
  }

 private: // not implemented; not copyable
  ValSetTimedConstraint( const ValSetTimedConstraint &);
  ValSetTimedConstraint & operator=( const ValSetTimedConstraint &);
};
#endif
