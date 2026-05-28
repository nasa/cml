/*******************************************************************************
PURPOSE:
  (Table lookup model component representing the independent variable and its
   lookup)

LIBRARY DEPENDENCY:
  (../src/table_independent_variable.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Dec 2015) (New implementation))
   ((Bingquan Wang) (OSR)(Aug 2017) (IVV code cleanup and refactored))
  )
*******************************************************************************/

#ifndef ANTARES_TABLE_INDEPENDENT_VARIABLE_HH
#define ANTARES_TABLE_INDEPENDENT_VARIABLE_HH

#include <string>
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "table_type_defs.hh"


/*****************************************************************************
TableIndependentVariable
Purpose:(Members and methods to provide basic lookup functionality on an
         independent variable, so that the location can be determined for use
         in generating data from the dependent-variable data-table)
*****************************************************************************/
class TableIndependentVariable
{
public:
  // The method used to identify which calibration point(s) should be used
  // to generate the output.
  // In general, the independent variable will take a value between two
  // adjacent calibrated points, this enumeration provides the control for
  // how each DEPENDENT variable uses the value of each of its associated
  // INDEPENDENT variables.
  // This enumeration is a component of every interface between
  // independent-dependent variables.
  enum LookupMethod {
    Interp, /* Interpolate between the adjacent points, this method is
                 typically used with continuous-variable outputs. */
    Prev,   /* Use the value corresponding to the lower index when the
                 independent variable lies between two calibration points.
                 Note - This uses the lower INDEX, not the lower VALUE.
                        When the data is decreasing, the lower index
                        represents the higher value.
                 Typically used for a discrete variable.*/
    Next,   /* Use the value corresponding to the upper index when the
                 independent variable lies between two calibration points.
                 Note - This uses the lower INDEX, not the lower VALUE.
                        When the data is decreasing, the lower index
                        represents the higher value.
                 Typically used for a discrete variable.*/
    Floor,  /* Use the value corresponding to the index representing the
                 smaller calibrated value when the independent variable
                 lies between two calibration points.
                 Note - This uses the smaller VALUE, not the lower INDEX.
                        When the data is decreasing, the smaller value
                        is represented by higher index.
                 Typically used for a discrete variable.*/
    Ceil,   /* Use the value corresponding to the index representing the
                 larger calibrated value when the independent variable
                 lies between two calibration points.
                 Note - This uses the larger VALUE, not the higher INDEX.
                      When the data is decreasing, the larger value
                      is represented by lower index.
                 Typically used for a discrete variable.*/
    Round   /* Use the value corresponding to the index that has a
                 calibrated value that is arithmetically closest to the
                 independent variable.
                 Typically used for a discrete variable.*/
  };

  enum Continuity {
    Linear, /* Table covers finite region.  Out-of-bounds values evaluate
               to edge data.*/
    WrapAround /* Table wraps infinitely.  Out-of-bounds values wrap back
                   around from the other side.*/
  };


// Behavioral inputs:
  bool perform_full_search; /* (--)
       Flag indicating that a global binary search should be performed every
       time.  This is used when the independent variable is random; when the
       variable is sequential, a sweeping search (default) is more efficient.
       Default: false. */

// Variables with public access for monitoring purposes:
  double fraction; /* (--)
       Represents the location of the data value as a fraction of the
       interval between adjacent calibration points. fraction = 0.0 means at
       the lower index.*/
  bool prox_override; /* (--)
       Instruction to the table manager to use index_prox instead of index
       for cases involving discrete interpretations of this variable.
       Indicates that the variable is in very close proximity to a
       calibrated value, and the index associated with that value should be
       used instead of the regular index.*/
  size_t index_prox; /* (--)
       The index of the data point that is very close to the value of the
       independent variable.  This is only used in conjunction with
       prox_override = true. */

  DoubleVec data;  /* (--)
       The array of calibrated data points for this independent variable */

protected:
  size_t index; /* (--)
       The index of the calibrated data point whose value immediately precedes
       (or is equal to) the value of the independent variable.
       For a data set that is increasing, this is the closest data point that
       is not greater than the independent variable.
       For a data set that is decreasing, this is the closest data point that
       is not less than the independent variable. */

private:
  const double  &variable; /* (--)
       reference to the value used as the independent variable. */
  std::string name;        /* (--) name of this independent variable */
  size_t size; /* (--)
       length of the data table ("array") for this independent variable. */

