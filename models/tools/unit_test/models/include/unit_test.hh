/*********************************** TRICK HEADER **************************
PURPOSE:    (To provide a Trick-friendly unit-test framework)
REFERENCES: (../models-C by Jason Arnold)

LIBRARY DEPENDENCY: (
   (../src/unit_test.cc)
   )

PROGRAMMERS:
  (((Jason Arnold) (Titan) (Jul 2005))
   ((Gary Turner) (OSR) (Oct 2016)
    (transition from char*, malloc, free, sprintf, etc. to use STL strings,
    vectors, and lists))
   ((Bingquan Wang) (OSR) (Jan 2018) (Migrated to FSL))
  )
***************************************************************************/
#ifndef CML_UNIT_TEST_FRAMEWORK_HH
#define CML_UNIT_TEST_FRAMEWORK_HH

#include <list>
#include <vector>
#include <string>

#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
SweepSet
Purpose:(Collection of data necessary for sweeping through a data set)
*****************************************************************************/
class SweepSet {
 protected:
  double & variable; /* (--) reference to the variable being set. */
  double start;      /* (--) start value of the sweep.  */
  double end;        /* (--) end value of the sweep (inclusive). */
  double increment;  /* (--) variable increment on each data point. */
  double value;      /* (--) the value to be assigned to "variable" */
  double epsilon;    /* (--)
      the threshold for determining whether the sweep has reached "end". */
 public:
  SweepSet( double & variable,
            double start,
            double end,
            double incr);
  SweepSet(const SweepSet &other);
  SweepSet& operator = (const SweepSet &other);

  virtual ~SweepSet() {};
  void initialize() {variable = start;};
  bool increment_sweep();
};

/*****************************************************************************
UnitTestFramework
Purpose:(The main unit-test framework)
*****************************************************************************/
class UnitTestFramework {

 public:
  bool enabled; /* (--)
       allows selective engagement of multiple frameworks within a sim. */
  bool cycle_data; /* (--)
       cycle the data set when it reaches the end. */
  std::string vars_file_name ; /* (--)
     filename of file containing list of variable names for data */
  std::string data_file_name ; /* (--)
     filename of file containing white-space delimited data */
  std::string linked_vars_file_name; /* (--)
     filename of file containing two elements per row: a variable name and
     its associated data-file-name. */

 protected:
  bool initialized;  /* (--) model has been initialized. */

  bool using_sweeps; /* (--)
         model is using data sweeps (as opposed to a file containing the
         specific data values). */
  bool first_sweep; /* (--) boolean flag for special treatment on the
         first sweep. */
  bool sweeps_complete; /* (--) all variable sweeps are complete. */
  std::list<SweepSet> sweeps; /* (--) list of variables being swept. */

  std::vector<std::string> variables; /* (--) list of variables by name.*/
  std::list<std::string> commands; /* (--) list of parsed commands */
  std::list<std::string> titles; /* (--)
      optional list of titles; titles get printed through cout to give a
      status of where the progress is.  Typically not used. */
  struct LinkedVars {
    std::string variable_name; /* (--) name of the variable. */
    std::string filename;      /* (--) name of the file with associated data*/
  };
  std::list<LinkedVars> linked_variables; /* (--)
      list of variables with their associated filenames.*/

  bool warning_on_sim_end; /* (--)
      flag to indicate whether to drop a warning if the sim overruns the
      available data.  defaults to true.  Set to false once the warning
      has been sent. */
  unsigned int cycle_overruns; /* (--)
      Number of sim-cycles after the data has expired; used to provide
      a buffer before dropping the warning. */

 public:
  unsigned int cycle_overruns_limit; /* (--)
      Drop the warning when cycle_overruns exceeds this value. */

  UnitTestFramework();
  virtual ~UnitTestFramework(){};

  void add_sweep( double & variable,
                  double start,
                  double end,
                  double incr);
  void initialize();
  void update();
 protected:
  void configure_sweeps();
  void configure_file_combinations();
  void configure_from_definition_file();
  void populate_linked_variables();
  void process_linked_variables();
  void populate_variable_names();

  void update_sweeps();
  void update_file();
 private:
  friend class UnitTestFrameworkTest;
  UnitTestFramework (const UnitTestFramework&);
  UnitTestFramework & operator = (const UnitTestFramework&);
  std::string expand_env_variables(const std::string& input);
};

// Interface to make this look like the C-style unit-test framework:
class UNIT_TEST : public UnitTestFramework {};

inline void unit_test_init( UNIT_TEST * data) {data->initialize();};
inline void unit_test( UNIT_TEST * data) {data->update();};
inline void unit_test_dd( UNIT_TEST * data) {(void) data;};
inline void unit_test_shutdown( UNIT_TEST * data) {(void) data;};
#endif
