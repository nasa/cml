/******************************** TRICK HEADER *********************************
PURPOSE:
  (Define structure and function prototypes for counting the number of
  times some variable of type double has a value that falls closest to each
  of a set of target values.)

ASSUMPTIONS:
  (Target values are monotonically increasing)

PROGRAMMERS:
  (
    ((Matthew Elmer, Gary Turner) (OSR) (Jun 2023)
      (Refactored content from GNC_PAR; Overhauled grok_target_counter))
  )
*******************************************************************************/

#include "../include/proximity_counter.hh"

/*****************************************************************************
Constructors
*****************************************************************************/
CML_ProximityCounter::CML_ProximityCounter()
  :
  targets(),
  targets_ready(false),
  ntarget(),
  target_data(nullptr),
  name()
{}
/****************************************************************************/
CML_ProximityCounter::CML_ProximityCounter(
  const std::vector<double> & targets_)
  :
  CML_ProximityCounter()
{
  set_data(targets_);
}

/*****************************************************************************
set_data
Purpose:
  Sets the edge data for constructing the targets. Deletes all previous
  target structure and counts for those targets.
Options:
 - Pass in a STL-vector of n edge values, there will be (n-1) targets from
   these n values
 - Pass in a C-style array of n edge values, there will be (n-1) targets from
   these n values
 - Pass in an upper limit and a lower limit and the number of targets;
   targets will be equally spaced ebtween the two limits.
*****************************************************************************/
void
CML_ProximityCounter::set_data(const std::vector<double> & targets_)
{
  targets.clear();
  targets_ready = false;

  size_t n_targets_ = targets_.size();
  // Sanity check for number of targets:
  if (n_targets_ < 1) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot count onto fewer than 1 targets.\n"
      "Group (",name,") cannot be used.\n");
    return;
  }

  // Sanity check for monotonicity of targets_:
  for (size_t ii = 1; ii < n_targets_; ii++) {
    if (targets_[ii] <= targets_[ii-1]) {
      CMLMessage::error( __FILE__,__LINE__,
        "Bin targets_ (target-group name : ",name,") are malformed,\n"
        "they are not monotonically increasing.\n"
        "  edge(",ii-1,") = ",targets_[ii-1],"\n"
        "  edge(",ii,  ") = ",targets_[ii],  "\n"
        "Group (",name,") cannot be used.\n");
      return;
    }
  }

  ntarget = n_targets_;
  targets.resize(ntarget);
  targets_ready = true;
  target_data = targets.data();
  for (size_t ii = 0; ii < ntarget; ii++) {
    targets[ii].value = targets_[ii];
    targets[ii].count = 0;
  }
}

/*****************************************************************************
insert
Purpose:Increments the count in the target in which the specified value falls.
Note:
  Check each target on the half-open interval [L, R) except for the last, which
  is a fully closed interval [L, R].
  Processing upward from lower limit requires special treatment of last
  target (all but last upper bound are open).
  Processing downward from upper limit does not (all lower bounds are
  closed) so we do that.
*****************************************************************************/
void
CML_ProximityCounter::insert(double value)
{
  // Consider only values below upper edge and only if model passed sanity
  // check.
  if (!targets_ready) {return;}

  if (value < targets[0].value) {
    targets[0].count++;
  }
  else if (value >= targets[ntarget-1].value) {
    targets[ntarget-1].count++;
  }
  else {
    size_t ix_search = 0;
    while (ix_search < ntarget-2 &&
           value > targets[ix_search+1].value) {
      ix_search++;
    }
    // ix_search is between 0 and ntarget-2
    // value lies between targets[ix_search] and targets[ix_search+1]
    // Find the difference between value and the two bounding target values.
    double delta_low = value - targets[ix_search].value;
    double delta_high = targets[ix_search+1].value - value;
    // increment the count for the closer target.
    if (delta_high > delta_low) {
      targets[ix_search].count++;
    } else {
      targets[ix_search+1].count++;
    }
  }
}
