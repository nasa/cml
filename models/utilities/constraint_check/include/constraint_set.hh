/*******************************************************************************

PURPOSE:
   (Manages a set of constraints for evaluating whether a requirement set has
   been satisfied.)

LIBRARY DEPENDENCY:
  ((../src/constraint_set.cc)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )

*******************************************************************************/
#ifndef CML_CONSTRAINT_SET_HH
#define CML_CONSTRAINT_SET_HH

#include "constraint.hh"
#include <list>

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

/*****************************************************************************
Name: ConstraintSet
Purpose:
  A base-class implementation of a class for maanging a set of constraints.
Notes:
- This is intended as a base-class, to be extended for a particular purpose.
  The extended class would implement instances of various constraints and --
  at the construction of those constraint instances -- have them loaded onto
  this base-class's constraints list. Additionally, the extended class would
  provide -- either by calculation or pass-through -- a value to be referenced
  by this class for the time interval between constraint-checking.
- Despite the intent, this class remains a stand-alone, instantiable class.
*****************************************************************************/
class ConstraintSet : public SubscriptionBase
{
 protected:
  unsigned int num_violations; /* (--)
    Counts the number of constraints violated in each cycle.
    Do not confuse with ConstraintBase::violate_count, which counts the number
    of occurrences of a specific constraint violation. across all cycles.*/

 public:
  std::list< Constraint *> constraints; /* (--)
    Set of pointers to the instances of ConstraintBase being managed by this
    class. Constraints should be instantiated independently and added to the
    set.*/

  ConstraintSet();
  virtual ~ConstraintSet(){};
  void initialize();
  void update();
  void activate();
  static bool constraint_disabled( const Constraint * c);

 private: // not implemented, not copyable.
  ConstraintSet( ConstraintSet &);
  ConstraintSet &operator= ( ConstraintSet &);
};
#endif
