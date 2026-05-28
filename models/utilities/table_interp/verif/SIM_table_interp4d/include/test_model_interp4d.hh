/******************************************************************************
PURPOSE:
    (Test interpolation functions on four  dimensional table)

PROGRAMMERS:
    (((Bingquan Wang) (OSR) (May 2017) (The initial implementation)))

*******************************************************************************/

#ifndef TEST_MODEL_INTERP4D_HH
#define TEST_MODEL_INTERP4D_HH

#include <cmath>
#include "../../include/test_model_interp.hh"
#include "../../../include/table_interp.h"

static inline double cal_val(const double x, const double y, const double z, 
                             const double w)
{
  return x*(1.0+std::cos(x)) + 1.5*y*(2.0+std::sin(y)) + 
         0.5*z*(1.0+std::sin(z)) + w*(1.0+std::cos(w));
}

template<int L, int K, int N, int M>
class TestModelInterp4D: public TestModelInterp<L, K, N, M>
{
public:
  typedef TestModelInterp<L, K, N, M> Base;
  typedef typename Base::DblTableType DblTableType;
  typedef typename Base::BptTableList BptTableList;
  typedef typename Base::ValTableType ValTableType;

  TestModelInterp4D(const double base_in=-2.0,  const double step_in=0.5) : 
    base(base_in), step(step_in), low_index_x(0), frac_x(0.0),
    low_index_y(0), frac_y(0.0), low_index_z(0), frac_z(0.0),
    low_index_w(0), frac_w(0.0)
  {
    static_assert(0<N&&0<M&&0<K&&0<L , "Dimension N, M, K and L should be great than zero.");
  }
 
  double update_impl(const DblTableType &var_list) const
  {
    static const double (&bpt_tbl_x)[N] = std::get<2>(Base::bpt_table_list);
    static const double (&bpt_tbl_y)[M] = std::get<3>(Base::bpt_table_list);
    static const double (&bpt_tbl_z)[K] = std::get<1>(Base::bpt_table_list);
    static const double (&bpt_tbl_w)[L] = std::get<0>(Base::bpt_table_list);

    scale(var_list[0], &frac_x, &low_index_x, bpt_tbl_x, N);  
    scale(var_list[1], &frac_y, &low_index_y, bpt_tbl_y, M);
    scale(var_list[2], &frac_z, &low_index_z, bpt_tbl_z, K);
    scale(var_list[3], &frac_w, &low_index_w, bpt_tbl_w, L);
   
    return table4(&(Base::val_table[0][0][0][0]), frac_x, low_index_x, N,
                  frac_y, low_index_y, M, frac_z, low_index_z, K, frac_w,
                  low_index_w);
  }
   
  void set_data(const bool asc, BptTableList &bpt_tbl_list, ValTableType &val_tbl) const 
  {
    static double (&bpt_tbl_x)[N] = std::get<2>(bpt_tbl_list);
    static double (&bpt_tbl_y)[M] = std::get<3>(bpt_tbl_list);
    static double (&bpt_tbl_z)[K] = std::get<1>(bpt_tbl_list);
    static double (&bpt_tbl_w)[L] = std::get<0>(bpt_tbl_list);

    const double step_impl = asc ? step : -step;

    bpt_tbl_x[0] = base;
    bpt_tbl_y[0] = base;
    bpt_tbl_z[0] = base;
    bpt_tbl_w[0] = base;
    for (int i=1; i<N; ++i) {
      bpt_tbl_x[i] = bpt_tbl_x[i-1]+step_impl;
    }        
    for (int i=1; i<M; ++i) {
      bpt_tbl_y[i] = bpt_tbl_y[i-1]+2.0*step_impl;
    }
    for (int i=1; i<K; ++i) {
      bpt_tbl_z[i] = bpt_tbl_z[i-1]+2.0*step_impl;
    }
    for (int i=1; i<L; ++i) {
      bpt_tbl_w[i] = bpt_tbl_w[i-1]+2.0*step_impl;
    }

    for (int k=0; k<K; ++k)
      for (int i=0; i<N; ++i)
        for (int j=0; j<M; ++j)
          for (int l=0; l<L; ++l)
            val_tbl[l][k][i][j] = cal_val(bpt_tbl_x[i], bpt_tbl_y[j], bpt_tbl_z[k],
                                          bpt_tbl_w[l]);
  }

  int get_low_index_x() const { return low_index_x; }
  int get_low_index_y() const { return low_index_y; }
  int get_low_index_z() const { return low_index_z; }
  int get_low_index_w() const { return low_index_w; }
  double get_frac_x() const { return frac_x; }
  double get_frac_y() const { return frac_y; }
  double get_frac_z() const { return frac_z; }
  double get_frac_w() const { return frac_w; }

private:
  TestModelInterp4D(const TestModelInterp4D&);
  TestModelInterp4D& operator=(const TestModelInterp4D&); 

  const double base;
  const double step;
  mutable int low_index_x;
  mutable double frac_x;
  mutable int low_index_y;
  mutable double frac_y;
  mutable int low_index_z;
  mutable double frac_z;
  mutable int low_index_w;
  mutable double frac_w;
};

#endif /*TEST_MODEL_INTERP4D_HH */

