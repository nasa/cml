/*******************************************************************************
PURPOSE:
  (Table lookup model master controller, AbstractTableLookup is the base class for the
   table manager classes AbstractTableLookup and SimpleTableLookup.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Oct 2017) (New implementation))
  )
*******************************************************************************/

#include <algorithm> //std::find, find_if, any_of

#include "../include/table_lookup_set.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
AbstractTableLookup::AbstractTableLookup()
  :
  independents(),
  dependents(),
  tables_to_destroy()
{
  subscribe_name = "AbstractTableLookup:";
}


/*****************************************************************************
Destructor
*****************************************************************************/
AbstractTableLookup::~AbstractTableLookup()
{
  std::vector<TableIndependentVariable*>::iterator it1 =
                                              independents_to_destroy.begin();
  for (; it1!=independents_to_destroy.end(); ++it1) {
    delete (*it1);
  }

  std::vector<GenericMultiInputTable*>::iterator it2 =  tables_to_destroy.begin();
  for (; it2!=tables_to_destroy.end(); ++it2) {
    delete (*it2);
  }
}

/*****************************************************************************
initialize
Purpose:(Initialize the model)
*****************************************************************************/
void
AbstractTableLookup::initialize()
{
  if (!enabled) {
    return;
  }

  if (initialized) {
    CMLMessage::warn(
      __FILE__, __LINE__, "Redundant Initialization\n",
      "Called the initialize method redundantly.\n"
      "The initialize method has already completed successfully.\n");
    return;
  }

  if (tables.empty()) {
    CMLMessage::error(
      __FILE__, __LINE__, "Incomplete Initialization\n",
      "There are no data tables available for the AbstractTableLookup to use.\n"
      "Initialization failed.\n");
    return;
  }
  if (independents.empty()) {
    CMLMessage::error(
      __FILE__, __LINE__, "Incomplete Initialization\n",
      "The AbstractTableLookup has no registered independent variables to use "
      "for input.\nInitialization failed.\n");
    return;
  }

  if (dependents.empty()) {
    CMLMessage::error(
      __FILE__, __LINE__, "Incomplete Initialization\n",
      "The AbstractTableLookup has no registered dependent variables to populate "
      "for output.\nInitialization failed.\n");
    return;
  }

  // Initialize all of the independent variables
  for (TableIndependentVariable * indep : independents) {
    // If any of the independents fail to initialize, stop the table-manager
    // initialization.
    if (!indep->initialize()) {
      CMLMessage::error(
        __FILE__,__LINE__,"Initialization error.\n",
        "Failure to initialize TableIndependentVariable ",
        indep->get_name(), "\nresults in a failure to initialize the "
        "table-manager.\n");
      return;
    }
  }

  /* Initialize every table registered with this manager.
   * - if any table's initialization fails, this initialization fails, return.
   * - for each table, check that its independent variables are known
   *   to this manager.
   * - NOTE: this could use a std::all_of algorithm, but is left as a for-loop
   *         because it is easier to read.*/
  for (TableItem_t & table_iter: tables) {
    if (!table_iter.first->initialize()) {
      CMLMessage::error(
        __FILE__,__LINE__,"Initialization error.\n",
        "Failure to initialize GenericMultiInputTable results in a\n"
        "failure to initialize the table-manager.\n");
      return;
    }

    /* Now check that all tables have all of their independent variables known
     * to the table-manager.
     * If any table does not meet this requirement, then it will not be able
     * to process a lookup correctly because an independent variable that is
     * not being processed by the manager will not be updated and made
     * available to the table correctly.*/

    /* For the current table (in the table-for-loop), get a reference to the
     * start of the set of independent-variables used by the table.*/
    const IndepPairVec & table_indeps = table_iter.first->get_independents();

    /* Now if ANY OF this table's independent-variables ("table_indeps") have
     * no match found in the manager's independent variables ("independents")
     * (i.e. NONE OF the manager's independents match), then the table cannot
     * be properly processed.
     * - In this case, post an error and return, leaving the table-manager
     *   uninitialized. The table cannot process a lookup
     *   based on an independent variable that is not being processed by
     *   the manager.
     *  Note - each table_indeps element accesses a pair comprising:
     * {{a pointer to the variable itself} , {how it is to be interpreted}}
     * the first element of this pair is the useful one, we need to verify that
     * this variable is known to the manager.*/
    if( std::any_of( table_indeps.begin(), table_indeps.end(),
        [this]( const IndepPair & pair_)
          {return ( std::none_of( independents.begin(), independents.end(),
                    [pair_]( TableIndependentVariable * var_)
                    {return (pair_.first == var_);})); }
      )) {
      CMLMessage::error(
        __FILE__, __LINE__, "Incomplete Initialization\n",
        "One of the tables has an independent variable that is not known\n"
        "to the AbstractTableLookup manager.\n"
        "This is most likely a configuration error.\n"
        "The table itself already initialized correctly, so its\n"
        "independent variable(s) exists. But the table manager will not be\n"
        "able to keep it/them updated.\n"
        "Table Manager initialization failed.\n");
      return;
    }
  }  /* confirmed that, for each table, all its independent-variables are
        initialized and found in the manager's set of all independent-variables
        known to the manager.*/

  // Reach here only if all tables are usable.
  SubscriptionBase::initialize();
}

