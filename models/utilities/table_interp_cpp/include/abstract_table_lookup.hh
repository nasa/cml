/*******************************************************************************
PURPOSE:
  (Abstract Table Lookup provides the base class for the
   SimpleTableLookup and the more generic TableLookupSet.
   This is not an abstract class in its true sense because it has no pure
   virtual methods. However, the intent is that it serve that role;
   the class lacks significant components found in its derivatives,
   and instantiations of this class (rather than using its derivatives --
   SimpleTableLookup or TableLookupSet) are going to be very limited in
   their capability.)

LIBRARY DEPENDENCY:
  (../src/abstract_table_lookup.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Oct 2017) (New implementation))
  )
*******************************************************************************/

#ifndef ANTARES_ABSTRACT_TABLE_LOOKUP_HH
#define ANTARES_ABSTRACT_TABLE_LOOKUP_HH

#include "generic_multi_input_table.hh"
#include "table_independent_variable.hh"

// These headers support dynamic allocation:
#include "generic_single_input_table.hh"
#include "single_input_table_for_angles.hh"
#include "single_input_table_for_quaternions.hh"
#include "single_input_table_var_with_deriv.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include <vector>
#include <string>

class AbstractTableLookup : public SubscriptionBase
{
public:
  typedef std::pair<GenericMultiInputTable*, bool> TableItem_t;

protected:
  // NOTE - these are vectors of pointers rather than vectors of instances to
  //        support the architecture in which the elements are instantiated by
  //        the user outside and imported into this model.

  std::vector<TableItem_t> tables; /* (--)
       STL vector of pointers to all known tables managed by this class.*/
  std::vector<TableIndependentVariable *> independents; /* (--)
       STL vector of pointers to all known IndependentVariable instances.*/
  DoublePtrVec dependents; /* (--)
       STL vector of pointers to all dependent variables.  These are external
       to the model. */
  std::vector<GenericMultiInputTable *> tables_to_destroy; /* (--)
       STL vector of pointers to all tables that were created by this model.
       Note - tables may be created externally and linked in.  Such an instance
              would be present in the tables vector and not in the
              tables_to_destroy vector.*/
  std::vector<TableIndependentVariable *> independents_to_destroy; /* (--)
       STL vector of pointers to all independents created by this model.
       Note - independents may be created externally and linked in.  Such an
              instance would be present in the independents vector and not in
              the independents_to_destroy vector.*/

public:
  enum TableType {// not instantiated, used only as an input.
    Generic,      // Straight multi-input table
    GenericSingle,// Straight single-input table
    AngularDeg,   // Dependent data is checked for wrap-around conditions at 360
                  // so interpolation goes to the correct quadrant of the circle
    AngularRad,   // Dependent data is checked for wrap-around conditions at pi
                  // so interpolation goes to the correct quadrant of the circle
    Quaternion,   // table has access to a spherical interpolation algorithm.
    VarDeriv      // table uses the relationship between a variable and its
                  // derivative to fit to a cubic polynomial
  };

  AbstractTableLookup();
  virtual ~AbstractTableLookup();

  virtual void initialize();
  virtual bool update();

  virtual GenericMultiInputTable * create_table(
                                     double &output_variable,
                                     TableType type = Generic);
  virtual GenericMultiInputTable * create_table(
                                     size_t num_vars,
                                     double *variable_0,
                                     ...);
  virtual GenericMultiInputTable * create_table(
                                     const DoublePtrVec & variables,
                                     TableType type = Generic);

  TableIndependentVariable* create_independent_variable(
                         const std::string &name_in,
                         const double &variable_in,
                         const TableIndependentVariable::Continuity continuity =
                                          TableIndependentVariable::Linear);

  bool is_a_dependent_variable( double &variable);

  bool is_table_interp_enabled(const GenericMultiInputTable *tbl) const;
  void enable_table_interp(const GenericMultiInputTable *tbl, bool flag);

protected:
  void verify_and_add_dependent_variable(double *variable_ptr);
  void verify_independent_name(const std::string &name_in);
  virtual TableIndependentVariable* lookup_independent(
                                                 const std::string &var_name);

private:
  // Prevent accidental copy and assignment:
  AbstractTableLookup (const AbstractTableLookup&);
  AbstractTableLookup& operator = (const AbstractTableLookup&);
};
#endif
