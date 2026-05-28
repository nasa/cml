/******************************************************************************
PURPOSE:
    (Test interpolation functions on two dimensional table)

PROGRAMMERS:
    (((Bingquan Wang) (OSR) (May 2017) (The initial implementation)))

*******************************************************************************/

#ifndef TEST_MODEL_INTERP2D_HH
#define TEST_MODEL_INTERP2D_HH

#include <cmath>
#include "../../include/test_model_interp.hh"
#include "../../../include/table_interp.h"

static inline double cal_val(const double x, const double y)
{
  return x*(1.0+std::cos(x)) + 1.5*y*(2.0+std::sin(y));
}

template<int N, int M>
class TestModelInterp2D: public TestModelInterp<N, M>
{
public:
  typedef TestModelInterp<N, M> Base;
  typedef typename Base::DblTableType DblTableType;
  typedef typename Base::BptTableList BptTableList;
  typedef typename Base::ValTableType ValTableType;

  TestModelInterp2D(const double base_in=-2.0,  const double step_in=0.5) : 
    base(base_in), step(step_in), low_index_x(0), frac_x(0.0),
    low_index_y(0), frac_y(0.0)
  {
    static_assert(0<N && 0<M, "Parameter N and  M should be great than zero.");
  }
 
  double update_impl(const DblTableType &var_list) const
  {
    static const double (&bpt_tbl_x)[N] = std::get<0>(Base::bpt_table_list);
    static const double (&bpt_tbl_y)[M] = std::get<1>(Base::bpt_table_list);

    scale(var_list[0], &frac_x, &low_index_x, bpt_tbl_x, N);  
    scale(var_list[1], &frac_y, &low_index_y, bpt_tbl_y, M);
   
    return table2(&(Base::val_table[0][0]), frac_x, low_index_x,
                  frac_y, low_index_y, M);
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

  int get_low_index_x() const { return low_index_x; }
  int get_low_index_y() const { return low_index_y; }
  double get_frac_x() const { return frac_x; }
  double get_frac_y() const { return frac_y; }

private:
  TestModelInterp2D(const TestModelInterp2D&);
  TestModelInterp2D& operator=(const TestModelInterp2D&); 

  const double base;
  const double step;
  mutable int low_index_x;
  mutable double frac_x;
  mutable int low_index_y;
  mutable double frac_y;
};

#endif /*TEST_MODEL_INTERP2D_HH */