/********************************************************************************
Method: update
Purpose: (Runs all of the tables in the set.)
********************************************************************************/
bool
AbstractTableLookup::update()
{
  if (!active) {
    return false;
  }

  bool ret_value = true;
  // update all independents:
  for (TableIndependentVariable * indep_iter : independents) {
    if (!indep_iter->update()) {
      CMLMessage::error(
        __FILE__, __LINE__, "Update failure\n",
        "The TableIndependentVariable ", indep_iter->get_name(),
        "failed to update.\n"
        "Tables depending on it will likely fail to update correctly.\n");
      ret_value = false;
    }
  }

  // update all tables:
  for (TableItem_t & table_iter : tables) {
    // if table is enabled, update it.  If update fails, return false
    // if table is not enabled, ignore it.
    if (table_iter.second && !table_iter.first->update()) {
      CMLMessage::error(
        __FILE__, __LINE__, "Update failure\n",
        "The GenericMultiInputTable failed to update.\n"
        "Its dependent variable(s) may have obsolete values.\n");
      ret_value = false;
    }
  }
  // return true if everything completed succesfully.
  return ret_value;
}

/*****************************************************************************
create_table
Purpose:(creates a new lookup table, tests it, and adds it to the list)
*****************************************************************************/
GenericMultiInputTable*
AbstractTableLookup::create_table(
            double &dependent_variable,
            TableType type)
{
  DoublePtrVec scratch(1, &dependent_variable);
  return create_table(scratch, type);
}

//*****************************************************************************
// WARNING: Use this method with caution.
//          num_vars must match the number of double* arguments!!
GenericMultiInputTable*
AbstractTableLookup::create_table(
            size_t num_vars,
            double *variable_0,
            ...)
{
  DoublePtrVec variables(num_vars);
  va_list variables_in;
  va_start(variables_in, variable_0);
  if (0 < num_vars)
    variables[0] = variable_0;
  for (size_t ii = 1; ii < num_vars; ++ii) {
    variables[ii] = va_arg(variables_in, double*);
  }
  va_end(variables_in);

  return create_table(variables); // default to Generic type, no angular option
}
//*****************************************************************************
GenericMultiInputTable*
AbstractTableLookup::create_table(
            const DoublePtrVec & variables,
            TableType type)
{
  for ( double* var : variables) {
    verify_and_add_dependent_variable( var);
  }

  GenericMultiInputTable *new_table = nullptr;
  switch(type) {
  case Generic:
    new_table = new GenericMultiInputTable(variables);
    break;
  case GenericSingle:
    new_table = new GenericSingleInputTable(variables);
    break;
  case AngularDeg:
    new_table = new SingleInputTableForAngles(variables, false);
    break;
  case AngularRad:
    new_table = new SingleInputTableForAngles(variables);
    break;
  case Quaternion:
    new_table = new SingleInputTableForQuaternions(variables);
    break;
  case VarDeriv:
    new_table = new SingleInputTableVarDeriv(variables);
    break;
  default:
    // FAILSAFE - the list above should be comprehensive so this should not be
    //            accessible.
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid option.\n",
    "Invalid option (",type,") passed to create table.\n"
    "For Legal options, see TableType enumerated list.\n");
  }

  tables_to_destroy.push_back(new_table);
  tables.push_back(TableItem_t(new_table, true));
  return new_table;
}

/*****************************************************************************
create_independent_variable
Purpose:(creates a TableIndependentVariable of the specified size and name)
*****************************************************************************/
TableIndependentVariable*
AbstractTableLookup::create_independent_variable(
    const std::string &name_in,
    const double &variable_in,
    const TableIndependentVariable::Continuity continuity)
{
  verify_independent_name(name_in);

  TableIndependentVariable* new_var =
    new TableIndependentVariable( name_in, variable_in, continuity );

  independents_to_destroy.push_back(new_var);
  independents.push_back(new_var);
  return new_var;
}

