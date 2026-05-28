/******************************************************************************
PURPOSE:
    (Test two dimensional table interpolation)

PROGRAMMERS:
    (((Bingquan Wang) (OSR) (May 2017) (The initial implementation)))

*******************************************************************************/

#ifndef TEST_MODEL_LOOKUP2D_HH
#define TEST_MODEL_LOOKUP2D_HH

#include <cmath>
#include "../../include/test_model_interp.hh"
#include "../../../include/osiris_math.h"

static inline double cal_val(const double x, const double y)
{
  return x*(1.0+std::cos(x)) + 1.5*y*(2.0+std::sin(y));
}

template<int N, int M>
class TestModelLookup2D: public TestModelInterp<N, M>
{
public:
  typedef TestModelInterp<N, M> Base;
  typedef typename Base::DblTableType DblTableType;
  typedef typename Base::BptTableList BptTableList;
  typedef typename Base::ValTableType ValTableType;

  TestModelLookup2D(const double base_in=-2.0,  const double step_in=0.5) : 
    base(base_in), step(step_in)
  {
    static_assert(0<N&&0<M, "Dimension N and  M should be great than zero.");
  }
 
  double update_impl(const DblTableType &var_list) const
  {
    static const double (&bpt_tbl_x)[N] = std::get<0>(Base::bpt_table_list);
    static const double (&bpt_tbl_y)[M] = std::get<1>(Base::bpt_table_list);

    return table_lookup_2d(var_list[1], M, bpt_tbl_y, var_list[0], N, bpt_tbl_x,
                           &(Base::val_table[0][0]));
  }
   
  void set_data(const bool asc, BptTableList &bpt_tbl_list, ValTableType &val_tbl) const 
  {
    static double (&bpt_tbl_x)[N] = std::get<0>(bpt_tbl_list);
    static double (&bpt_tbl_y)[M] = std::get<1>(bpt_tbl_list);

    const double step_impl = asc ? step : -step;

    bpt_tbl_x[0] = base;
    bpt_tbl_y[0] = base;
    for (int i=1; i<N; ++i) {
      bpt_tbl_x[i] = bpt_tbl_x[i-1]+step_impl;
    }        
    for (int j=1; j<M; ++j) {
      bpt_tbl_y[j] = bpt_tbl_y[j-1]+2.0*step_impl;
    }
  
    for (int i=0; i<N; ++i)
      for (int j=0; j<M; ++j)
        val_tbl[i][j] = cal_val(bpt_tbl_x[i], bpt_tbl_y[j]);
  }

private:
  TestModelLookup2D(const TestModelLookup2D&);
  TestModelLookup2D& operator=(const TestModelLookup2D&); 

  const double base;
  const double step;
};

#endif /*TEST_MODEL_LOOKUP2D_HH */

