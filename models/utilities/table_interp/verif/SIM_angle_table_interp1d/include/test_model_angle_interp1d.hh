/******************************************************************************
PURPOSE:
    (Test angle interpolation functions on one dimensional table)

PROGRAMMERS:
    (((Bingquan Wang) (OSR) (May 2017) (The initial implementation)))

*******************************************************************************/

#ifndef TEST_MODEL_ANGLE_INTERP1D_HH
#define TEST_MODEL_ANGLE_INTERP1D_HH

#include <cmath>
#include "../../include/test_model_interp.hh"
#include "../../../include/table_interp.h"

static inline double cal_angle(const double x)
{
  static const double PI_2 = 2*M_PI;

  double angle = x*(1.0+std::cos(x));
  while (0 > angle) angle+=PI_2;
  while (PI_2 < angle) angle-=PI_2;

  return angle; 
}


template<int N=25>
class TestModelAngleInterp1D: public TestModelInterp<N>
{
public:
  typedef TestModelInterp<N> Base;
  typedef typename Base::DblTableType DblTableType;
  typedef typename Base::BptTableList BptTableList;
  typedef typename Base::ValTableType ValTableType;

  TestModelAngleInterp1D(const double base_in=-3.0, const double step_in=0.5) : 
    base(base_in), step(step_in), low_index(0), frac(0.0)
  {
    static_assert(0<N, "Dimension N should be great than zero.");
  }
 
  double update_impl(const DblTableType &var_list) const
  {
    static const double (&bpt_tbl)[N] = std::get<0>(Base::bpt_table_list);
    scale(var_list[0], &frac, &low_index, bpt_tbl, N);  
   
    return table1_angle(&(Base::val_table[0]), frac, low_index);
  }
   
  void set_data(const bool asc, BptTableList &bpt_tbl_list, ValTableType &val_tbl) const 
  {
    static double (&bpt_tbl)[N] = std::get<0>(bpt_tbl_list);
    const double step_impl = asc ? step : -step;
    bpt_tbl[0] = base;
    val_tbl[0] = cal_angle(base); 
    for (int i=1; i<N; ++i) {
      bpt_tbl[i] = bpt_tbl[i-1]+step_impl;
      val_tbl[i] = cal_angle(bpt_tbl[i]);
    }        
  }

  int get_low_index() const { return low_index; }
  double get_frac() const { return frac; }

private:
  TestModelAngleInterp1D(const TestModelAngleInterp1D&);
  TestModelAngleInterp1D& operator=(const TestModelAngleInterp1D&); 

  const double base;
  const double step;
  mutable int low_index;
  mutable double frac;
};

#endif /*TEST_MODEL_ANGLE_INTERP1D_HH */

