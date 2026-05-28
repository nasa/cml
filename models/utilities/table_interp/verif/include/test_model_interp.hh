/******************************************************************************
PURPOSE:
    (Base test model for interpolations on different dimensional tables)

ICG:
    (No)

SWIG:
    (No)

PROGRAMMERS:
    (((Bingquan Wang) (OSR) (May 2017) (The initial implementation))
     ((Bingquan Wang) (OSR) (Jan 2018) (Migrated to FSL env.))
    )

*******************************************************************************/
#ifndef TEST_MODEL_INTERP_HH
#define TEST_MODEL_INTERP_HH

#ifndef SWIG
#include <tuple>
#endif /* SWIG */

#include "generic_interp_array.hh"

template<int Dim0, int... DimList>
class TestModelInterp
{
public:
  TestModelInterp() { }
  virtual ~TestModelInterp() { }
 
  template<typename... VarList>
  double update(const double var0, const VarList... var_list) const
  {
    static_assert(sizeof...(var_list)==sizeof...(DimList), 
      "The number of input variables should match the number of dimensions.");

    input_list[0] = var0;
    return update<1>(var_list...);  
  }

  void set_default_data(const bool asc)
  {
    set_data(asc, bpt_table_list, val_table);
  }
  
protected:
  typedef GenericInterpArray<Dim0, DimList...> ValTableType;
  typedef std::tuple<double[Dim0], double[DimList]...> BptTableList;
  typedef double DblTableType[sizeof...(DimList)+1];

  BptTableList bpt_table_list;
  ValTableType val_table;

  virtual void set_data(const bool asc, BptTableList &bpt_tbl_list, ValTableType &val_tbl) const = 0;
  virtual double update_impl(const DblTableType &var_list) const = 0;

private:
  TestModelInterp(const TestModelInterp&);
  TestModelInterp& operator=(const TestModelInterp&);

  mutable DblTableType input_list;

  template<int N, typename... VarList>
  double update(const double var0, const VarList... var_list) const
  {
    input_list[N] = var0;
    return update<N+1>(var_list...);
  }
  
  template<int N=sizeof...(DimList)+1>
  double update() const
  {
     return update_impl(input_list); 
  }
};

#endif /* TEST_MODEL_INTERP_HH */

