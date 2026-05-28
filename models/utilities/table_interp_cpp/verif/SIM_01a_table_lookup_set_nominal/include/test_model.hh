/*******************************************************************************
PURPOSE:
  (Table lookup test, provides data to 3 dependent variables as a function of
   4 independent variables via 2 tables.

   multi-table provides data to dependent_vars[0-1] as function of
     independent variables [0-2]
   single_table provides data to only dependent_vars[2]) as a function of
     independent variables [0,1,3]
   independent variable 3 is a wrap-around independent, whereas the otheri
     three independents are all (default) linear independents.

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2014) (New implementation for C++ models))
   ((Bingquan Wang) (OSR) (August 2017) (IV&V cleanup))
  )
*******************************************************************************/

#ifndef ANTARES_TABLE_LOOKUP_TEST_MODEL_HH
#define ANTARES_TABLE_LOOKUP_TEST_MODEL_HH

#include <algorithm> //std::swap()
#include <cstddef> // memcpy
#include <cstring> // NULL
#include <vector>  // vector

#include "../../../include/table_lookup_set.hh"
#include "../../../include/table_independent_variable.hh"

class TestModelBase
{
public:
  TableLookupSet table_set;
  double dependent_vars[3]; /* (--)
     The dependent variables would typically reside in the respective model,
     while the independent variables might be completely external.
     For this testing, the independents are instantiated in the S_define, and
     the dependents are instantiated here. */
  TableIndependentVariable::LookupMethod lookup_method[4];

  // Data elements.  Put here to ensure commonality between the two
  // implementations.
  // Note that I have chosen to use a STL-vector structure to hold the data for
  // the independent variables, and an array structure to hold the data for the
  // dependent variables.  Both options are actually available for both data
  // sets, they are both illustrated here for increased exposure.
  std::vector< std::vector<double> > independents_data;
  double single_table_vals[3][4][3];
  double multi_table_vals[2][3][4][3];
  std::vector<size_t> single_table_dims;
  std::vector<size_t> multi_table_dims;


  TestModelBase()
    :
    table_set(),
    dependent_vars(),
    lookup_method(),
    single_table_dims(4),
    multi_table_dims(4)
  {
    table_set.subscribe();

    multi_table_dims[0]=2;
    multi_table_dims[1]=3;
    multi_table_dims[2]=4;
    multi_table_dims[3]=3;
    double temp_table[2][3][4][3] =
       {{{{1000,1001,1002},{1010,1011,1012},{1020,1021,1022},{1030,1031,1032}},
         {{1100,1101,1102},{1110,1111,1112},{1120,1121,1122},{1130,1131,1132}},
         {{1200,1201,1202},{1210,1211,1212},{1220,1221,1222},{1230,1231,1232}}},

        {{{2000,2001,2002},{2010,2011,2012},{2020,2021,2022},{2030,2031,2032}},
         {{2100,2101,2102},{2110,2111,2112},{2120,2121,2122},{2130,2131,2132}},
         {{2200,2201,2202},{2210,2211,2212},{2220,2221,2222},{2230,2231,2232}}}};
    memcpy(multi_table_vals, temp_table, sizeof(temp_table));


    single_table_dims[0]=1;
    single_table_dims[1]=3;
    single_table_dims[2]=4;
    single_table_dims[3]=3;
    double temp_table1[3][4][3] =
        {{{3000,3001,3002},{3010,3011,3012},{3020,3021,3022},{3030,3031,3032}},
         {{3100,3101,3102},{3110,3111,3112},{3120,3121,3122},{3130,3131,3132}},
         {{3200,3201,3202},{3210,3211,3212},{3220,3221,3222},{3230,3231,3232}}};
    memcpy(single_table_vals, temp_table1, sizeof(temp_table1));

    // Note - independents can be populated the same way as the table-vals --
    // using memcpy.
    // Illustrating the vector way instead for increased exposure.
    double temp0[3] = {9,8,-2};
    std::vector<double> indep_0_data (temp0, temp0+3);
    double temp1[4] = {2,4,6,8};
    std::vector<double> indep_1_data (temp1, temp1+4);
    double temp2[3] = {2,4,6};
    std::vector<double> indep_2_data (temp2, temp2+3);
    std::vector<double> indep_3_data = indep_2_data;

    independents_data.push_back(indep_0_data);
    independents_data.push_back(indep_1_data);
    independents_data.push_back(indep_2_data);
    independents_data.push_back(indep_3_data);


    for (size_t ii = 0; ii < 4; ++ii) {
      lookup_method[ii] = TableIndependentVariable::Interp;
    }
  }