/*****************************************************************************
verify_independent_name
Purpose:(Verifies that no two independent variables are tagged with the same
         name)
*****************************************************************************/
void
AbstractTableLookup::verify_independent_name(
       const std::string &name_in)
{
  if (name_in.empty()) {
    // No verification performed on empty strings.
    // Architectural decision made to disallow lookup of variable by its empty
    // string name, so there is no danger of getting the wrong one when
    // multiple names have not been assigned.
    return;
  }

  TableIndependentVariable *ret = AbstractTableLookup::lookup_independent(name_in);
  if (ret != NULL) {
    CMLMessage::fail(
      __FILE__, __LINE__, "Invalid variable addition\n",
      "The attempted addition of an independent variable to a table set\n"
      "caused a duplication in variable names (",name_in,").\n"
      "TableIndependentVariable instance-names may be omitted entirely if\n"
      "they are not needed but may not be duplicated.\n"
      "Variables may be represented by / associated with multiple instances\n"
      "of TableIndependentVariable (e.g. pressure_hifi, pressure_lofi both\n"
      "represent pressure). In such a case, they must be given unique names\n"
      "if a name is specified.\n");
       //FATAL. exit
  }
}


/*****************************************************************************
is_a_dependent_variable
Purpose:(returns a bool indicating whether the specified dependent variable
         is populated by this table-set.)
*****************************************************************************/
bool
AbstractTableLookup::is_a_dependent_variable( double &variable)
{
  DoublePtrVec::iterator it = std::find(dependents.begin(),
                                                dependents.end(),
                                                &variable);
  return dependents.end()!=it;
}

/*****************************************************************************
verify_and_add_dependent_variable
Purpose:(Verifies that the output variable from any table is unique and
         defined.)
*****************************************************************************/
void
AbstractTableLookup::verify_and_add_dependent_variable(double *variable_ptr)
{
  if (variable_ptr == NULL) {
    CMLMessage::fail(
      __FILE__, __LINE__, "Invalid table addition\n",
      "One of the output variables from the table lookup has no target to "
      "populate.\nAll output data must have a specified target.\n");
  }

  if (is_a_dependent_variable(*variable_ptr)) {
    CMLMessage::fail(
      __FILE__, __LINE__, "Invalid table addition\n",
      "The attempted addition of a table to a table-set caused a duplication\n"
      "of output variables.\n"
      "It is not clear which of these table-outputs will be used to populate\n"
      "the output variable.\nFatal error.\n");
  }
  // tested variable does not match any existing outputs from this set.  Add it
  // to the list
  dependents.push_back (variable_ptr);
}

/*****************************************************************************
lookup_independent)
Purpose:(Returns the address to the TableIndependentVariable with matching
         name. An invalid name will result in a NULL return.)
*****************************************************************************/
TableIndependentVariable*
AbstractTableLookup::lookup_independent(
    const std::string &var_name)
{
  auto it = std::find_if(
                 independents.begin(), independents.end(),
                 [var_name] (TableIndependentVariable * var_) {
                   return (var_name.compare(var_->get_name()) == 0);
                 });
  if (it == independents.end()) { return nullptr; }
  return *it;
}

/*****************************************************************************
is_table_interp_enabled
Purpose:(identifies whether a specific table is enabled.)
*****************************************************************************/
bool
AbstractTableLookup::is_table_interp_enabled(
    const GenericMultiInputTable *tbl) const
{
  if (!tbl || tables.empty()) {return false;}
  auto it = std::find_if( tables.begin(), tables.end(),
                          [tbl] (const TableItem_t & table_) {
                            return (table_.first == tbl);
                          });
  if (it == tables.end()) {return false;}
  return (*it).second;
}

/*****************************************************************************
enable_table_interp
Purpose:(sets the boolean flag on a specific table to indicate whether the
         manager should be updating it.)
*****************************************************************************/
void
AbstractTableLookup::enable_table_interp(
    const GenericMultiInputTable *tbl, bool flag)
{
  if (!tbl || tables.empty()) return;

  auto it = std::find_if( tables.begin(), tables.end(),
                          [tbl] (const TableItem_t & table_) {
                            return (table_.first == tbl);
                          });
  if (it != tables.end()) {
    (*it).second = flag;
  }
}
