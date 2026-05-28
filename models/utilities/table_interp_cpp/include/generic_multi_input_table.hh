/*******************************TRICK HEADER***********************************
 PURPOSE:
    (definitions for the single-variable and multi-variable
      n-dimensional lookup tables and for the base class from which
      they derive.)

 LIBRARY DEPENDENCY:
    ((../src/generic_multi_input_table.cc)
    )

 PROGRAMMERS:
   (((Gary Turner) (OSR) (dec 2015) (Antares) (initial version))
    ((Bingquan Wang) (OSR) (Aug 2017) (Antares) (IVV code cleanup and refactored))
   )
*******************************************************************************/
#ifndef ANTARES_GENERIC_MULTI_INPUT_TABLE_HH
#define ANTARES_GENERIC_MULTI_INPUT_TABLE_HH

#include <list>
#include <string> // std::string
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "table_independent_variable.hh"
#include "table_type_defs.hh"


/*****************************************************************************
GenericMultiInputTable
Purpose:
  (Generic representation of a table of fixed data.  The table can:
   - utilize any (positive) number of inputs, including 1.
   - represent any number of dependent variables as long as they depend on
     the same independent variables in the same way.
*****************************************************************************/
class GenericMultiInputTable
{
 friend class SingleInputTableVarDeriv;
 protected:
  bool trivial_case; /* (--)
       Blocks the table-lookup when all independents have only one value.
       Default: true, usually modified to false internally. */
  bool output_ptrs_set;    /* (--)
       Flag to indicate the output pointers have been assigned and checked for
       NULL. */
  bool data_loaded; /* (--) internal flag indicating that data-load succeeded. */
  bool initialized; /* (--) set to true internal at successful initialization. */

  DoubleVec  data; /* (--)
       The externally defined dependent variable data. Because the number
       of dimensions is ambiguous, these data are stored in a 1-dimensional
       vector and the model contains the logic necessary to access the
       correct locations.*/
  SizeVec    size_of_dimension; /* (--)
       Specified number of data elements in each dimension. */
  DoubleVec  data_point_weight; /* (--)
       The weighting of each data point involved in the interpolation.
       Generated from the independent-variable fraction values.*/
  SizeVec    data_point_index; /* (--)
       The index of each data point involved in the interpolation. */
  SizeVec  num_data_elements_per_increment_of_index; /* (--)
       When the index ticks up on this dimension, this value specifies how
       many elements get skipped over in the data vector.*/

  DoublePtrVec output; /* (--)
       STL-vector of pointers to the variables to be populated by the lookup.*/
  IndepPairVec independents; /* (--)
       Collection of the independent variables used as inputs, coupled with the
       interpretation of how to use the values of the independent variables.*/
 public:
  GenericMultiInputTable();
  GenericMultiInputTable( double *dependent_variables,
                          size_t num_vars);
  explicit GenericMultiInputTable( double & dependent_var);
  explicit GenericMultiInputTable( const DoublePtrVec & dependent_variables);
  virtual ~GenericMultiInputTable() { }

  //the first element of dim_list is the number of dependent variables and
  //the rest are the size of each dimension of dependent variable
  bool load_data( const double *data_in,
                  const SizeVec &dim_list);
  bool load_data( const DoubleVec & data_in,
                  const SizeVec &dim_list);

  void add_dependent( double & dependent);
  void append_dependent_data( double & dependent,
                              const DoubleVec & data_in,
                              const SizeVec &dim_list);
  void add_independent( TableIndependentVariable & var,
                        TableIndependentVariable::LookupMethod lookup_method=
                                             TableIndependentVariable::Interp);
  void add_independent( TableIndependentVariable & var,
                        size_t index,
                        TableIndependentVariable::LookupMethod lookup_method=
                                             TableIndependentVariable::Interp);
  virtual bool initialize();
  virtual bool update();

  // getters and setters:
  void set_output(const DoublePtrVec &variables) { populate_output(variables); }
  void populate_output(const DoublePtrVec &var_ptr_list);
  bool is_initialized() const { return initialized; }
  bool is_data_loaded() const { return data_loaded; }
  const IndepPairVec & get_independents() const {return independents;}
  size_t get_num_dep_vars() const { return output.size();};
  size_t get_data_size() const { return data.size();}
  double* get_dependent_variable_ptr(size_t index) {return output.at(index);};

  void bias_data(double bias, size_t idx1, size_t idx2);
  void bias_data(double bias, size_t idx) { bias_data( bias, idx, idx);}
  void bias_data(double bias) {bias_data( bias, 0, data.size()-1);}
  void scale_data(double scale, size_t idx1, size_t idx2);
  void scale_data(double scale, size_t idx) { scale_data( scale, idx, idx);}
  void scale_data(double scale) {scale_data( scale, 0, data.size()-1);}

  void revert_initialization() {independents.clear();
                                initialized = false;}

 protected:
  virtual bool generate_output();
  bool precheck_output();
  void generate_trivial_output();
  bool index_checks(size_t & idx1, size_t & idx2, std::string func);

 private:
  bool load_data_internal_check( const SizeVec &dim_list );
  bool copy_data(const double * data_in);
  bool copy_data(const DoubleVec & data_in);
  size_t configure_internal_data_structure();
  void configure_support_arrays();
  virtual void generate_base_values();
  size_t generate_interp_index(size_t interp_pt);
  size_t generate_lookup_index();

  // Disable the copy/assignment operators
  GenericMultiInputTable (const GenericMultiInputTable&);
  GenericMultiInputTable& operator = (const GenericMultiInputTable&);
};
#endif
