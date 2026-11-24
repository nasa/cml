/******************************** TRICK HEADER *********************************
PURPOSE:
  (Define structure and function prototypes for counting the number of
  times some integer variable has specific values.)

LIBRARY DEPENDENCY:
  (../src/target_counter.cc)

PYTHON_MODULE: (cml)

PROGRAMMERS:
  (
    ((Matthew Elmer, Gary Turner) (OSR) (Jun 2023)
      (Refactored content from GNC_PAR; Overhauled grok_bin_counter))
  )
*******************************************************************************/
#ifndef CML_TARGET_COUNTER_HH
#define CML_TARGET_COUNTER_HH

#include <cstddef>
#include <string>
#include <vector>

namespace cml {

/*****************************************************************************
TargetCounterElement
Purpose: Contents of each element of the working vector
*****************************************************************************/
struct TargetCounterElement {
  int value; /* (--)
    Target integer value to be counted */
  unsigned int count; /* (1)
    Counter. Incremented when variable = value. */
};


/*****************************************************************************
TargetCounter
Purpose:
  Receives a value via the insert() command and bins it into one of
  a set of bins, each covering a finite domain.
*****************************************************************************/
class TargetCounter {
 protected:
  std::vector<TargetCounterElement> targets;  /* (--)
    The targets to be counting occurrences of. */
  size_t ntarget;  /* (--)
    Number of targets.*/
  const TargetCounterElement* target_data;  /* (--)
    Array of targets data. For logging compatibility only.*/

 public:
  std::string name; /* (--)
    Name of group (for debugging purposes)*/
  TargetCounter();
  explicit TargetCounter( const std::vector<int> & targets_);
  TargetCounter( int limit_a,
                     int limit_b);
  TargetCounter(const TargetCounter&) = delete;
  TargetCounter& operator=(const TargetCounter&) = delete;

  void insert(int value);
  bool does_val_exist(int val) const;

  void set_data( const std::vector<int> & targets_);
  void set_data( int limit_a,
                 int limit_b);
  template <size_t n_targets>
  void set_data( const int (&targets_)[n_targets])
  //                             int num_targets)
  {
    std::vector<int> targets_v( targets_, targets_+n_targets);
    set_data( targets_v);
  }
  size_t get_ntarget() const {return ntarget;}
};

} // namespace cml

#endif
