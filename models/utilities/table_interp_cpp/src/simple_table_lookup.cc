/*******************************************************************************
PURPOSE:
  (Table lookup model)

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2014) (New implementation for C++ models))
   ((Bingquan Wang) (OSR) (Nov 2016) (Fixed the mismatch of new[] and delete))
   ((Bingquan Wang) (OSR) (Apr 2017) (Disabled the compilation warning of float-point
                                      number equality comparision))
   ((Bingquan Wang) (OSR) (Aug 2017) (IVV code cleanup and refactored))
   ((Brian Birmingham) (OSR) (June 2022) (Data reload capability))
  )
*******************************************************************************/

#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/simple_table_lookup.hh"


/********************************************************************************
Method: SimpleTableLookup
Purpose: (Constructor)
********************************************************************************/
SimpleTableLookup::SimpleTableLookup()
  :
  AbstractTableLookup(),
  lookup_method(TableIndependentVariable::Interp),
  indep_data_loaded(false),
  dependent_data_loaded(false),
  data_reloading(false),
  independent(NULL),
  comprehensive_data_reload(false)
{}

/*****************************************************************************
load_independent_data
Purpose:(Create the table-variable and populate it with independent-variable
         data.
Return:(Always returns the value of the independent_data_loaded variable)
*****************************************************************************/
bool
SimpleTableLookup::load_independent_data(
    const double                               & independent_variable,
    const double                               * data_in,
    const size_t                                 num_elements,
    const TableIndependentVariable::LookupMethod lookup,     //optional
    const TableIndependentVariable::Continuity   continuity) //optional
{
  if (data_in == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid data load\n",
      "Null pointer was passed in as the origin of the data source array.\n"
      "Data load failed.\n");
    return false;
  }

  DoubleVec scratch(data_in, data_in+num_elements);
  return load_independent_data( independent_variable,
                                scratch,
                                lookup,
                                continuity);
}
/****************************************************************************/
bool
SimpleTableLookup::load_independent_data(
    const double                               & independent_variable,
    const DoubleVec                            & data,
    const TableIndependentVariable::LookupMethod lookup,      //optional
    const TableIndependentVariable::Continuity   continuity)  //optional
{
  // Store its interpretation for application at initialization.
  lookup_method = lookup;

  if (!independents.empty()) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Data warning:\n",
      "Independent variable has already been created.\n"
      "Overwriting data.\n");
    // Delete the independent if this class was responsible for creating it
    // (which it almost certainly was).
    // We are about to create a new one and there can be only one with the
    // Simple TableLookup.
    if (!independents_to_destroy.empty()) {
      delete independents_to_destroy[0];
    }
    // regardless of whether this class was responsible for the memory
    // space, clear out the references to this obsolete instance.
    independents.clear();
    independents_to_destroy.clear();
  }
  // Now create a whole new independent variable, adding it to the
  // independents vector at index 0.
  TableIndependentVariable * new_var = create_independent_variable(
                                                         "default",
                                                         independent_variable,
                                                         continuity);
  independent = new_var;
  indep_data_loaded = new_var->load_data( data);

  // Now we need to be very careful if we are re-loading data
  // post-initialization (note -- data_reloading is an internal flag that
  // means "was initialized" and is associated with re-loading both
  // dependent and independent data; it would be true only if the dependent
  // data has already been re-loaded as part1 of a complete post-init re-load).
  // If the model has already gone through the initialization process, that
  // included all of the sanity checks to make sure that the dependent
  // variable and independent variable are populated with data that are
  // consistently sized and internally valid.
  // By loading new independent data, we just invalidated all of those
  // sanity checks by redefining our independent variable data, which may
  // have involved resizing the independent variable array. So if running
  // post-initialization, we have some additional checks:
  if (initialized || data_reloading) {
    // At this point, we could pull out those components of the
    // initialization sequence that are relevant to a re-initialization, but
    // the reality is that this includes most of the sequence.
    // So the safest and easiest path is to revert the initialization flags
    // and reinitialize everything.

    // switch any subscriptions over to pending, to be reapplied at
    // (re)-initialization
    sub_pending += num_subscriptions;
    num_subscriptions = 0;
    active = false;

    // Now revert the initialization flags. With this change, we will have to
    // re-initialize to get those subscriptions re-applied. This way, if
    // anything goes awry, the model can't be accidentally re-activated
    // with a new subscription following a failed (re)-initialization.
    initialized = false;

    // One caveat: if both the independent and dependent variables are
    // being overwritten (although in this case, it would probably be
    // preferable to use a different instance of SimpleTableLookup),
    // we don't want to reinitialize with the old dependent data still
    // in situ. Such a scenario is recognized with the
    // "comprehensive_data_reload" flag.
    if (comprehensive_data_reload) {
      // First, reset this flag to false to indicate that we are no longer
      // waiting to load both independent and dependent data (the independent
      // just got loaded). Set the "data_reloading" flag to true to
      // indicate that we are midway through the data-reload (have the
      // independent, still waiting for the dependent).
      // Note - if the dependent data re-load was processed first -- before
      // this independent data re-load -- then that process
      // would also have set comprehensive_data_reload = false and
      // data_reloading = true, and we would now be ready to re-initialize
      // the model.
      comprehensive_data_reload = false;
      data_reloading = true;
    }
    else { // re-loading the independent data completed the data re-load, so
           // we can re-initialize.

      // During the initialization process, consistency in array sizing is
      // checked during the initialization of the *dependent* table, so it is
      // important to mark the dependent data as uninitialized -- even if
      // it has not changed -- so that its size will be compared against
      // the new independent data. For this, we revert the initialization
      // of the dependent data.
      // First, make sure that there is a valid pointer to a dependent
      // data table. It would be difficult -- but not impossible -- to get
      // here without having a dependent data table (this would require
      // that there was a failed attempt to re-load the dependent-data)
      if (!tables.empty()) {
        // Note - Having no tables will be detected in the upcoming initialize()
        // call and initialization will fail with associated message ... so
        // we don't need to take any action here upon this detection.
        // This is basically a nullpointer check to avoid trying to access
        // non-existent data.

        // Note -- revert_initialization also clears out the old pointer to the
        // independent variable, allowing the initialize() method to replace it
        // with the new one.
        tables[0].first->revert_initialization();
      }
      // Note -- do not need to flag the independent variable. We just
      //         created a new one, so it is definitely not initialized.

      // Now re-initialize the whole package.
      initialize();
      // In the case of this being the second half of a complete data re-load,
      // data_reloading would be true but the reload is now complete so reset
      // it. Otherwise it would be false and setting it to false is harmless.
      data_reloading = false;
    }
  }

  return indep_data_loaded;
}

