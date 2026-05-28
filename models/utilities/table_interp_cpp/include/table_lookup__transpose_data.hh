/*******************************TRICK HEADER******************************
PURPOSE: (
     Provides a table-lookup-set when the data is presented in a
     transpose  presentation.  The format assumed for data presentation
     by the table-interpolation model is variable-by-variable:
         {var1(x_1), var1(x_2), var1(x_3), ...,
          var2(x_1), var2(x_2), var2(x_3), ...,
          var3(x_1), var3(x_2), var3(x_3), ...}
     This format easily supports multi-variable interpolation.
     However, in the case of only one independent variable, the data may
     often be presented line-by-line based on the value of the independent
     variable:
       {{var1(x_1), var2(x_1), var3(x_1), ...}
        {var1(x_2), var2(x_2), var3(x_2), ...}
        {var1(x_3), var2(x_3), var3(x_3), ...}}

     This class supports data presented in the latter format.  The data
     may be subdivided into as many tables as necessary or desired.)

LIBRARY DEPENDENCY:
   ((../src/table_lookup__transpose_data.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Nov 2017) (Antares) (initial)))
**********************************************************************/
#ifndef SINGLE_INPUT_TRANSPOSE_DATA_SET
#define SINGLE_INPUT_TRANSPOSE_DATA_SET

#include <vector>
#include <list>

#include "generic_multi_input_table.hh"
#include "table_lookup_set.hh"
#include "abstract_table_lookup.hh"
#include "table_type_defs.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
TableLookupTransposeDataSet_TableConfig
Purpose:( Provides configuration information for an instance of a table that
          will hold a subset of the data from the incoming data file.)
Assumptions: ((Data for this table is contiguous within each line of the
               specified data.)
             ((Data for this table is found at the same indices on every
               line of the specified data.)
             )
*****************************************************************************/
class TableLookupTransposeDataSet_TableConfig
{
 friend class TableLookupTransposeDataSet;
 public:
  typedef std::list<DoubleVec>   DoubleVecList;

  std::string name; /* (--) Name of this table-configuration.*/
  bool block_independent_addition; /* (--)
    When used to configure the specific data-set, the corresponding Table
    (table_ptr) will by default be added to the table-manager
    (TableLookupTransposeDataSet). This flag blocks this addition.
    This is used when the configured table is only to be used indirectly.
    Default: false. */
  size_t index_low; /* (--)
    First index in the data file containing contiguous data for this table.
    If the data is not contiguous, specify indices as a vector. */
  size_t index_high; /* (--)
    Last index in the data file containing contiguous data for this table.
    If the data is not contiguous, specify indices as a vector. */
  std::vector<size_t> indices; /* (--)
     Set of indices in the data file containing data for this table.
     Used when data is not contiguous and cannot be specified by a range
     [index_low,index_high].
     If this vector is empty, model will use index_low, index_high.
     If this vector is non-empty, model will ignore index_low, index_high. */

  GenericMultiInputTable * table_ptr; /* (--) pointer to this model's
                            data-table.  May be left NULL, in which case a
                            table will be created on-the-fly. */
  DoublePtrVec  dependent_variables; /* (--) STL-vector containing pointers
                            to the dependent variables; these variables will
                            be populated by the table found at table_ptr.  Used
                            only if the table is being constructed on-the-fly.*/
  AbstractTableLookup::TableType table_type;/* (--) Type of table to be created if
                            creating a table on-the-fly. Otherwise, this is
                            unused.  Default: Generic.*/
  TableIndependentVariable::LookupMethod lookup_method; /* (--) Type of lookup
                            method to be used by the associated independent
                            variable.*/
 private:
  DoubleVec  data_scratch;  /* (--) STL-vector containing the data for
                            the variables represented by this table.  This is a
                            temporary storage location, available for
                            constructing the data vector from the
                            transpose data before being sent to table_ptr for
                            permanent storage.*/
  size_t num_variables;  /* (count) number of variables in the associated table.*/
  size_t total_num_lines;/* (count) number of lines of data coming in. Set by call
                          from
                          TableLookupTransposeDataSet::process_transpose_data()*/

 public:
  TableLookupTransposeDataSet_TableConfig();
  virtual ~TableLookupTransposeDataSet_TableConfig() {};

 private: // called from TableLookupTransposeDataSet
  void initialize(size_t num_lines);
  void check_indices();
  void parse_line( size_t line_num,
                   const DoubleVec & data_line);
  void populate_table();

  // Using default copy-constructor
};


/*****************************************************************************
TableLookupTransposeDataSet
Purpose:(A TableLookup that populates and holds a set of data populated by
         data presented in a transposed format.)
*****************************************************************************/
class TableLookupTransposeDataSet : public TableLookupSet
{
 public:
  typedef std::list<DoubleVec>   DoubleVecList;

  std::vector<TableLookupTransposeDataSet_TableConfig> table_config; /* (--)
              A set of table-configurations; these are used to extract the data
              from the consolidated data set for loading onto each instance of
              GenericMultiInputTable used by this manager.*/
  const double * independent_var; /* (--)
              A pointer to the variable used as the independent-variable.
              Used only if the TableIndependentVariable is being created
              on-the-fly.*/
  TableIndependentVariable::Continuity indep_continuity; /* (--)
              Indicates how the edges of the independent data are to be treated.
              Used only if the TableIndependentVariable is being created
              on-the-fly.*/

  bool populate_independent_from_file; /* (--)
              Flag indicating that the data for the independent variable is
              also found in the data file.*/
  size_t indep_index; /* (--)
              The index of the data for the independent variable.
              Used only if populate_independent_from_file is true */
 private:
  size_t min_length;     /* (count) number of variables on the shortest line in
                          the data set.*/
  size_t min_length_line;/* (count) line number of the shortest line.*/
  bool data_processed; /* (--) flag indicating that the process_data method
                               has executed and the tables been populated.*/

 public:
  TableLookupTransposeDataSet();
  virtual ~TableLookupTransposeDataSet(){};

  bool process_data( const std::string & filename,
                     size_t terminate_on_line = 100000);
  bool process_data( const DoubleVecList & data_file);
  bool convert_txt_to_list_vec( const std::string & filename,
                                DoubleVecList & data_list,
                                size_t terminate_on_line = 100000);
  TableLookupTransposeDataSet_TableConfig * get_config( const char * name);
  TableLookupTransposeDataSet_TableConfig * get_config( const std::string & name);
  void add_config( TableLookupTransposeDataSet_TableConfig & new_config)
    {  table_config.push_back(new_config);}
  bool remove_config( TableLookupTransposeDataSet_TableConfig & config);
 private:
  void check_independent();
  // Disable the copy/assignment operators
  TableLookupTransposeDataSet ( const TableLookupTransposeDataSet&);
  TableLookupTransposeDataSet& operator = ( const TableLookupTransposeDataSet&);
};
#endif
