/******************************** TRICK HEADER *********************************
PURPOSE:
  (Define structure and function prototypes for counting the number of
  times some integer variable has specific values.)

PROGRAMMERS:
  (
    ((Matthew Elmer, Gary Turner) (OSR) (Jun 2023)
      (Refactored content from GNC_PAR; Overhauled grok_bin_counter))
  )
*******************************************************************************/

#include <algorithm> // any_of, find_if
#include "../include/target_counter.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
CML_TargetCounter::CML_TargetCounter()
  :
  targets(),
  ntarget(0),
  target_data(nullptr),
  name()
{}
/****************************************************************************/
CML_TargetCounter::CML_TargetCounter(
  const std::vector<int> & targets_)
  :
  CML_TargetCounter()
{
  set_data( targets_);
}
/****************************************************************************/
CML_TargetCounter::CML_TargetCounter(
  int limit_a,
  int limit_b)
  :
  CML_TargetCounter()
{
  set_data( limit_a, limit_b);
}

/*****************************************************************************
set_data
Purpose:
  Sets the target values for counting hits. Deletes all previous
  target values and counts for those targets.
Options:
 - Pass in a STL-vector of n target values
 - Pass in a C-style array of n target values
 - Pass in an upper limit and a lower limit; the model will assign a
   target value for every integer between the limits, including both limits.
*****************************************************************************/
void
CML_TargetCounter::set_data(const std::vector<int> & targets_)
{
  targets.clear();
  ntarget = 0;

  size_t num_targets = targets_.size();
  // Sanity check for number of targets:
  if (num_targets == 0) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot count onto fewer than 1 target.\n"
      "Target counter (",name,") cannot be used.\n");
    return;
  }

  // Resize to accommodate all of the members of targets_, but then check for
  // uniqueness, strip any that are duplicates, and resize again.
  targets.resize(num_targets);
  for (size_t i_src = 0; i_src < num_targets; ++i_src) {
    // Check for uniqueness:
    int src = targets_[i_src];
    if (!does_val_exist(src)) {
      targets[ntarget].value = src;
      ntarget++;
    }
  }
  targets.resize(ntarget);
  target_data = targets.data();
}
/****************************************************************************/
void
CML_TargetCounter::set_data( int limit_a,
                             int limit_b)
{
  targets.clear();
  ntarget = 0;

  int lower_limit = std::min( limit_a, limit_b);
  int upper_limit = std::max( limit_a, limit_b);

  ntarget = upper_limit - lower_limit + 1;
  targets.resize(ntarget);
  target_data = targets.data();
  for (size_t ii = 0; ii < ntarget; ++ii) {
    targets[ii].value = lower_limit + ii;
  }
}

/*****************************************************************************
insert
Purpose: Increments the count for the specific target.
*****************************************************************************/
void
CML_TargetCounter::insert(int value)
{
  /* Note:
   *   Can use std::find_if here; using a lambda expression for the unary
   *   predicate
   *     - capture the input value
   *     - test each target's value against the captured "value"; if it
   *       matches, identify the target.
   *     - If a target was identified, increment its count.
   */
  auto it = std::find_if( targets.begin(), targets.end(),
                          [value](CML_TargetCounterElement & target) {
                            return (target.value == value);}
                        );
  if (it != targets.end()) { (it->count)++;}
}

/*****************************************************************************
does_val_exist
Purpose: Checks whether the specified value is in the targets vector
*****************************************************************************/
bool
CML_TargetCounter::does_val_exist(int val)
{

  /* Note: can use std::any_of here.
   *       We only need to test until one target (any target) meets the
   *       condition, and then we can return true. If we get to the end
   *       without finding any targets, return false. */
  if (std::any_of( targets.begin(), targets.end(),
      [val](CML_TargetCounterElement & target_)
                                          {return (val == target_.value);}))
  {
    return true;
  }
  return false;
}
