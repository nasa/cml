/******************************************************************************
PURPOSE:
    (Test interpolation functions on one dimensional table)

PROGRAMMERS:
    (((Bingquan Wang) (OSR) (May 2017) (The initial implementation)))

*******************************************************************************/

#ifndef TEST_MODEL_INTERP1D_HH
#define TEST_MODEL_INTERP1D_HH

#include <cmath>
#include "../../include/test_model_interp.hh"
#include "../../../include/table_interp.h"


template<int N>
class TestModelInterp1D: public TestModelInterp<N>
{
public:
  typedef TestModelInterp<N> Base;
  typedef typename Base::DblTableType DblTableType;
  typedef typename Base::BptTableList BptTableList;
  typedef typename Base::ValTableType ValTableType;

  TestModelInterp1D(const double base_in=-2.0,  const double step_in=0.5) : 
    base(base_in), step(step_in), low_index(0), frac(0.0)
  {
    static_assert(0<N, "Parameter N should be great than zero.");
  }
 
  double update_impl(const DblTableType &var_list) const
  {
    static const double (&bpt_tbl)[N] = std::get<0>(Base::bpt_table_list);
    scale(var_list[0], &frac, &low_index, bpt_tbl, N);  
   
    return table1(&(Base::val_table[0]), frac, low_index);
  }
   
  void set_data(const bool asc, BptTableList &bpt_tbl_list, ValTableType &val_tbl) const 
  {
    const double step_used = asc ? step : -step;
    static double (&bpt_tbl)[N] = std::get<0>(bpt_tbl_list);
    bpt_tbl[0] = base;
    val_tbl[0] = base*(1.0+std::cos(bpt_tbl[0]));
    for (int i=1; i<N; ++i) {
      bpt_tbl[i] = bpt_tbl[i-1]+step_used;
      val_tbl[i] = bpt_tbl[i]*(1.0 + std::cos(bpt_tbl[i]));
    }        
  }

  int get_low_index() const { return low_index; }
  double get_frac() const { return frac; }

private:
  TestModelInterp1D(const TestModelInterp1D&);
  TestModelInterp1D& operator=(const TestModelInterp1D&); 

  const double base;
  const double step;
  mutable int low_index;
  mutable double frac;
};

#endif /*TEST_MODEL_INTERP1D_HH */