  virtual ~TestModelBase() { };

  void initialize( ) { table_set.initialize(); }


private:
  // Disable the copy/assignment operations
  TestModelBase (const TestModelBase&);
  TestModelBase& operator = (const TestModelBase&);

};


/*****************************************************************************
TestModelDynamicAllocate
Purpose:( Implements the data tables as described in the main purpose at the
   top of this file.
   This class implements the components using dynamic memory allocation
   through the create_table andi create_independent_variable methods.
   Using these methods, the memory for these lookup tables will only be
   allocated as needed.)
*****************************************************************************/
class TestModelDynamicAllocate : public TestModelBase
{
public:
  TestModelDynamicAllocate()
    :
    TestModelBase()
  {};

  void configure( double independent_vars[4])
  {
    /* Multi-variable table takes 3 independent variables.
       Single-variable table also takes 3 independent variables, but 2 are
       shared with the multi-variable table.
       So we need to create 4 independent variables and 2 tables, and make
       the associations between the tables and the variables.
       This could be organized such that all of the allocations are performed,
       then all of the associations, or the create and associations could be
       interwoven so that elements are only created as needed.
       The associate_table_and_independent method has 12 unique signatures;
       several of these default to using the last item created on the table
       and independent-variable stacks, so to test as many of these as
       possible, we will be following the latter pattern -- weaving
       together the creation and association so we can utilize these
       last-item-on-the-stack default identifiers.

       NOTE -- It would be cleaner and easier to follow to use the former
       method, i.e.  create everything in one block and then associate the
       elements by identifying characteristic in another block.
       This design pattern is used in TestModelPreInstantiated, and is the
       recommended pattern even for dynamic allocation.
       The only reason this design is so interwoven is to explore as much
       of the code as is possible in this unit-test.
       IT IS NOT AN EXAMPLE TO FOLLOW IN A REAL IMPLEMENTATION!!!*/


    // First create and populate the first table
    GenericMultiInputTable * new_table =
                table_set.create_table(2,
                                       &dependent_vars[0],
                                       &dependent_vars[1]);
    new_table->load_data(&multi_table_vals[0][0][0][0], multi_table_dims);


    // now create and populate the first of the independent variables:
    TableIndependentVariable * new_var =
      table_set.create_independent_variable( "var_0",
                                             independent_vars[0],
                                             TableIndependentVariable::Linear);
    new_var->load_data( independents_data[0]);

    /* Associate these two components just created.
       For this association we will be testing the signature that has only
       a single argument -- the lookup method.
       Notes:
       - the zero-argument method uses this same signature; the lookup method
         is given a default setting of "Interp" (ie. interpolation)
       - using this default argument is not explicitly tested because we want
         the two table-sets to match so must provide a method here to match
         that provided to the TestModelPreInstantiated implementation
       - lacking an argument specifying which table to use, the method will
         default to using the last table on the stack.
       - lacking an argument specifying which variable to use, the method will
         default to using the last variable (i.e. var_0) on the stack.
       - lacking an argument specifying the position to which that variable
         should be assigned, the method will default to adding it onto the
         back of the table's current list of independent variables.
         Because that list is currently empty.  var_0 will be pushed into
         index 0*/
    table_set.associate_table_and_independent(lookup_method[0]);

    // create and populate another independent variable
    new_var = table_set.create_independent_variable( "var_1",
                                                     independent_vars[1]);
    new_var->load_data( independents_data[1]);

    /* Associate this variable with the existing table
       For this association we will be testing the signature that provides
       specifications for position and lookup-type.
       Notes:
       - lacking an argument specifying which table to use, the method will
         again default to using the last table on the stack.
       - lacking an argument specifying which variable to use, the method will
         default to using the last variable (i.e. var_1) on the stack.
       - the position to which that variable should be assigned is specified
         as 1.  In this case, this specification is redundant because if it
         had been omitted, the variable would have been added to the end of
         the table's list, which would still have been at index 1.
       Additional note on this method, especially if running through SWIG
       - the use of a "0" for the index generates an ambiguity
         because 0 can also mean NULL.
         To specify placement at position "0", it may be necessary to
         specify type as well "(size_t)0" */
    table_set.associate_table_and_independent(1,
                                              lookup_method[1]);

    // create and populate another independent variable
    new_var = table_set.create_independent_variable( "var_2",
                                                    independent_vars[2]);
    new_var->load_data( independents_data[2]);


    /* Associate this variable with the existing table
       For this association we will be testing the signature that provides
       specifications for the variable and lookup-type.
       Notes:
       - lacking an argument specifying which table to use, the method will
         again default to using the last table on the stack.
       - the variable to be associated is identified in this version.
         In this case, its specification is redundant because if it had been
         omitted, the method would default to using the last variable on the
         stack, which is also "var_2".
       - lacking an argument specifying the position to which that variable
         should be assigned, the method will default to adding it onto the
         back of the table's current list of independent variables.
         Because that list already has 2 elements ("var_0" and "var_1"),
         "var_2" will be pushed into index 2. */
    table_set.associate_table_and_independent(new_var,
                                              lookup_method[2]);


    // Now create and populate the other table
    new_table = table_set.create_table( dependent_vars[2]);
    new_table->load_data(&single_table_vals[0][0][0], single_table_dims);

    /* Associate this table and its first variable, "var_0"
       For this association we will be testing the signature that provides
       specifications for the table and lookup-type.
       Notes:
       - lacking an argument specifying which table to use, the method will
         default to using the last table on the stack, which is now the
         second table.
       - the variable to be associated must be identified in this version.
         "var_0" is no longer on the top of the stack ("var_1" is above it,
         and "var_2" above "var_1") so to get to "var_0", we must identify it.
         In this case, we identify it by name.
       - lacking an argument specifying the position to which that variable
         should be assigned, the method will default to adding it onto the
         back of the table's current list of independent variables.
         Because that list is currently empty.  var_0 will be pushed into
         index 0*/
    table_set.associate_table_and_independent( "var_0",
                                               lookup_method[0]);

    /* NOTE - would usually add var_1 next, but I'm holding off to test the
              ability to add items out of order using placement specifiers.
              So create and associate the last of the independents:*/

    // create and populate the last independent variable
    new_var = table_set.create_independent_variable(
                                         "var_3",
                                         independent_vars[3],
                                         TableIndependentVariable::WrapAround);
    new_var->load_data( independents_data[3]);

    /* Associate this variable and the current table.
       This association is deliberately (and unusually) made out of order
       to test the ability to associate variables out of order.
       This is not recommended.
       For this association we will be testing the signature that provides
       specifications for the variable (by address), its position, and its
       lookup-type.
       Notes:
       - lacking an argument specifying which table to use, the method will
         default to using the last table on the stack, which is still the
         second table.
       - the variable to be associated is identified by address.
         In this case, its specification is redundant because if it had been
         omitted, the method would default to using the last variable on the
         stack, which is also "var_3".
       - the position for this variable is specified.  Notice that if this
         was not specified, it would have defaulted to the back of the list
         (index-value 1, index-value 0 is filled with "var_0").
         In this case, we deliberately place it at index 2, which means
         index 1 will be filled witha  NULL entry until it can be connected
         to a variable at a later time.*/
    table_set.associate_table_and_independent(new_var,
                                              2,
                                              lookup_method[3]);

    /* Now associate "var_1" (out of order) and the current table.
       For this association we will be testing the signature that provides
       specifications for the variable (by name), its position, and its
       lookup-type.
       Notes:
       - lacking an argument specifying which table to use, the method will
         default to using the last table on the stack, which is still the
         second table.
       - the variable to be associated is identified by name.
       - the position for this variable is specified.  Notice that if this
         was not specified, it would have defaulted to the back of the list
         (index-value 3, the first 3 entries are ["var_0", NULL, "var_3"])
         In this case, we deliberately place it at index 1, overwriting the
         NULL entry.*/
    table_set.associate_table_and_independent( "var_1",
                                               1,
                                               lookup_method[1]);
  }
  virtual ~TestModelDynamicAllocate() { };
  void update() { table_set.update(); };
private:
  // Disable the copy/assignment operations
  TestModelDynamicAllocate (const TestModelDynamicAllocate&);
  TestModelDynamicAllocate& operator = (const TestModelDynamicAllocate&);
};