  bool data_loaded;        /* (--)
       Flag to indicate whether the data is loaded into the data table array.
       Default: False */
  bool initialized; /* (--)
       initialize() method successfully ran.
       Default: False*/
  bool table_values_increasing; /* (--)
       Flag indicating whether the data increases with index.
       Default: True.*/
  bool off_table_front;      /* (--)
       Flag indicating that lookup value is off the front end of data table:
       - if table-values increase, the value of the independent variable is
       less than that of the the first calibration point
       - if table-values decrease, the value of the independent variable is
       greater than that of the the first calibration point.
       Default: False*/
  bool off_table_back;     /* (--)
      flag indicating that the lookup value is off the back end of data table.
      See also off_table_front.
      Default: False. */
  Continuity continuity;   /* (--) determines how the independent variable
      handles values that are out of bounds. */

  double back_value;       /* (--) last value in vector data. */
  double front_value;      /* (--) first value in vector data. */
  double max_value;        /* (--) largest value in vector data; equals
      front_value if values are decreasing, or back_value if values are
      increasing. */
  double min_value;        /* (--) smallest value in vector data; equals
      front_value if values are increasing, or back_value if values are
      decreasing. */
  double delta;            /* (--) difference between maxValue and minValue. */
  double modified_value;   /* (--) value of independent variable after rounding
      or truncating has been applied, if needed */

  const double FRAC_EPS;   /* (--) the gate value to consider fraction as zero. */

public:
  TableIndependentVariable( const double &variable_in,
                            double frac_eps_in=1.0e-9);
  TableIndependentVariable( const std::string &name,
                            const double &variable_in,
                            double frac_eps_in=1.0e-9);
  TableIndependentVariable( const double &variable_in,
                            const Continuity continuity,
                            double frac_eps_in=1.0e-9);
  TableIndependentVariable( const std::string &name,
                            const double &variable_in,
                            const Continuity continuity,
                            double frac_eps_in=1.0e-9);
  virtual ~TableIndependentVariable() = default;

  virtual bool load_data( const double* const data_in,
                          size_t size_in);
  virtual bool load_data( const DoubleVec & data_in);

  bool initialize();

  virtual bool update();

  void bias_data(double bias, size_t idx1, size_t idx2);
  void bias_data(double bias, size_t idx) { bias_data( bias, idx, idx);}
  void bias_data(double bias) {bias_data( bias, 0, data.size()-1);}

  void scale_data(double scale, size_t idx1, size_t idx2);
  void scale_data(double scale, size_t idx) { scale_data( scale, idx, idx);}
  void scale_data(double scale) {scale_data( scale, 0, data.size()-1);}

  void clear_data() {data.clear(); data_loaded = false;}
  void reset_index_front(){index = 0;};
  void reset_index_back(){index = size-1;};

  // Setters and getters:
  void set_name( const std::string &new_name);

  size_t get_size() const {return size;}
  const char * get_name_char() const {return name.c_str();}
  const std::string& get_name() const { return name;}
  bool get_initialized() const { return initialized;}
  size_t get_index() const { return index;}

  bool is_table_increasing() const { return table_values_increasing;}
  bool is_data_loaded() const { return data_loaded;}
  bool is_off_table() {return off_table_back || off_table_front;}
  bool is_off_table_back() {return off_table_back;}
  bool is_off_table_front() {return off_table_front;}
  bool in_back_of(  const double val1, const double val2) const;
  bool in_front_of( const double val1, const double val2) const;

private:
  void binary_search();
  bool determine_modified_value();
  void sweep_up();
  void sweep_down();
  void tag_as_off_table_back();
  void tag_as_off_table_front();
  void generate_fraction();
  bool check_monotonicity( const std::vector<double> & data_in);

  // Disable the copy and assigment operations
  TableIndependentVariable (const TableIndependentVariable&);
  TableIndependentVariable& operator = (const TableIndependentVariable&);
};


// Independent variables are presented as a STL-pair:
//  - the first element is a pointer to the TableIndependentVariable instance
//  - the second element is the interpretation of how the value of the
//    independent variable should be used to extract the value of the
//    dependent variable.
// Because each independent variable may be used differently by different
// dependent variables, this information resides with the dependent variable
// (which this class represents) and not with the TableIndependentVariable
// (which this table accesses).  Default behavior is to interpolate based on
// the neighboring calibrated values, but alternatives such as rounding,
// ceiling, floor, etc. are also available to support discrete variables.
   typedef std::pair<TableIndependentVariable*,
                     TableIndependentVariable::LookupMethod> IndepPair;
   typedef std::vector<IndepPair> IndepPairVec;

#endif