/*****************************************************************************
load_dependent_data
Purpose:(Create the table and populate it with dependent-variable data.
Return:(Always returns the value of the dependent_data_loaded variable)
*****************************************************************************/
bool
SimpleTableLookup::load_dependent_data(
         double       & dependent_var,
         const double * data,
         const size_t   num_data_points,
         TableType      type) //optional
{
  if (data == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid data load\n",
      "Null pointer was passed in as the origin of the data source array.\n"
      "Data load failed.\n");
    return false;
  }

  // create a vector containing a single point being the address of the
  // dependent variable.
  std::vector<double *> scratch_var(1, &dependent_var);
  DoubleVec scratch_data(data, data+num_data_points);
  return load_dependent_data( scratch_var,
                              scratch_data,
                              num_data_points,
                              type);
}
/****************************************************************************/
bool
SimpleTableLookup::load_dependent_data(
         double          & dependent_var,
         const DoubleVec & data,
         TableType         type) //optional
{
  // create a vector containing a single point being the address of the
  // dependent variable.
  std::vector<double *> scratch_var(1, &dependent_var);
  return load_dependent_data( scratch_var,
                              data,
                              data.size(),
                              type);
}
/****************************************************************************/
bool
SimpleTableLookup::load_dependent_data(
         double       * dependent_vars,
         const size_t   num_vars,
         const double * data,
         const size_t   num_data_points_per_variable,
         TableType      type) //optional
{
  if (data == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid data load\n",
      "Null pointer was passed in as the origin of the data source array.\n"
      "Data load failed.\n");
    return false;
  }
  if (dependent_vars == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid data load\n",
      "Null pointer was passed in as the origin of the dependent-variables "
      "array.\nData load failed.\n");
    return false;
  }
  std::vector<double *> scratch_vars(num_vars, NULL);
  for (unsigned int ii = 0; ii < num_vars; ++ii) {
    scratch_vars[ii] = dependent_vars + ii;
  }
  size_t num_data_points = num_data_points_per_variable * num_vars;
  DoubleVec scratch_data(data, data+num_data_points);
  return load_dependent_data( scratch_vars,
                              scratch_data,
                              num_data_points_per_variable,
                              type);
}
/****************************************************************************/
bool
SimpleTableLookup::load_dependent_data(
         double          * dependent_vars,
         const size_t      num_vars,
         const DoubleVec & data,
         const size_t      num_data_points_per_variable,
         TableType         type) //optional
{
  if (dependent_vars == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid data load\n",
      "Null pointer was passed in as the origin of the dependent-variables "
      "array.\nData load failed.\n");
    return false;
  }
  std::vector<double *> scratch_vars(num_vars, NULL);
  for (unsigned int ii = 0; ii < num_vars; ++ii) {
    scratch_vars[ii] = dependent_vars + ii;
  }
  return load_dependent_data( scratch_vars,
                              data,
                              num_data_points_per_variable,
                              type);
}
/****************************************************************************/
bool
SimpleTableLookup::load_dependent_data(
         const DoublePtrVec & dependent_vars,
         const double       * data,
         const size_t         num_data_points_per_variable,
         TableType            type) //optional
{
  if (data == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid data load\n",
      "Null pointer was passed in as the origin of the data source array.\n"
      "Data load failed.\n");
    return false;
  }
  size_t num_data_points = num_data_points_per_variable * dependent_vars.size();
  DoubleVec scratch_data(data, data+num_data_points);
  return load_dependent_data( dependent_vars,
                              scratch_data,
                              num_data_points_per_variable,
                              type);
}
/****************************************************************************/
bool
SimpleTableLookup::load_dependent_data(
         const DoublePtrVec & dependent_vars,
         const DoubleVec    & data,
         const size_t         num_data_points_per_variable,
         TableType            type)
{
  // NOTE - ultimately all other options come through this version of
  // load_dependent_data.  The others just massage the inputs until they look
  // like these inputs and then come here anyway.
  size_t num_deps = dependent_vars.size();
  if (num_deps < 1 || num_data_points_per_variable < 1) {
    CMLMessage::error(
      __FILE__,__LINE__,"Data error:\n",
      "The number of dependents (",num_deps,") and number of data points "
      "per variable (",num_data_points_per_variable,")\n"
      "must be at least 1.  Table data load failed.\n");
    return false;
  }

  // Check size of data:
  size_t target_size =  num_deps * num_data_points_per_variable;
  if (data.size() < target_size) {
    CMLMessage::error(
      __FILE__,__LINE__,"Data error:\n",
      "There is insufficient data to populate :\n",
      num_deps," variables at\n",
      num_data_points_per_variable," values per variable.\n"
      "Data set has only ",data.size()," elements.\n");
    return false;
  }

  if (!tables.empty()) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Data warning:\n",
      "Data table has already been created.\n"
      "Overwriting data.\n");
    // Delete the dependent if this class was responsible for creating it
    // (which it almost certainly was).
    // We are about to create a new one and there can be only one with the
    // Simple TableLookup.
    if (!tables_to_destroy.empty()) {
      delete tables_to_destroy[0];
    }
    // regardless of whether this class was responsible for the memory
    // space, clear out the references to this obsolete instance.
    tables.clear();
    tables_to_destroy.clear();
    dependents.clear();
  }

  SizeVec dim_list(2);
  dim_list[0] = num_deps;
  dim_list[1] = num_data_points_per_variable;

  GenericMultiInputTable * table = create_table(dependent_vars, type);
  // "table" already checked against NULL.

  dependent_data_loaded = table->load_data( data, dim_list);

  // See comments in load_independent_data(...) for more detailed
  // description of what is happening in this if(initialized) block.
  // Briefly, if the model is already initialized, we are re-loading data
  // mid-sim and need to re-run the sanity checks that were carried out at
  // initialization.
  if (initialized || data_reloading) {
    sub_pending += num_subscriptions;
    num_subscriptions = 0;
    active = false;
    initialized = false;

    if (comprehensive_data_reload) {
      comprehensive_data_reload = false;
      data_reloading = true;
    }
    else {
      // Deliberate symmetry break with the load_independent_data(...) flow.
      // Do not need to mark the indep table as uninitialized.
      initialize();
      data_reloading = false;
    }
  }
  return dependent_data_loaded;
}

