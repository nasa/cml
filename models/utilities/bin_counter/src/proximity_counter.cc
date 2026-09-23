/******************************** TRICK HEADER *********************************
PURPOSE:
  (Define structure and function prototypes for counting the number of
  times some variable of type double has a value that falls closest to each
  of a set of target values.)

LIBRARY DEPENDENCIES:
  ((cml/models/utilities/cml_message/src/cml_message.cc))

ASSUMPTIONS:
  (Target values are monotonically increasing)

PROGRAMMERS:
  (
    ((Matthew Elmer, Gary Turner) (OSR) (Jun 2023)
      (Refactored content from GNC_PAR; Overhauled grok_target_counter))
      ((Hansen Lian) (OSR) (July 2026) (Inherits from BinCounter))
  )
*******************************************************************************/

#include "../include/proximity_counter.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include <cstddef>
#include <vector>
#include <limits>

/*****************************************************************************
Constructors
*****************************************************************************/
CML_ProximityCounter::CML_ProximityCounter()
  :
  CML_BinCounter(),
  target_data(nullptr)
{}
/****************************************************************************/
CML_ProximityCounter::CML_ProximityCounter(
  const std::vector<double> & targets_)
  :
  CML_ProximityCounter()
{
  CML_ProximityCounter::set_data(targets_);
}

/*****************************************************************************
set_data
Purpose:
  Sets the edge data for constructing the targets. Deletes all previous
  target structure and counts for those targets.
Options:
 - Pass in a STL-vector of n target values, there will be (n) targets from
   these n values with bins spaced at the midpoint of consecutive targets.
 - Pass in a C-style array of n target values, there will be (n) targets from
   these n values with bins spaced at the midpoint of consecutive target
*****************************************************************************/
void
CML_ProximityCounter::set_data(const std::vector<double> & targets_)
{
  bins.clear();
  bins_ready = false;

  const size_t n_targets_ = targets_.size();
  // Sanity check for number of targets:
  if (n_targets_ < 1) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot count onto fewer than 1 targets.\n"
      "Group (",name,") cannot be used.\n");
    return;
  }


  std::vector<double> sorted_targets = targets_;

  // Sort the local copy
  std::sort(sorted_targets.begin(), sorted_targets.end());

  nbin = n_targets_;
  bins.resize(nbin);
  bins_ready = true;
  bin_data = target_data = bins.data();
  bins[0].bin_floor = std::numeric_limits<double>::lowest();
  for (size_t ii = 0; ii < nbin-1; ii++) {
    bins[ii].value = sorted_targets[ii];
    bins[ii].count = 0;
    bins[ii].bin_ceil =
    bins[ii+1].bin_floor = (sorted_targets[ii] + sorted_targets[ii+1])/2;
  }
  bins[nbin - 1].value = sorted_targets[nbin-1];
  bins[nbin - 1].count = 0;
  bins[nbin - 1].bin_ceil = std::numeric_limits<double>::max();
}
