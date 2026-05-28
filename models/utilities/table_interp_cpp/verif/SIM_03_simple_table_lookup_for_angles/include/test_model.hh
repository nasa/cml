/*******************************************************************************
PURPOSE:
  (Table lookup test)

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2014) (New implementation for C++ models))
   ((Bingquan Wang) (OSR) (August 2017) (IV&V cleanup))
  )
*******************************************************************************/

#ifndef ANTARES_TABLE_LOOKUP_TEST_MODEL_HH
#define ANTARES_TABLE_LOOKUP_TEST_MODEL_HH

#include <algorithm> //std::swap()

#include "../../../include/simple_angle_table_lookup.hh"

class TestModel {
public: 
  SimpleAngleTableLookup lookup;
  double var1;
  double var2;
  double var3;
  double driver_incr;
  double &driver;
  double is_asc;

  TestModel(double &driver_in,
            TableIndependentVariable::LookupMethod lookup_method_in =
                               TableIndependentVariable::INTERP)
    :
    lookup(driver_in, lookup_method_in),
    driver_incr(0.25),
    driver(driver_in),
    is_asc(true)
    {};

  ~TestModel(){};

  void update()
  {
    driver += driver_incr;
    lookup.lookup();
    var1 = lookup.output_values[0];
    var2 = lookup.output_values[1];
    var3 = lookup.output_values[2];
  };
  
  void default_data()
  {
    double scratch[5] = {1,2,3,4,5};
    if (!is_asc) {
      for (size_t i=0; i<2; ++i)
        std::swap(scratch[i], scratch[4-i]);
      driver_incr = - driver_incr; 
    }
    lookup.independent.load_data( scratch, 5);
    double scratch_out[3][5] = {{0.0, 1.5, 3.0, -3.0, -1.5},
                                {1.5, 3.0, 4.5,  6.0,  1.5},
                                {0.0, 3.5, 7.0, -2.0,  0.0}};
    lookup.load_data(&scratch_out[0][0], 3,5);
  };

private:
  // to disble the copy/assignment operators
  TestModel (const TestModel&);
  TestModel& operator = (const TestModel&); 
};
#endif

