/*******************************************************************************
PURPOSE:
  (Table lookup model)

LIBRARY DEPENDENCY:
  (../src/table_lookup_set.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Dec 2015) (New implementation))
   ((Bingquan Wang) (OSR) (Aug 2017) (IVV code cleanup and refactored))
  )
*******************************************************************************/

#ifndef ANTARES_TABLE_LOOKUP_SET_HH
#define ANTARES_TABLE_LOOKUP_SET_HH

#include <vector>
#include <string>
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "abstract_table_lookup.hh"
#include "generic_multi_input_table.hh"
#include "table_independent_variable.hh"

class TableLookupSet : public AbstractTableLookup
{
public:
  TableLookupSet();
  virtual ~TableLookupSet(){};

  void add_table( GenericMultiInputTable &new_table);
  void add_independent_variable( TableIndependentVariable &var_in);

  void associate_table_and_independent(
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     size_t table_ix,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     const std::string  &var_name,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     const std::string  &var_name,
                     size_t table_ix,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     TableIndependentVariable *ind_var,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     TableIndependentVariable *ind_var,
                     size_t table_ix,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     GenericMultiInputTable *table,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     GenericMultiInputTable *table,
                     size_t table_ix,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     GenericMultiInputTable *table,
                     const std::string &var_name,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     GenericMultiInputTable *table,
                     const std::string &var_name,
                     size_t table_ix,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     GenericMultiInputTable *table,
                     TableIndependentVariable *ind_var,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  void associate_table_and_independent(
                     GenericMultiInputTable *table,
                     TableIndependentVariable *ind_var,
                     size_t table_ix,
                     TableIndependentVariable::LookupMethod lookup_method
                                         = TableIndependentVariable::Interp);
  virtual TableIndependentVariable* lookup_independent(
                                          const std::string &var_name) override;

private:
  // Prevent accidental copy and assignment:
  TableLookupSet (const TableLookupSet&);
  TableLookupSet& operator = (const TableLookupSet&);
};
#endif
