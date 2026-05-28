/******************************** TRICK HEADER *********************************
PURPOSE:
  (Define structure and function prototypes for counting the number of
  times some integer variable has specific values.)

LIBRARY DEPENDENCY:
  (../src/target_counter.cc)

PROGRAMMERS:
  (
    ((Matthew Elmer, Gary Turner) (OSR) (Jun 2023)
      (Refactored content from GNC_PAR; Overhauled grok_bin_counter))
  )
*******************************************************************************/
#ifndef CML_TARGET_COUNTER_HH
#define CML_TARGET_COUNTER_HH

#include <vector>
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
CML_TargetCounterElement
Purpose: Contents of each element of the working vector
*****************************************************************************/
struct CML_TargetCounterElement {
  int value; /* (--)
    Target integer value to be counted */
  unsigned int count; /* (--)
    Counter. Incremented when variable = value. */
};


/*****************************************************************************
CML_TargetCounter
Purpose:
  Receives a value via the insert() command and bins it into one of
  a set of bins, each covering a finite domain.
*****************************************************************************/
class CML_TargetCounter {
 protected:
  std::vector<CML_TargetCounterElement> targets;  /* (--)
    The targets to be counting occurrences of. */
  size_t ntarget;  /* (--)
    Number of targets.*/
  const CML_TargetCounterElement* target_data;  /* (--)
    Array of targets data. For logging compatibility only.*/

 public:
  std::string name; /* (--)
    Name of group (for debugging purposes)*/
  CML_TargetCounter();
  explicit CML_TargetCounter( const std::vector<int> & targets_);
  CML_TargetCounter( int limit_a,
                     int limit_b);

  void insert(int);
  bool does_val_exist(int);

  void set_data( const std::vector<int> & targets);
  void set_data( int limit_a,
                 int limit_b);
  template <size_t n_targets>
  void set_data( const int (&targets_)[n_targets])
  //                             int num_targets)
  {
    std::vector<int> targets_v( targets_, targets_+n_targets);
    set_data( targets_v);
  };

 private: // not implemented
  CML_TargetCounter(const CML_TargetCounter&);
  CML_TargetCounter& operator=(const CML_TargetCounter&);
};
#endif
