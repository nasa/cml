/*******************************************************************************

PURPOSE:
   (Defines the enumerations used by multiple classes within this model.)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/
#ifndef CML_CONSTRAINT_ENUM_HH
#define CML_CONSTRAINT_ENUM_HH

/**
 * Enumerations used by the %Constraint Check model classes
 */
struct ConstraintEnum
{
  /**
   * %Constraint test violation criteria
   *
   * Note that not all options will be valid for the specific constraint type
   * being checked.
   */
  enum ViolationCondition {
    Undefined, /**< Uninitialized condition */
    EQ, /**< Equality */
    NEQ, /**< Inequality */
    GT, /**< Greater-than */
    GE, /**< Greater-than or equal-to */
    LT, /**< Less-than */
    LE, /**< Less-than or equal-to */
    In, /**< Inside a bounded interval or set */
    Out /**< Outside a bounded interval or set */
  };

  /**
   * Options for whether the lower and/or upper bound of an interval are
   * considered to be within the interval
   */
  enum IntervalBounds {
    ClosedClosed, /**< Lower and upper bounds are included in the interval */
    ClosedOpen, /**< Lower bound is included in the interval but the upper bound is not */
    OpenClosed, /**< Lower bound is not included in the interval but the upper bound is included */
    OpenOpen /**< Neither the lower bound nor the upper bound are included in the interval */
  };
};
#endif
