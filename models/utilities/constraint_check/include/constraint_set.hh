/*******************************************************************************

PURPOSE:
   (Manages a set of constraints for evaluating whether a requirement set has
   been satisfied.)

LIBRARY DEPENDENCY:
  ((../src/constraint_set.cc)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES) (Initial version))
 )

*******************************************************************************/
#ifndef CML_CONSTRAINT_SET_HH
#define CML_CONSTRAINT_SET_HH

#include "constraint.hh"
#include <list>
#include <string>

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

/**
 * Manages a set of constraints
 *
 * Intended as a base class, to be extended for a particular purpose.
 * The extended class would implement instances of various constraints and --
 * at the construction of those constraint instances -- have them loaded onto
 * this base-class's constraints list. Additionally, the extended class would
 * provide -- either by calculation or pass-through -- a value to be referenced
 * by this class for the time interval between constraint-checking.
 */
class ConstraintSet : public SubscriptionBase
{
  protected:
  unsigned int num_violations{0}; /**< (--)
    Counts the number of constraints violated in this timestep.
    Do not confuse with @ref Constraint::violation_count, which counts the number
    of occurrences of a specific constraint violation. across all cycles. */
  std::string violated_constraints; /**< (--)
    Comma-separated list of the names of all constraints in the set that are
    violated on this timestep .*/

  /**
   * Activate the constraints in this set
   *
   * Called at subscription time
   */
  void activate() override;

  public:
  std::list< Constraint *> constraints; /**< (--)
    Set of pointers to the instances of @ref Constraint being managed by this
    class. Constraints should be instantiated independently and added to the
    set. */

  /**
   * Constructor
   */
  ConstraintSet() = default;

  /**
   * Copy constructor deleted
   */
  ConstraintSet( ConstraintSet &) = delete;

  /**
   * Copy assignment operator deleted
   */
  ConstraintSet &operator= ( ConstraintSet &) = delete;

  /**
   * Initialize each constraint and the set itself
   */
  void initialize() override;

  /**
   * Update all constraints and check for violations
   */
  void update();

  /**
   * @param constraint[in] Constraint to check
   * @return `true` if the provided constraint is disabled
   * @pre @constraint cannot be null
   */
  static bool constraint_disabled( const Constraint * constraint) {return !constraint->enabled;}

  /**
   * @return String of comma-separated constraint names which were violated this timestep
   */
  const std::string & get_violated_constraints() const { return violated_constraints;}
};
#endif
