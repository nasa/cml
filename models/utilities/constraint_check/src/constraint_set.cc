/*******************************************************************************

PURPOSE:
   (Manages a set of constraints for evaluating whether a requirement set has
   been satisfied.)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/
#include "../include/constraint_set.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
ConstraintSet::ConstraintSet()
  :
  num_violations(0),
  constraints()
{}

/*****************************************************************************
initialize
Purpose: Initializes the constraint-set and all its constraints
*****************************************************************************/
void
ConstraintSet::initialize()
{
  constraints.remove_if( ConstraintSet::constraint_disabled);

  for (Constraint * constraint : constraints) {
    constraint->initialize();
  }
  SubscriptionBase::initialize();
}

/*****************************************************************************
activate
Purpose:
  Called at subscription, initializes the time.
*****************************************************************************/
void
ConstraintSet::activate()
{
  for (Constraint * constraint : constraints) {
    constraint->activate();
  }
  SubscriptionBase::activate();
}

/*****************************************************************************
update
Purpose: Checks all constraints.
*****************************************************************************/
void
ConstraintSet::update()
{
  if (!active) {return;}

  num_violations = 0;
  for (Constraint * constraint : constraints) {
    constraint->update();
    num_violations += constraint->violated;
  }
}

/*****************************************************************************
constraint_disabled
Purpose: returns whether a cosntraint has been disabled.
*****************************************************************************/
bool
ConstraintSet::constraint_disabled( const Constraint * c)
{
  return !(c->enabled);
}
