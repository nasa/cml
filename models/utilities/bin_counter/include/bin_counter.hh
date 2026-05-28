/******************************** TRICK HEADER *********************************
PURPOSE:
  (Define structure and function prototypes for counting the number of
  times some variable of type double has a value that falls into specified bins)

ASSUMPTIONS:
  (Bin edges are monotonically increasing and bins are numerically adjacent.)

LIBRARY DEPENDENCY:
  (../src/bin_counter.cc)

PROGRAMMERS:
  (
    ((Matthew Elmer, Gary Turner) (OSR) (Jun 2023)
      (Refactored content from GNC_PAR; Overhauled grok_bin_counter))
  )
*******************************************************************************/
#ifndef CML_BIN_COUNTER_HH
#define CML_BIN_COUNTER_HH

#include <vector>
#include "cml/models/utilities/cml_message/include/cml_message.hh"


/*****************************************************************************
CML_BinCounterElement
Purpose: Contents of each element of the working vector
*****************************************************************************/
struct CML_BinCounterElement {
  double bin_floor; /* (--)
    Bin lower bound.*/
  double bin_ceil; /* (--)
    Bin upper bound. */
  unsigned int count; /* (--)
    Counter. Incremented when falling in this specific bin. */
};

/*****************************************************************************
CML_BinCounter
Purpose:
  Receives a value via the insert() command and bins it into one of
  a set of bins, each covering a finite domain.
*****************************************************************************/
class CML_BinCounter {
 protected:
  std::vector<CML_BinCounterElement> bins;  /* (--)
    The edges that separate each bin. */
  bool bins_ready; /* (--)
    Check on internal consistency of the edges in the bins array.*/
  size_t nbin;  /* (--)
    Number of bins. For logging compatibility only. */
  const CML_BinCounterElement* bin_data;  /* (--)
    Array of bins data. For logging compatibility only.*/

 public:
  std::string name; /* (--)
    Name of bin group (for debugging purposes)*/
  CML_BinCounter();
  explicit CML_BinCounter(const std::vector<double> & edges,
                          bool closed_ends = true);
  CML_BinCounter( double limit_a,
                  double limit_b,
                  unsigned int num_bins,
                  bool closed_ends = true);
  virtual ~CML_BinCounter(){};

  void insert(double);
  void set_data( const std::vector<double> & edges,
                 bool closed_ends = true);
  void set_data( double limit_a,
                 double limit_b,
                 unsigned int num_bins,
                 bool closed_ends = true);
  template <size_t n_edges>
  void set_data( const double (&edges)[n_edges],
                 bool closed_ends = true)
  {
    std::vector<double> edges_v( edges, edges+n_edges);
    set_data( edges_v, closed_ends);
  };

 private: // not implemented
  CML_BinCounter(const CML_BinCounter&);
  CML_BinCounter& operator=(const CML_BinCounter&);
};
#endif
