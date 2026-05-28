/*******************************************************************************
PURPOSE:
  (Table lookup model)

LIBRARY DEPENDENCY:
  (../src/simple_table_lookup.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2014) (New implementation for C++ models))
   ((Bingquan Wang) (OSR) (August 2017) (IVV code cleanup and refactored))
   ((Brian Birmingham) (OSR) (June 2022) (Data reload capability))
  )
*******************************************************************************/

#ifndef ANTARES_SIMPLE_TABLE_LOOKUP_HH
#define ANTARES_SIMPLE_TABLE_LOOKUP_HH


#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "abstract_table_lookup.hh"
#include "table_independent_variable.hh"
#include "table_type_defs.hh"


// This class provides the simple, most used lookup case where one independent
// variable is used as the basis for one-to-many dependent variables.
// It is related to the TableLookupSet class; while the latter provides the
// general capability of multiple linked tables across multiple independent
// variables, this class provides a more efficient algorithm and simpler
// interface for handling the case of a single independent variable.
//
// It USES one of the single-input tables (e.g. GenericSingleInputTable,
// SingleInputTableForAngles) for a table, and one of the
// TableIndependentVariable classes for monitoring the independent variable.
// This is the front-end to those classes.
//
// There is a distinction between the classes that represent the tables:
//   (GenericMultiInputTable and derivatives thereof)
// and those that manage the connection between input and output
//   (AbstractTableLookup and derivatives thereof - SimpleTableLookup, TableLookupSet).
// It is important to recognize that this class is not an INSTANCE of a
// GenericMultiInputTable, it is the MANAGER of one.

class SimpleTableLookup : public AbstractTableLookup
{
private:
  // The table resides in this base class; even if this gets extended, it still
  // resides here.  So these table-dependent values can be private to this
  // class.
  TableIndependentVariable::LookupMethod lookup_method; /* (--)
       Specifies the manner in which the independent variable data is to be
       handled.  Defaults to interpolation (Interp).*/
  bool indep_data_loaded;     /* (--) independent-data-loaded flag */
  bool dependent_data_loaded; /* (--) dependent-data-loaded flag.*/
  bool data_reloading; /* (--) in the process of re-loading data post-init. */

public:
  TableIndependentVariable * independent; /* (--)
    pointer to the independent variable. Note that this is identical to
    independents[0]; this is the public-facing interface to the element in
    the protected independents vector.*/
  bool comprehensive_data_reload; /* (--)
    A flag used only if re-loading data post-initialization. This flag
    indicates that both independent and dependent data are being reloaded.
    The re-initialization will be held until both sets have been populated.*/

  /*  Note (Birmingham/2022) -- change from using default constructor with care.
  Because of Trick checkpointing issues, the use of STL-containers of
  SimpleTableLookup is problematic, and C-style arrays of SimpleTableLookup
  are used instead in models using this class.
  This requires that SimpleTableLookup keep its default constructor form. */
  SimpleTableLookup();
  virtual ~SimpleTableLookup(){};


  bool load_dependent_data(
           double         & dependent_var,
           const double   * data,
           const size_t     num_data_points_per_variable,
           TableType        type = GenericSingle);
  bool load_dependent_data(
           double           & dependent_var,
           const DoubleVec  & data,
           TableType          type = GenericSingle);
  bool load_dependent_data(
           double       * dependent_vars,
           const size_t   num_vars,
           const double * data,
           const size_t   num_data_points_per_variable,
           TableType      type = GenericSingle);
  bool load_dependent_data(
           double           * dependent_vars,
           const size_t       num_vars,
           const DoubleVec  & data,
           const size_t       num_data_points_per_variable,
           TableType          type = GenericSingle);
  bool load_dependent_data(
           const DoublePtrVec & dependent_vars,
           const double       * data,
           const size_t         num_data_points_per_variable,
           TableType            type = GenericSingle);
  bool load_dependent_data(
           const DoublePtrVec & dependent_vars,
           const DoubleVec    & data,
           const size_t         num_data_points_per_variable,
           TableType            type = GenericSingle);

  bool load_independent_data(
           const double  & independent_variable,
           const double  * data,
           const size_t    num_elements,
           const TableIndependentVariable::LookupMethod lookup_method =
                                            TableIndependentVariable::Interp,
           const TableIndependentVariable::Continuity   continuity =
                                            TableIndependentVariable::Linear);
  bool load_independent_data(
           const double    & independent_variable,
           const DoubleVec & data,
           const TableIndependentVariable::LookupMethod lookup_method =
                                            TableIndependentVariable::Interp,
           const TableIndependentVariable::Continuity   continuity =
                                            TableIndependentVariable::Linear);

  virtual void initialize() override;
  GenericMultiInputTable * get_table();

private:
  // to disable the copy/assignment operators
  SimpleTableLookup (const SimpleTableLookup&);
  SimpleTableLookup& operator = (const SimpleTableLookup&);
};
#endif
