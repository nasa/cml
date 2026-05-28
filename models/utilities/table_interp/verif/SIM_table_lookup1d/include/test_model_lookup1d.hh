/******************************************************************************
PURPOSE:
    (Test one dimensional table interpolation)

PROGRAMMERS:
    (((Bingquan Wang) (OSR) (May 2017) (The initial implementation)))

*******************************************************************************/

#ifndef TEST_MODEL_LOOKUP1D_HH
#define TEST_MODEL_LOOKUP1D_HH

#include <cmath>
#include "../../include/test_model_interp.hh"
#include "../../../include/osiris_math.h"


template<int N>
class TestModelLookup1D: public TestModelInterp<N>
{
public:
  typedef TestModelInterp<N> Base;
  typedef typename Base::DblTableType DblTableType;
  typedef typename Base::BptTableList BptTableList;
  typedef typename Base::ValTableType ValTableType;

  TestModelLookup1D(const double base_in=-2.0,  const double step_in=0.5) : 
    base(base_in), step(step_in)
  {
    static_assert(0<N, "Dimension N should be great than zero.");
  }
 
  double update_impl(const DblTableType &var_list) const
  {
    static const double (&bpt_tbl)[N] = std::get<0>(Base::bpt_table_list);
    return table_lookup_1d(var_list[0], N, bpt_tbl, &(Base::val_table[0]));
  }
   
  void set_data(const bool asc, BptTableList &bpt_tbl_list, ValTableType &val_tbl) const 
  {
    static double (&bpt_tbl)[N] = std::get<0>(bpt_tbl_list);
    bpt_tbl[0] = base;
    val_tbl[0] = base*(1.0+std::cos(bpt_tbl[0]));
  
    const double step_impl = asc ? step : -step;
    for (int i=1; i<N; ++i) {
      bpt_tbl[i] = bpt_tbl[i-1]+step_impl;
      val_tbl[i] = bpt_tbl[i]*(1.0 + std::cos(bpt_tbl[i]));
    }        
  }

private:
  TestModelLookup1D(const TestModelLookup1D&);
  TestModelLookup1D& operator=(const TestModelLookup1D&); 

  const double base;
  const double step;
};

#endif /*TEST_MODEL_LOOKUP1D_HH */

