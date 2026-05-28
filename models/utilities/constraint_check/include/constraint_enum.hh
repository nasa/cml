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


struct ConstraintEnum
{
  enum ViolationCondition {
    Undefined,
    EQ,
    NEQ,
    GT,
    GE,
    LT,
    LE,
    In, // Used for bounded intervals and sets
    Out // Used for bounded intervals and sets
  };

  enum IntervalBounds{
    ClosedClosed,
    ClosedOpen,
    OpenClosed,
    OpenOpen
  };
};
#endif