/*****************************************************************************
Purpose:
   (A version of the test model in which the values are all
    pre-instantiated with the construction.  It is not necessary to
    create them using the create_* methods.
    The 4 independent variables are constructed using different constructor
    signatures.  This is for testing purposes and is not necessary, they may
    all use the same pattern. Just pick the most convenient.
    This class continues to test the associate_* methods, using the 6 not used
    in TestModelDynamicAllocate.
    This distinction is also for testing purposes; any associate_* method may
    be used and repeated as convenient.)
*****************************************************************************/
class TestModelPreInstantiated :public TestModelBase
{
public:
  TableIndependentVariable indep0;
  TableIndependentVariable indep1;
  TableIndependentVariable indep2;
  TableIndependentVariable indep3;
  GenericMultiInputTable   multi_table;
  GenericMultiInputTable   single_table;
  // Keep a record of the interpolation-indices for testing purposes:
  size_t indep0_ix;
  size_t indep1_ix;
  size_t indep2_ix;
  size_t indep3_ix;
  bool   indep0_off_table;


  // Constructor
  TestModelPreInstantiated( double independent_vars[4] )
    :
    TestModelBase(),
    indep0(independent_vars[0], TableIndependentVariable::Linear),
    indep1(independent_vars[1], TableIndependentVariable::Linear),
    indep2("indy2", independent_vars[2], TableIndependentVariable::Linear),
    indep3("indy3", independent_vars[3], TableIndependentVariable::WrapAround),
    multi_table(),
    single_table(dependent_vars[2])
  {}

