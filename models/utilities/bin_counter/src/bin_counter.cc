/******************************** TRICK HEADER *********************************
PURPOSE:
  (Define structure and function prototypes for counting the number of
  times some variable of type double has a value that falls into specified bins

ASSUMPTIONS:
  (Bin edges are monotonically increasing and bins are numerically adjacent.)

PROGRAMMERS:
  (
    ((Matthew Elmer, Gary Turner) (OSR) (Jun 2023)
      (Refactored content from GNC_PAR; Overhauled grok_bin_counter))
  )
*******************************************************************************/

#include <limits>
#include "../include/bin_counter.hh"

/*****************************************************************************
Constructors
*****************************************************************************/
CML_BinCounter::CML_BinCounter()
  :
  bins(),
  bins_ready(false),
  nbin(),
  bin_data(nullptr),
  name()
{}
/****************************************************************************/
CML_BinCounter::CML_BinCounter(
  const std::vector<double> & edges,
  bool closed_ends)
  :
  CML_BinCounter()
{
  set_data(edges, closed_ends);
}
/****************************************************************************/
CML_BinCounter::CML_BinCounter(
  double limit_a,
  double limit_b,
  unsigned int num_bins,
  bool closed_ends)
  :
  CML_BinCounter()
{
  set_data( limit_a, limit_b, num_bins, closed_ends);
}

/*****************************************************************************
set_data
Purpose:
  Sets the edge data for constructing the bins. Deletes all previous
  bin structure and counts for those bins.
Options:
 - Pass in a STL-vector of n edge values, there will be (n-1) bins from
   these n values
 - Pass in a C-style array of n edge values, there will be (n-1) bins from
   these n values
 - Pass in an upper limit and a lower limit and the number of bins;
   bins will be equally spaced ebtween the two limits.
*****************************************************************************/
void
CML_BinCounter::set_data(const std::vector<double> & edges,
                         bool closed_ends)
{
  bins.clear();
  bins_ready = false;

  size_t n_edges = edges.size();
  // Sanity check for number of bins:
  if (n_edges <= 1 && closed_ends) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot count into fewer than 1 bin.\n"
      "Bin group (",name,") cannot be used.\n");
    return;
  }

  // Sanity check for monotonicity of edges:
  for (size_t ii = 1; ii < n_edges; ii++) {
    if (edges[ii] <= edges[ii-1]) {
      CMLMessage::error( __FILE__,__LINE__,
        "Bin edges (bin-group name : ",name,") are malformed,\n"
        "they are not monotonically increasing.\n"
        "  edge(",ii-1,") = ",edges[ii-1],"\n"
        "  edge(",ii,  ") = ",edges[ii],  "\n"
        "Bin group cannot be used.\n");
      return;
    }
  }

  if (closed_ends) {
    nbin = n_edges - 1;
    bins.resize(nbin);

    bins[0].bin_floor = edges[0];
    for (size_t ii = 0; ii < nbin-1; ii++) {
      bins[ii].bin_ceil =
      bins[ii+1].bin_floor = edges[ii+1];
      bins[ii].count = 0;
    }
    bins[nbin-1].bin_ceil = edges[n_edges-1];
    bins[nbin-1].count = 0;
  }
  else {
    nbin = n_edges + 1;
    bins.resize(nbin);

    bins[0].bin_floor = std::numeric_limits<double>::lowest();
    for (size_t ii = 0; ii < nbin-1; ii++) {
      bins[ii].bin_ceil =
      bins[ii+1].bin_floor = edges[ii];
      bins[ii].count = 0;
    }
    bins[nbin-1].bin_ceil = std::numeric_limits<double>::max();
    bins[nbin-1].count = 0;
  }
  bins_ready = true;
  bin_data = bins.data();
}
/*****************************************************************************/
void
CML_BinCounter::set_data( double limit_a,
                          double limit_b,
                          unsigned int num_bins,
                          bool closed_ends)
{
  bins.clear();
  bins_ready = false;

  // Sanity check for number of bins:
  if (num_bins == 0) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot count into fewer than 1 bin.\n"
      "Bin group (",name,") cannot be used.\n");
    return;
  }

  double interval;
  double lower_limit;
  double upper_limit;
  if (limit_b > limit_a) {
    interval = (limit_b - limit_a) / num_bins;
    lower_limit = limit_a;
    upper_limit = limit_b;
  }
  else if ( limit_b < limit_a) {
    interval = (limit_a - limit_b) / num_bins;
    lower_limit = limit_b;
    upper_limit = limit_a;
  }
  else {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot count into an infinitesimal domain.\n"
      "Bin group (",name,") cannot be used.\n");
    bins_ready = false;
    return;
  }

  if (closed_ends) {
    nbin = num_bins;
    bins.resize(nbin);

    bins[0].bin_floor = lower_limit;
    for (size_t ii = 0; ii < nbin-1; ii++) {
      bins[ii].bin_ceil    =
      bins[ii+1].bin_floor = lower_limit + (ii+1) * interval;
      bins[ii].count = 0;
    }
    bins[nbin-1].bin_ceil = upper_limit;
    bins[nbin-1].count = 0;
  }
  else {// produce the same num_bins bins, but add overflow bins one either end.
    nbin = num_bins+2;
    bins.resize(nbin);

    bins[0].bin_floor = std::numeric_limits<double>::lowest();
    for (size_t ii = 0; ii < nbin-1; ii++) {
      bins[ii].bin_ceil =
      bins[ii+1].bin_floor = lower_limit + ii * interval;
      bins[ii].count = 0;
    }
    bins[nbin-1].bin_ceil = std::numeric_limits<double>::max();
    bins[nbin-1].count = 0;
  }
  bins_ready = true;
  bin_data = bins.data();
}


/*****************************************************************************
insert
Purpose:Increments the count in the bin in which the specified value falls.
Note:
  Check each bin on the half-open interval [L, R) except for the last, which
  is a fully closed interval [L, R].
  Processing upward from lower limit requires special treatment of last
  bin (all but last upper bound are open).
  Processing downward from upper limit does not (all lower bounds are
  closed) so we do that.
*****************************************************************************/
void
CML_BinCounter::insert(double value)
{
  // Consider only values below upper edge and only if model passed sanity
  // check.
  if (bins_ready && value <= bins[nbin-1].bin_ceil) {
    for (int ii = nbin - 1; ii >= 0; ii--) {
      if (value >= bins[ii].bin_floor) {
        bins[ii].count++;
        return;
      }
    }
  }
}
