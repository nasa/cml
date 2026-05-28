/*******************************************************************************

PURPOSE:
   (Defines specific constraint tests for checking a sim variable value against
    a domain specification and duration constraint.)

LIBRARY DEPENDENCY:
  ((../src/constraint_test.cc)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
******************************************************************************/
#ifndef CML_CONSTRAINT_TEST_HH
#define CML_CONSTRAINT_TEST_HH

#include "constraint_enum.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
ConstraintTest
Purpose:
  Abstract base class of a constraint-test.
  Supports creation of a polymorphic list of constraint-tests.
  Each instance of this classs represents one of the tests in
  evaluating whether a constraint has been violated.
*****************************************************************************/
class ConstraintTest
{
 public:
  bool enabled; /* (--)
    Flag indicating that the test is intended to be used by the associated
    constraint.*/
 protected:
  bool initialized; /* (--)
    Flag indicating that the test has been successfully initialized.*/
  bool active; /* (--)
    Flag indicating that this test is currently in use within its
    constraint.*/
  ConstraintEnum::ViolationCondition violation_condition; /* (--)
    The specific violation condition for this test.*/
  bool violation; /* (--)
    Flag indicates that this test has failed. When used as part of
    TimedConstraint, this flag means that the value has been outside the
    allowable domain for more than the allowable duration.*/
  bool prev_violation; /* (--)
    Copy of previous value of violation. Used to identify whether a
    violation is "new".*/
  unsigned int violation_count; /* (1)
    Count of the number of unique violations of this test.*/
  double equality_threshold; /* (--)
    Threshold for evaluating when two values are equal when their data-types
    are floating-point representations. */

 public:
  ConstraintTest();
  virtual ~ConstraintTest(){};

  virtual bool initialize() = 0;
  void count_violations();


  virtual double get_time_limit() {return 0.0;}
  void activate() {active = (initialized && enabled);}

  bool is_initialized() {return initialized;}
  void set_violation_condition( ConstraintEnum::ViolationCondition cond);
  ConstraintEnum::ViolationCondition get_violation_condition() {
                                               return violation_condition;}
  bool get_violation(){return violation;}
 protected:
  void initialize_threshold();
  void initialize_interval();
  void initialize_val_set();
};


/*****************************************************************************
Name: ConstraintTestTimed
Purpose:
  Abstract extended class of a constraint-test.
  Adds timer capability to the base class.
  Each instance of this classs represents one of the tests in
  Class members associated with timed-tests.
Note:
- Each concrete constraint-test inherits from ConstraintTest or
  ConstraintTestTimed.
- To the extent possible, as much common content has been placed in one of
  these two classes. Inheritance from them into the constraint-test
  template-classes has to be duplicated to avoid complications of inheriting
  from a class-template which Trick-SWIG does not suppoort well.
*****************************************************************************/
class ConstraintTestTimed : public ConstraintTest
{
 public:
  bool use_timer; /* (--)
    Allows a ConstrainTestTimed to be treated as a ConstraintTest,
    circumventing the timing aspect.*/
  double time_limit; /* (--)
    The duration for which a domain violation may exist before triggering a
    test violation. Not used in all cases.*/
  double gap_time_limit; /* (s)
    Used if recover_quiet_violation; reset violation timer to 0 if
    gap_time > this value.*/
  bool recover_quiet_violation; /* (--)
    Flag that allows a tagged violation to reset its accumulated violation
    time after a period of non-violation. */
 protected:
  bool numerical_violation; /* (--)
    An interim violation-flag. This identifies whether the numerical
    comparison test indicates a violation.
    The inherited violation flag is repurposed to represent a numerical
    violation with sufficient persistence.*/
  bool existing_numerical_violation; /* (--)
    Flag indicating that a numerical violation has been identified, but not
    necessarily for a timer-violation.*/
  bool checking_gap_timer; /* (--)
    Flag indicating that there is currently no violation but the gap timer
    is being monitored before resetting the violation-timer.*/
  double violation_timer; /* (s)
    Time interval that the numerical-test has been persistently in violation.*/
  double gap_timer; /* (s)
    Time interval that the numerical-test has been persistently satisfied.*/
  double accumulated_violation_timer; /* (s)
    Accumulated durations for which the numerical-test has been in violation;
    this value does not reset if the numerical-test is satisfied.*/

  ConstraintTestTimed();
  virtual ~ConstraintTestTimed(){};

  double get_time_limit() override {return time_limit;}
  void initialize_gap_time();
  void update_timer(double delta_time);
};
#endif