  void configure()
  {
    // set outputs for multi-table
    std::vector<double*> dependent_list(2);
    dependent_list[0] = &dependent_vars[0];
    dependent_list[1] = &dependent_vars[1];
    multi_table.set_output(dependent_list);

    // Load data
    indep0.load_data( independents_data[0]);
    indep1.load_data( independents_data[1]);
    indep2.load_data( independents_data[2]);
    indep3.load_data( independents_data[3]);
    single_table.load_data(&single_table_vals[0][0][0], single_table_dims);
    multi_table.load_data(&multi_table_vals[0][0][0][0], multi_table_dims);

    // Add tables to set:
    table_set.add_table( single_table);
    table_set.add_table( multi_table);

    // Add variables to set:
    table_set.add_independent_variable(indep0);
    table_set.add_independent_variable(indep1);

    // Test the other 6 methods of associating tables and independents
    // while the independents are being added.
    // tab-ptr, var-ptr
    table_set.associate_table_and_independent( &single_table,
                                               &indep0,
                                               lookup_method[0]);
    // table-ptr only, takes the last indep (indep1)
    table_set.associate_table_and_independent( &single_table,
                                               lookup_method[1]);

    // Table-ptr and table-index takes the last independent (indep1)
    // Put it at index 1 - this should create NULL values at index 0
    // because it has not been populated.
    table_set.associate_table_and_independent( &multi_table,
                                               1,
                                               lookup_method[1]);
    // Table-ptr var-ptr and table-index
    table_set.associate_table_and_independent( &multi_table,
                                               &indep0,
                                               0,
                                               lookup_method[0]);

    table_set.add_independent_variable(indep2);
    table_set.add_independent_variable(indep3);


    // table-ptr, var-name
    table_set.associate_table_and_independent( &single_table,
                                               "indy3",
                                               lookup_method[3]);
    // table-ptr, var-name, table-index
    table_set.associate_table_and_independent( &multi_table,
                                               "indy2",
                                               2,
                                               lookup_method[2]);
  };

  virtual ~TestModelPreInstantiated() {};
  void update()
  {
    table_set.update();
    indep0_ix = indep0.get_index();
    indep1_ix = indep1.get_index();
    indep2_ix = indep2.get_index();
    indep3_ix = indep3.get_index();
    indep0_off_table = indep0.is_off_table();
  }

  void print_tiv_indep0()
  {
    std::cout << "TIV data for indep0:  [";
    for (size_t ii = 0; ii < indep0.data.size(); ++ii) {
      std::cout << indep0.data[ii] << "  ";
    }
    std::cout << "]\n";
  }

  void print_tiv_indep1()
  {
    std::cout << "TIV data for indep1:  [";
    for (size_t ii = 0; ii < indep1.data.size(); ++ii) {
      std::cout << indep1.data[ii] << "  ";
    }
    std::cout << "]\n";
  }

private:
  // Disable the copy/assignment operations
  TestModelPreInstantiated (const TestModelPreInstantiated&);
  TestModelPreInstantiated& operator = (const TestModelPreInstantiated&);
};
#endif