/*****************************************************************************
initialize
Purpose:(
  Initialization check.  Note that some of these checks are redundant with
  the same checks made in the forwarded AbstractTableLookup::initialize()
  method.  Both classes need these checks -- the AbstractTableLookup to
  handle initialization of th emore generic model implementation, and this
  class to protect the population of the assignment of the independent to
  the dependent table near the end of this method. We could avoid some of
  this duplication by splitting AbstractTableLookup::initialize(), but
  it's not worth it.
*****************************************************************************/
void
SimpleTableLookup::initialize()
{
  if (!enabled) {
    return;
  }
  // If already initialized, skip the specifics and just grab the message
  // from AbstractTableLookup.
  if (!initialized) {
    if (independents.empty()) {
      CMLMessage::error(
        __FILE__,__LINE__,"Initalization Error.\n",
        "Incomplete initialization.  There is no independent variable.\n");
      return;
    }
    if (!indep_data_loaded) {
      CMLMessage::error(
        __FILE__,__LINE__,"Initalization Error.\n",
        "Incomplete initialization.  There is no independent data.\n");
      return;
    }

    if (tables.empty()) {
      CMLMessage::error(
        __FILE__,__LINE__,"Initalization Error.\n",
        "Incomplete initialization.  There is no data table.\n");
      return;
    }
    if (!dependent_data_loaded) {
      CMLMessage::error(
        __FILE__,__LINE__,"Initalization Error.\n",
        "Incomplete initialization.  There is no dependent data.\n");
      return;
    }
    // Associate the one independent with the one dependent.
    tables[0].first->add_independent( *(independents[0]),
                                      0,
                                      lookup_method);
  }

  AbstractTableLookup::initialize();
  if (!initialized) {
    tables[0].first->revert_initialization();
  }
}


/*****************************************************************************
get_table
Purpose: Returns a pointer to the table.
*****************************************************************************/
GenericMultiInputTable *
SimpleTableLookup::get_table()
{
  if (tables.empty()) {
    return nullptr;
  }

  return tables[0].first;
}
