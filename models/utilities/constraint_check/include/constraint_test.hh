/*******************************************************************************

PURPOSE:
   (Defines specific constraint tests for checking a sim variable value against
    a domain specification and duration constraint.)

LIBRARY DEPENDENCY:
  ((../src/constraint_test.cc)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES) (Initial version))
 )
******************************************************************************/
#ifndef CML_CONSTRAINT_TEST_HH
#define CML_CONSTRAINT_TEST_HH

#include "constraint_enum.hh"

/**
 * Provides the mechanism for testing some value against some threshold using some
 * specified logical comparison.
 *
 * Multiple constraint tests are combined together to form a constraint.
 */
class ConstraintTest
{
 public:
  bool enabled{true}; /**< (--)
    Flag indicating that the test is intended to be used by the associated
    constraint. */
 protected:
  bool initialized{false}; /**< (--)
    Flag indicating that the test has been successfully initialized. */
  bool active{false}; /**< (--)
    Flag indicating that this test is currently in use within its constraint. */
  ConstraintEnum::ViolationCondition violation_condition{ConstraintEnum::Undefined}; /**< (--)
    The specific violation condition for this test. */
  bool violation{false}; /**<  (--)
    Flag indicating that this test has failed. When used as part of
    @ref TimedConstraint, this flag means that the value has been outside the
    allowable domain for more than the allowable duration. */
  bool prev_violation{false}; /**<  (--)
    Copy of previous value of violation. Used to identify whether a
    violation is "new". */
  unsigned int violation_count{0}; /**<  (1)
    Count of the number of unique violations of this test. */
  double equality_threshold{1e-6}; /**< (--)
    Threshold for evaluating when two values are equal when their data-types
    are floating-point representations. */

 public:
  /**
   * Constructor
   */
  ConstraintTest() = default;

  /**
   * Virtual destructor
   */
  virtual ~ConstraintTest() = default;

  /**
   * Verify that the model has been configured correctly
   *
   * @return `false` if the constraint has been configured improperly
   */
  virtual bool initialize() = 0;

  /* Note -- get_time_limit and get_threshold are needed at the top level
     because they are called for all constraint-tests that show a violation
     within a constraint. They are meaningless for constraint-test types that
     have no time-limit or no single threshold and simply return 0.0 for
     those classes. They are meaningful for constraint-test types:
     - {ConstraintTestTimed} and its derivatives, and
     - {ConstraintTest_Threshold, ConstraintTest_ThresholdTimed}
     respectively, and are redefined in those specific classes.*/

  /**
   * @return The duration for which a domain violation may exist before triggering a
   *         test violation
   *
   * @note This is only meaningful for timed constraints.
   */
  virtual double get_time_limit() const { return 0.0;}

  /**
   * @return The threshold value that, if crossed, will trigger a domain violation
   *
   * @note This is only meaningful for single-threshold-based constraints.
   */
  virtual double get_threshold() const { return 0.0;}

  /**
   * @return `true` if the model is active and checking for violations
   */
  bool is_active() const {return active;}

  /**
   * Activate the model
   */
  void activate() {active = (initialized && enabled);}

  /**
   *
   * @return `true` if the model has been initialized
   */
  bool is_initialized() const {return initialized;}

  /**
   * Set the violation condition to test against
   *
   * @param cond[in] Violation condition option
   */
  void set_violation_condition( ConstraintEnum::ViolationCondition cond);

  /**
   * @return The violation condition being tested against
   */
  ConstraintEnum::ViolationCondition get_violation_condition() const {
                                               return violation_condition;}

  /**
   * @return `true` if the constraint has been violated
   */
  bool get_violation() const {return violation;}

 protected:
  /**
   * Update the counter tracking the number of unique violations
   */
  void count_violations();

  /**
   * Check that the model configuration is valid for a threshold-based constraint test
   */
  void initialize_threshold();

  /**
   * Check that the model configuration is valid for an interval-based constraint test
   */
  void initialize_interval();

  /**
   * Check that the model configuration is valid for a value-set-based constraint test
   */
  void initialize_val_set();
};

/**
 * A constraint check which monitors the duration over which a test violation occurs
 *
 * Extends the base @ref ConstraintTest functionality to trigger violations after a
 * constraint has been violated for a specified period of time.
 */
class ConstraintTestTimed : public ConstraintTest
{
 public:
  bool use_timer{true}; /**< (--)
    Allows a @ref ConstrainTestTimed to be treated as a @ref ConstraintTest,
    circumventing the timing aspect if this is false. */
  double time_limit{0.0}; /**< (s)
    The duration for which a domain violation may exist before triggering a
    test violation. Not used in all cases. */
  double gap_time_limit{0.0}; /**< (s)
    Used if @ref recover_quiet_violation; reset @ref violation_timer to 0 if
    @ref gap_time > this value. */
  double time_limit_tolerance{1E-10}; /**< (s)
    Tolerance used when comparing time limits against accumulated simulation
    time. Used to protect against floating point rounding causing a violation to
    be true a simulation cycle too late or too early when running this model at
    certain update periods. */
  bool recover_quiet_violation{false}; /**< (--)
    If true, the violation timer will be reset after a period of non-violation
    time greater than @ref gap_time_limit. */
 protected:
  bool numerical_violation{false}; /**< (--)
    An interim violation-flag. This identifies whether the numerical
    comparison test indicates a violation this cycle.
    The inherited violation flag is repurposed to represent a numerical
    violation with sufficient persistence. */
  bool existing_numerical_violation{false}; /**< (--)
    Flag indicating that a numerical violation has been identified, but not
    necessarily for a timer-violation. */
  bool checking_gap_timer{false}; /**< (--)
    Flag indicating that there is currently no violation but the gap timer
    is being monitored before resetting the violation-timer. */
  double violation_timer{0.0}; /**< (s)
    Time interval that the numerical-test has been persistently in violation. */
  double gap_timer{0.0}; /**< (s)
    Time interval that the numerical-test has been persistently satisfied. */
  double accumulated_violation_timer{0.0}; /**< (s)
    Accumulated durations for which the numerical-test has been in violation;
    this value does not reset if the numerical-test is satisfied; it exists for
    logging purposes. */

 public:
  /**
   * Constructor
   */
  ConstraintTestTimed() = default;

  /**
   * @return @ref time_limit
   */
  double get_time_limit() const override {return time_limit;}

 protected:
  /**
   * Set the @ref gap_time_limit equal to the @ref time_limit if it has not been set
   * independently
   */
  void initialize_gap_time();

  /**
   * Update the violation timer
   *
   * @param delta_time[in] Elapsed time since the last violation check
   */
  void update_timer(double delta_time);
};
#endif
