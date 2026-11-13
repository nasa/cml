/*******************************************************************************

PURPOSE:
   (Manages a set of constraints for evaluating whether a requirement set has
   been satisfied.)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES) (Initial version))
 )
*******************************************************************************/
#include "../include/constraint.hh"
#include "../include/constraint_set.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

/**
 * @note If any of the constraints in this set are disabled, calling this function
 *       will make them permanently unavailable for the duration of the simulation.
 */
void
ConstraintSet::initialize()
{
  constraints.remove_if( constraint_disabled);

  for (Constraint * constraint : constraints) {
    constraint->initialize();
  }
  SubscriptionBase::initialize();
}

// Activate the constraints in this set
void
ConstraintSet::activate()
{
  for (Constraint * constraint : constraints) {
    constraint->activate();
  }
  SubscriptionBase::activate();
}

// Update all constraints and check for violations
void
ConstraintSet::update()
{
  if (!active) {return;}

  num_violations = 0;
  violated_constraints.clear();
  for (Constraint * constraint : constraints) {
    constraint->update();
    if (constraint->violated) {
      num_violations++;
      if (!violated_constraints.empty()) {
        violated_constraints += ", ";
      }
      violated_constraints += constraint->name;
    }
  }
}
