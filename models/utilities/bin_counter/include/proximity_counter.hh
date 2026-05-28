/******************************** TRICK HEADER *********************************
PURPOSE:
  (Define structure and function prototypes for counting the number of
  times some variable of type double has a value that falls closest to each
  of a set of target values.)

ASSUMPTIONS:
  (Target values are monotonically increasing)

LIBRARY DEPENDENCY:
  (../src/proximity_counter.cc)

PROGRAMMERS:
  (
    ((Matthew Elmer, Gary Turner) (OSR) (Jun 2023)
      (Refactored content from GNC_PAR; Overhauled grok_bin_counter))
  )
*******************************************************************************/
#ifndef CML_PROXIMITY_COUNTER_HH
#define CML_PROXIMITY_COUNTER_HH

#include <vector>
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
CML_ProximityCounterElement
Purpose: Contents of each element of the working vector
*****************************************************************************/
struct CML_ProximityCounterElement {
  double value; /* (--)
    Target value to be counted */
  unsigned int count; /* (--)
    Counter. Incremented when value is the closest one among 
    CML_ProximityCounterElement vector to the variable. */
};


/*****************************************************************************
CML_ProximityCounter
Purpose:
  Receives a value via the insert() command and bins it into one of
  a set of bins, each covering a finite domain.
*****************************************************************************/
class CML_ProximityCounter {
 protected:
  std::vector<CML_ProximityCounterElement> targets;  /* (--)
    The target values. */
  bool targets_ready; /* (--)
    Check on presence of data in the targets array.*/
  size_t ntarget;  /* (--)
    Number of targets.*/
  const CML_ProximityCounterElement* target_data;  /* (--)
    Array of targets data. For logging compatibility only.*/

 public:
  std::string name; /* (--)
    Name of group (for debugging purposes)*/
  CML_ProximityCounter();
  explicit CML_ProximityCounter(const std::vector<double> & targets_);
  void insert(double);
  void set_data( const std::vector<double> & targets);
  template <size_t n_targets>
  void set_data( const double (&targets)[n_targets])
  {
    std::vector<double> targets_v( targets, targets+n_targets);
    set_data( targets_v);
  };

 private: // not implemented
  CML_ProximityCounter(const CML_ProximityCounter&);
  CML_ProximityCounter& operator=(const CML_ProximityCounter&);
};
#endif
