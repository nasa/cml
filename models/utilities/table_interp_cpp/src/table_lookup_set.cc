/*******************************************************************************
PURPOSE:
  (Table lookup model master controller)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Dec 2015) (New implementation))
   ((Bingquan Wang) (OSR) (Aug 2017) (IVV code cleanup and refactored))
  )
*******************************************************************************/

#include <cstdarg> // for variadic function arguments

#include "../include/table_lookup_set.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
TableLookupSet::TableLookupSet()
{
  tables.reserve(5);
  independents.reserve(5);
  dependents.reserve(10);
  tables_to_destroy.reserve(5);
  independents_to_destroy.reserve(5);
}


/*****************************************************************************
add_table
Purpose:(Adds a pre-created table to the lookup list.
         Because all tables will be run at every lookup point,
         it is imperative that no output variables be duplicated.)
Usage: (Called externally to add a table built elsewhere)
*****************************************************************************/
void
TableLookupSet::add_table( GenericMultiInputTable &new_table)
{
  if (initialized) {
    CMLMessage::error(
      __FILE__, __LINE__, "Invalid sequence\n",
      "Attempt to add new table.\n"
      "It is not possible to add tables after the manager\n"
      "has been initialized.\n");
    return;
  }

  size_t num_vars = new_table.get_num_dep_vars();
  for (size_t ii = 0; ii < num_vars; ++ii) {
    double *var_ptr = new_table.get_dependent_variable_ptr(ii);
    verify_and_add_dependent_variable( var_ptr);
  }
  // If still going here, the verifications passed.  The dependent variables
  // have not been assigned to other tables.  However, that does not mean this
  // table is unique.  Consider this quirky sequence:
  //   1. Add an empty table to the manager
  //   2. Add the dependent variable(s) to the table
  //   3. Add the table to the manager again.
  // In this scenario, the second addition of the table will be a duplicate,
  // but checking the dependent variables is unsufficient.  So check the tables
  // as well.
  std::vector<TableItem_t>::iterator it_tab =  tables.begin();
  for (; it_tab!=tables.end(); ++it_tab) {
    if (&new_table == (*it_tab).first) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Initialization duplication detected.\n",
        "The same table was added to the table-manager twice.\n"
        "Omitting the second instance of this table, but including\n"
        "the second instance's dependent variables\n");
      return;
    }
  }

  // Add this table to the list and default it to be enabled.
  tables.push_back(AbstractTableLookup::TableItem_t(&new_table, true));
}

/*****************************************************************************
add_independent_variable
Purpose:(Adds an independent variable created elsewhere.  External call.)
*****************************************************************************/
void
TableLookupSet::add_independent_variable(
      TableIndependentVariable &var_in)
{
  if (initialized) {
    CMLMessage::error(
      __FILE__, __LINE__, "Invalid sequence\n",
      "Attempt to add independent variable ",var_in.get_name(),"\n"
      "It is not possible to add an independent variable after the model\n"
      "has been initialized.\n");
    return;
  }

  // Check that it has not already been added:
  std::vector<TableIndependentVariable*>::iterator it = independents.begin();
  for (; it != independents.end(); ++it) {
    if (&var_in == *it) {
      CMLMessage::warn(
        __FILE__, __LINE__, "Duplicate variable addition\n",
        "Attempted to add variable ",var_in.get_name(),
        " but it has already been added.\n"
        "Addition aborted.\n");
      return;
    }
  }

  // Check that it has a unique name, names are used for lookups.
  // Note - no-name is ok, no-name cannot be used for a lookup so no risk of
  //        getting the wrong one.
  // Failure (i.e. name duplication) results in termination.
  verify_independent_name(var_in.get_name());

  // Add this independent to the list.
  independents.push_back(&var_in);
}

/*****************************************************************************
associate_table_and_independent
Purpose:(Adds the independent variable to the last table in the tables list.
         There are multiple options for the performing this task)
*****************************************************************************/
void
TableLookupSet::associate_table_and_independent(
                    TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(tables.back().first,
                                  independents.back(), lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
     size_t table_ix,
     TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(tables.back().first,
                                  independents.back(),
                                  table_ix,
                                  lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
    const std::string &var_name,
    TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(tables.back().first,
                                  lookup_independent(var_name),
                                  lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
    const std::string &var_name,
    size_t table_ix,
    TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(tables.back().first,
                                  lookup_independent(var_name),
                                  table_ix,
                                  lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
  TableIndependentVariable *ind_var,
  TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(tables.back().first,
                                  ind_var,
                                  lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
  TableIndependentVariable *ind_var,
  size_t table_ix,
  TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(tables.back().first,
                                  ind_var,
                                  table_ix,
                                  lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
  GenericMultiInputTable *table,
  TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(table,
                                  independents.back(),
                                  lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
  GenericMultiInputTable *table,
  size_t table_ix,
  TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(table,
                                  independents.back(),
                                  table_ix,
                                  lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
  GenericMultiInputTable *table,
  const std::string &var_name,
  TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(table,
                                  lookup_independent(var_name),
                                  lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
  GenericMultiInputTable *table,
  const std::string &var_name,
  size_t table_ix,
  TableIndependentVariable::LookupMethod lookup_method)
{
  associate_table_and_independent(table,
                                  lookup_independent(var_name),
                                  table_ix,
                                  lookup_method);
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
  GenericMultiInputTable *table,
  TableIndependentVariable *ind_var,
  TableIndependentVariable::LookupMethod lookup_method)
{
  if (ind_var != NULL && table != NULL) {
    table->add_independent(*ind_var, lookup_method);
  }
}

//*****************************************************************************
void
TableLookupSet::associate_table_and_independent(
  GenericMultiInputTable  *table,
  TableIndependentVariable *ind_var,
  size_t table_ix,
  TableIndependentVariable::LookupMethod lookup_method)
{
  if (ind_var != NULL && table != NULL) {
    table->add_independent(*ind_var, table_ix, lookup_method);
  }
}

/*****************************************************************************
lookup_independent
Purpose:(Returns the address to the TableIndependentVariable with matching
         name.)
*****************************************************************************/
TableIndependentVariable*
TableLookupSet::lookup_independent(
    const std::string &var_name)
{
  if (var_name.empty()) {
    CMLMessage::error(
      __FILE__, __LINE__, "Invalid variable lookup\n",
      "Cannot identify an independent variable without a name.\n"
      "Specified name is empty.\n"
      "Returning NULL\n");
    return NULL;
  }

  TableIndependentVariable *ret = AbstractTableLookup::lookup_independent(var_name);
  if (ret == NULL) {
    CMLMessage::error(
      __FILE__, __LINE__, "Invalid variable lookup\n",
      "No independent variable named ",var_name," was found in the "
      "table-set.\n");
  }
  return ret;
}
