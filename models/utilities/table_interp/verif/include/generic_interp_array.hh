/******************************************************************************
PURPOSE:
    (Generic multiple dimensional array class for model table_interp test)

ICG:
    (No)

SWIG:
    (No)

PROGRAMMERS:
    (((Bingquan Wang) (OSR) (May 2017) (The initial implementation)))

*******************************************************************************/
#ifndef GENERIC_INTERP_ARRAY_HH
#define GENERIC_INTERP_ARRAY_HH

template <int... DimList> struct GenericInterpArray;

template<int Dim>
struct GenericInterpArray<Dim>
{
  typedef double TableType[Dim];
  TableType table;

  double& operator[](const int index) { return table[index]; }
  const double& operator[](const int index) const { return table[index]; } 
};

template<int Dim0, int... DimList>
struct GenericInterpArray<Dim0, DimList...>
{
  typedef GenericInterpArray<DimList...> SubArrayType;
  typedef typename SubArrayType::TableType SubTableType;
  typedef SubTableType TableType[Dim0];
  TableType table;

  SubTableType& operator[](const int index) { return table[index]; }
  const SubTableType& operator[](const int index) const { return table[index]; } 
};

#endif /* GENERIC_INTERP_ARRAY_HH */

