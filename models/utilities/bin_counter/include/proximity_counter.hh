/******************************** TRICK HEADER *********************************
PURPOSE:
  (Define structure and function prototypes for counting the number of
  times some variable of type double has a value that falls closest to each
  of a set of target values.)

LIBRARY DEPENDENCY:
  (../src/proximity_counter.cc)

PROGRAMMERS:
  (
    ((Matthew Elmer, Gary Turner) (OSR) (Jun 2023)
      (Refactored content from GNC_PAR; Overhauled grok_bin_counter))
    ((Hansen Lian) (OSR) (July 2026) (Inherits from BinCounter))
  )
*******************************************************************************/
#ifndef CML_PROXIMITY_COUNTER_HH
#define CML_PROXIMITY_COUNTER_HH

#include <cstddef>
#include <string>
#include <vector>
#include <algorithm>

#include "cml/models/utilities/bin_counter/include/bin_counter.hh"

/*****************************************************************************
CML_ProximityCounter
Purpose:
  Receives a value via the insert() command and bins it into one of
  a set of bins, each covering a finite domain.
*****************************************************************************/
class CML_ProximityCounter : public CML_BinCounter {
  protected:
  const CML_BinCounterElement* target_data;  /* (--)
    Array of bins data. For logging compatibility only.*/
 public:
  CML_ProximityCounter();
  explicit CML_ProximityCounter(const std::vector<double> & targets_);
  CML_ProximityCounter(const CML_ProximityCounter&) = delete;
  CML_ProximityCounter& operator=(const CML_ProximityCounter&) = delete;
  using CML_BinCounter::insert;
  void set_data( const std::vector<double> & targets_);
  template <size_t n_targets>
  void set_data( const double (&new_targets)[n_targets])
  {
    std::vector<double> targets_v( new_targets, new_targets+n_targets);
    set_data( targets_v);
  }
  size_t get_ntarget() const {return nbin;}
};
#endif
