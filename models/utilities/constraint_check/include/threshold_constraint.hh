/*******************************************************************************

PURPOSE:
   (Define constraints for checking against sim variables)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/
#ifndef CML_THRESHOLD_CONSTRAINT_HH
#define CML_THRESHOLD_CONSTRAINT_HH

#include <cmath>

#include "constraint.hh"
#include "constraint_test_templates.hh"
#include "constraint_test_timed_templates.hh"
#include "constraint_set.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"


/*****************************************************************************
Name: ThresholdInstantConstraint
Purpose:
  Uses the threshold constraint tests, which are violated when the variable
  has an unfavorable comparison against some threshold value.
  The comparison could be one of several options:
   ==, !=, >, >=, <, <=
  Combining these tests typically results in a constraint violation when
  the variable violates wither all thresholds, or any threshold.

  The tests used in this constraint are set to be marked as violated
  immediately upon the variable violating the specified threshold bounds.

Notes:
Notes:
- There is a lot of similarity between this class and the comparable
  ThresholdTimedConstraint class.
  An inheritance pattern was deliberately avoided here
  to avoid the complication of inheriting from a class-template (the common
  material would have to be templated), and having to expose all possible
  base classes to SWIG.
  It is simpler to duplicate some content.

- There is limited value in having mutiple threshold-violation tests without
  time delays on the individual tests.
  Using ANY logic, whichever constraint-test is least restrictive would
  always trip the constraint; using ALL logic, whichever constraint-test is
  most restrictive would always trip the constraint. In either case, only 1
  constraint-test is actually useful. Nevertheless, the overhead of
  executing with the possibility of having multiple tests is negligible and
  simplifies the overall design.
*****************************************************************************/
template<typename T, size_t NumThresholds>
class ThresholdInstantConstraint : public Constraint
{
 protected:
  const T & variable; /* (--)
    The variable being monitored for violation of the
    constraints. */
 public:
  ConstraintTest_Threshold<T> tests[NumThresholds]; /* (--)
    The single Constraint-test for this constraint.
    Note -- cannot use a std::array here because Trick's checkpoint-restart
    prevents use of STL-arrays of templated instances.*/


  /*******************************************************************
  Constructor / Destructor
  ********************************************************************/
  ThresholdInstantConstraint (const T & var)
    :
    Constraint(NumThresholds),
    variable(var),
    tests()
  {
    for (auto & test : tests) {
      test_list.push_back(&test);
    }
  }
  ThresholdInstantConstraint (const T & var,
                              ConstraintSet & set)
    :
    ThresholdInstantConstraint (var)
  {
    set.constraints.push_back(this);
  }
  virtual ~ThresholdInstantConstraint(){};

  /***************************************************************************
  * Inherited -  initialize
  ***************************************************************************/

  /***************************************************************************
  * Name: update
  * Purpose: Main executable of this constraint.
  ***************************************************************************/
  void update()
  {
    if (!active) {return;}

    for (auto & test : tests) {
      test.test_violation(variable);
    }
    post_update(); // inherited.
  }

 private: // not implemented; not copyable
  ThresholdInstantConstraint( const ThresholdInstantConstraint &);
  ThresholdInstantConstraint & operator=( const ThresholdInstantConstraint &);
};




/*****************************************************************************
Name: ThresholdTimedConstraint
Purpose:
  Uses the threshold constraint tests, which are violated when the variable
  has an unfavorable comparison against some threshold value.
  The comparison could be one of several options:
   ==, !=, >, >=, <, <=
  Combining these tests typically results in a constraint violation when
  the variable violates wither all thresholds, or any threshold.

  The tests used in this constraint are set to be marked as violated
  when the variable persistently violates the specified threshold for
  an assigned period of time.

Example:
  violation if:
    x > 1 for 3 seconds, OR
    x > 2 for 2 second,  OR
    x > 3 for 1 second

Notes:
- There is a lot of similarity between this class and the comparable
  ThresholdInstantConstraint class.
  An inheritance pattern was deliberately avoided here
  to avoid the complication of inheriting from a class-template (the common
  material would have to be templated), and having to expose all possible
  base classes to SWIG.
  It is simpler to duplicate some content.
*****************************************************************************/
template<typename T, size_t NumThresholds>
class ThresholdTimedConstraint : public Constraint
{
 protected:
  const T & variable; /* (--)
    The variable being monitored for violation of the
    constraints. */
  const double & delta_time; /* (s)
    Reference to the time-elapsed between calls to update this constraint.*/
 public:
  ConstraintTest_ThresholdTimed<T> tests[NumThresholds]; /* (--)
    The single Constraint-test for this constraint.
    Note -- cannot use a std::array here because Trick's checkpoint-restart
    prevents use of STL-arrays of templated instances.*/


  /*******************************************************************
  Constructor / Destructor
  ********************************************************************/
  ThresholdTimedConstraint (const T & variable_,
                            const double & delta_time_)
    :
    Constraint(NumThresholds),
    variable(variable_),
    delta_time(delta_time_),
    tests()
  {
    for (auto & test : tests) {
      test_list.push_back(&test);
    }
  }
  /********************************************************************/
  ThresholdTimedConstraint (const T & variable,
                            const double & delta_time,
                            ConstraintSet & set)
    :
    ThresholdTimedConstraint (variable,
                              delta_time)
  {
    set.constraints.push_back(this);
  }
  virtual ~ThresholdTimedConstraint(){};

  /***************************************************************************
  * Inherited -  initialize
  ***************************************************************************/

  /***************************************************************************
  * Name: update
  * Purpose: Main executable of this constraint.
  ***************************************************************************/
  void update()
  {
    if (!active) {return;}

    for (auto & test : tests) {
      test.test_violation(variable,
                          delta_time);
    }
    post_update(); // inherited.
  }

 private: // not implemented; not copyable
  ThresholdTimedConstraint( const ThresholdTimedConstraint &);
  ThresholdTimedConstraint & operator=( const ThresholdTimedConstraint &);
};



/*****************************************************************************
Name: ThresholdTimedConstraintSpecData
Purpose:
  An extension of ThresholdTimedConstraint, with the addition of
  arrays for specifying the data and the capability to interpolate between
  the specified data values to generate tests at interim data points.
  The ThresholdTimedConstraint class only supports direct population of the
  values in its tests array.
Notes:
- There is a lot of similarity between this class and the comparable
  ThresholdTimedConstraint class.
  An inheritance pattern was deliberately avoided here
  to avoid the complication of inheriting from a class-template (the common
  material would have to be templated), and having to expose all possible
  base classes to SWIG.
  It is simpler to duplicate some content.
*****************************************************************************/
template<typename T, size_t NumSpecs, size_t NumTests>
class ThresholdTimedConstraintSpecData : public Constraint
{
 protected:
  const T & variable; /* (--)
    The variable being monitored for violation of the
    constraints. */
  const double & delta_time; /* (s)
    Reference to the time-elapsed between calls to update this constraint.*/
  const size_t num_specs_available; /* (--)
    Number of specifications available, as set at construction.*/
  size_t num_specs; /* (--)
    Number of specifications to be used.
    Note -- the separation of num_specs from num_specs_available supports
    the use of a subset of the available specifications for generating the
    testing parameters.
    num_specs and num_specs_available are initialized to the same value and
    num_specs can be modified via set_num_specs prior to initialization.*/
 public:
  ConstraintTest_ThresholdTimed<T> tests[NumTests]; /* (--)
    The set of Constraint-tests for this constraint.
    Each test has a threshold value and a duration that, if the
    reference variable crosses the threshold for longer than
    duration, results in a violation.
    Note -- cannot use a std::array here because Trick's checkpoint-restart
    prevents use of STL-arrays of templated instances.*/


  // Configurations:
  bool recover_quiet_violation; /* (--)
    Flag that is copied to each test if populate_tests_from_arrays to
    indicate that quiet test-violations can be reset.*/
  bool populate_tests_from_arrays; /* (--)
    Flag indicating whether the test values should be populated directly by
    the user or indirectly by populating arrays and copying those data to the
    test data. If this option is set to false, the class becomes functionally
    equivalent to ThresholdTimedConstraintS.
    Default: true */
  T threshold_spec[NumSpecs]; /* (--)
    Set of specified values for the thresholds of tests.
    If populate_tests_from_arrays, these values are used to populate the
    threshold values in the constraint-tests.
    If (NumSpecs == NumTests), these threshold_spec values are copied
    directly; otherwise, they will be interpolated to generate the threshold
    values covering the same domain but in a different number of steps. */
  double time_spec[NumSpecs];  /* (s)
    Set of specified values for the associated duration before a
    numerical violation of the threshold becomes an actual violation of the
    constraint-test.
    If populate_tests_from_arrays, these values are used to populate the
    time_limit values in the constraint-tests.
    If (NumSpecs == NumTests), these time_spec values are copied
    directly; Otherwise, they will be interpolated to generate the time_limit
    values covering the same range but in a different number of steps. */
  bool use_linear_interpolation; /* (--)
    Flag used only when interpolating time_limit values from time_spec
    values. Flag indicates whether the interpolation should be linear or
    logarithmic.
    Default: true (linear).*/

  // Outputs:
  T test_violation_value; /* (--)
    The threshold value of (one of) the test that resulted in a violation.
    If multiple tests result in a violation, the value recorded here is that
    of the lowest indexed test.*/

  /*******************************************************************
  Constructor / Destructor
  ********************************************************************/
  ThresholdTimedConstraintSpecData (const T & variable_,
                                    const double & delta_time_)
    :
    Constraint(NumTests),
    variable(variable_),
    delta_time(delta_time_),
    num_specs_available( NumSpecs),
    num_specs( NumSpecs),
    recover_quiet_violation(false),
    populate_tests_from_arrays(true),
    threshold_spec(),
    time_spec(),
    use_linear_interpolation(true),
    test_violation_value()
  {
    for (auto & test : tests) {
      test_list.push_back(&test);
    }
  }
  ThresholdTimedConstraintSpecData (const T & variable,
                                    const double & delta_time,
                                    ConstraintSet & set)
    :
    ThresholdTimedConstraintSpecData (variable,
                                      delta_time)
  {
    set.constraints.push_back(this);
  }
  virtual ~ThresholdTimedConstraintSpecData(){};

  /***************************************************************************
  Name: set_num_specs
  Purpose:
    Sets the number of specification values.
  ***************************************************************************/
  void set_num_specs( size_t val)
  {
    if ( !initialized &&
         val <= num_specs_available) {
      num_specs = val;
    }
    else {
      CMLMessage::error( __FILE__,__LINE__,
        "Error in attempting to set the number of specification values\n"
        "to use in generating the testing parameters.\n"
        "There are 2 requirements:\n"
        "- Number of specifications (",val,") must be not more than the\n"
        "  maximum number specified at construction (",num_specs_available,")\n"
        "- Constraint must not yet be initialized (initialize-status = ",
        initialized,")\n"
        "Number of specifications remains at ",num_specs,"\n");
    }
  }

  /***************************************************************************
  * Name: initialize
  * Purpose: Sanity-checks
  *          Generates the interpolated test points based on the provided
  *          specification values.
  ***************************************************************************/
  void initialize()
  {
    if (num_specs == 0 || num_tests == 0) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot initialize a constraint with number of specifications\n"
        "or number of bins assigned to 0.\nInitialization failed.\n");
      return;
    }
    if (num_specs == 1 && num_tests > 1) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot create multiple test points (bins) for a constraint defined\n"
        "with a single specification. Re-configure to require only a single\n"
        "test point, or provide at least one more specification value to\n"
        "support interpolation.\nInitialization failed.\n");
      return;
    }
    if (num_tests == 1 && num_specs > 1) {
      CMLMessage::warn( __FILE__,__LINE__,
        "A constraint was configured to use a single test point\n"
        "(num_tests = 1) but with mutiple specification points.\n"
        "Only the first specification point will be used as the single "
        "test point.\n");
    }


    if (populate_tests_from_arrays) {
      /* if num-bins and num-specs are equal, just copy the spec values to
       * the tests.*/
      if (num_tests == num_specs) {
        for (size_t ii = 0; ii < num_specs; ++ii) {
          tests[ii].threshold = threshold_spec[ii];
          tests[ii].time_limit = time_spec[ii];
        }
      }
      /* otherwise, interpolate. This is involved so gets its own method.
       * If interpolation fails, initialization fails.*/
      else if (!interpolate_values()) {return;}

      for (ConstraintTest_ThresholdTimed<T> & test : tests) {
        test.recover_quiet_violation = recover_quiet_violation;
      }
    }
    // else no action -- use the values as populated directly in the tests.

    // Continue with the standard initialization.
    Constraint::initialize();
  }

  /***************************************************************************
  * Name: interpolate_values
  * Purpose:
  *  Used when the test configuration values are to be populated from the
  *  input data arrays in this class, and the size of the input arrays is
  *  not the same as the number of tests. The test values will be interpolated
  *  from the input values.
  ***************************************************************************/
  bool interpolate_values()
  {
    /* We match the first set of spec values to the first set of test-values,
     * so assign those before any logic-branching occurs. */
    tests[0].threshold  = threshold_spec[0];
    tests[0].time_limit = time_spec[0];

    /* If there is only 1 constraint-test (i.e. num_tests=1),
     * the one assignment is sufficient. */
    if (num_tests == 1) { return true;}
    /* If there are only 2 constraint-tests, we use the last spec value for
     * the second test and are done without interpolating.*/
    if (num_tests == 2) {
      tests[1].threshold  = threshold_spec[num_specs-1];
      tests[1].time_limit = time_spec[num_specs-1];
      return true;
    }

    /* Otherwise (num_tests > 2):
     *    Aside: therefore num_specs > 1; if num_specs==0 or num_specs==1
                 and num_tests>1, the initialize() method would have already
                 failed out.)
     * we will interpolate between the spec values to get the test values.
     * First, if there are more than 2 spec-threshold values (num_specs>2),
     * check the monotonicity of the spec-threshold values;
     * we cannot interpolate properly without monotonicity.
     * NOTES
     * - fail on non-unique values also, it is later assumed that the
     *   values are unique.
     * - use indices 0 and 1 to specify whether the data is increasing or
     *   decreasing and then check all other adjacent specified values. Check
     *   0->1 again to confirm they are not the same value */
    int parameter_direction = (threshold_spec[1] > threshold_spec[0])? 1 : -1;
    for (size_t ii = 1; ii < num_specs; ++ii) {
      if (parameter_direction * (threshold_spec[ii] - threshold_spec[ii-1])
                                                                     <= 0.0) {
        CMLMessage::error( __FILE__,__LINE__,
          "Initialization failed.\n"
          "Cannot reliably interpolate between specification points when the\n"
          "specification values are not monotonic.\n"
          "Unable to create the test point values.\n");
        return false; // initialization failed
      }
    }


    // Copy the last spec-data set to the last test-data set.
    tests[num_tests-1].threshold  = threshold_spec[num_specs-1];
    tests[num_tests-1].time_limit = time_spec[num_specs-1];

    /* Make the test threshold values equally spaced between the
     * first and last spec threshold values.
     * Note -- using double datatype here to avoid problems with integer
     *         arithmetic if the constraint variable is an integer datatype.*/
    double threshold_resolution =
         (threshold_spec[num_specs-1] - threshold_spec[0])*1.0 / (num_tests-1);
    for (size_t ii = 1; ii <= num_tests-2; ++ii) {
      tests[ii].threshold = threshold_spec[0] + ii * threshold_resolution;
    }

    /* Set the test threshold values for the time.
     * Compute the test time_limit values as interpolations from the spec time
     * values. These will be interpolated using either linear or logarithmic
     * interpolation.
     * In both cases, we need to generate an index and fraction
     * value. The indexing is the same for both interpolation methods, but the
     * fraction value is method-specific. Generate the index first.

     *The index value is the largest index within the threshold_spec
     * array with a value that is closer to threshold_spec[0] than is the
     * generated threshold value for the test. Because the data-set has already
     * been tested to be monotonic, that means the threshold-spec values can
     * be compared directly with the test-threshold value to find the last
     * threshold-spec value that is smaller than the test-threshold value (if
     * monitonically increasing), or the last one that is larger than the
     * test-threshold value (if monitonically decreasing). This dichotomoy
     * between increasing and decreasing data sets can be most easily
     * accommodated by multiplying the values by parameter_direction; then if
     * the data set is decreasing, its negative values are increasing.

     * We have three indices, so this can get confusing:
     * - the test-index [0,num_tests-1] identifies which test is being
     *   addressed.
     * - the spec-index [0,num_specs-1] identifies which of the values from
     *   the spec arrays is being accessed.
     * - the mapped-index [0,num_specs-1] is an array that identifies the
     *   spec-index associated with each of the test-indices.

     * Notes:
        - The first test (at index 0) will have spec-index 0 and fraction 0;
        - The last test (at index num_tests-1) will have spec-index
          (num_specs-1) and fraction 0 by definition.
        - The first and last test values have already been assigned.
        - The highest value of any other mapped-index is num_specs-2.*/
    size_t mapped_ix[num_tests-1];
    mapped_ix[0] = 0; // unused spacer, we already have values for test-0.
    size_t spec_ix=0;
    for (size_t test_ix = 1; test_ix <= num_tests-2; ++test_ix) {
      while( (spec_ix < num_specs-2) &&
             ((parameter_direction * tests[test_ix].threshold) >
              (parameter_direction * threshold_spec[spec_ix+1]))) {
        ++spec_ix;
      }
      mapped_ix[test_ix] = spec_ix;
    }


    // The fraction -- and the way it is used -- differs between algorithms.
    double frac = 0.0; // fraction of interval between spec values

    if (use_linear_interpolation) {
      /* t = t_0 + f * (t_1 - t_0)
         with f = (x - x_0) / (x_1 - x_0)
         where:
         - t is time,
         - x is threshold,
         - subscript 0 references the spec value at the mapped-index,
         - subscript 1 references the next spec value after the mapped-index,
         - the non-subscripted values are the test values.*/
      for (size_t test_ix = 1; test_ix <= num_tests-2; ++test_ix) {
        size_t map_ix = mapped_ix[test_ix];
        frac = (tests[test_ix].threshold - threshold_spec[map_ix]) /
            (threshold_spec[map_ix+1] -  threshold_spec[map_ix]);
        tests[test_ix].time_limit = time_spec[map_ix] +
          frac * (time_spec[map_ix+1] -  time_spec[map_ix]);
      }

    } else { // Use logarithmic interpolation.
      /* We take the log of the values of threshold_spec and the log
       * of the values of time_spec, perform a linear interpolation on these
       * log-values to give an interpolated log-time, then invert to give an
       * interpolated time.
       *   lg(t) = lg(t_0) + f * (lg(t_1) - lg(t_0))
       *         = lg(t_0) + f * lg( t_1 / t_0)
       *   =>  t = t_0 * (t_1 / t_0)^f
       * where:
       *     f = (lg(x) - lg(x_0)) / (lg(x_1) - lg(x_0))
       *       = lg(x / x_0) / lg(x_1 / x_0)
       *

       * Logarithmic interpolation will only work when the spec data has a
       * consistent sign. Otherwise it will fail on the interpolation
       * interval in which the sign changes because we cannot take the
       * logarithm of a non-positive value.
       * Note: if values are consistently negative, the algorithm will still
       *       work because it is based on logs of ratios of numbers
       *       (lg(x) - lg(y) = lg(x/y))*/
      if ( (threshold_spec[0] * threshold_spec[num_specs-1]) <= 0.0 ||
           (time_spec[0]  * time_spec[num_specs-1])  <= 0.0) {
        CMLMessage::error( __FILE__,__LINE__,
          "Initialization failed.\n"
          "Constraint is configured to generate the values of the test\n"
          "points base don logarithmic interpolation of the specification "
          "values.\nLogarithmic interpolation requires that the "
          "specification values have a consistent sign;\n"
          "the spec values cannot cross or equal zero.\n");
        return false; // initialization fails.
      }
      /* Compute the log of threshold-spec value ratios up front to avoid
       * redundant repetition (typically, we are adding more tests than
       * specified, so the same spec-ix will be used multiple times).
       *      lg_ratio[i] = lg (threshold[i+1] / threshold[i]) */
      double log_threshold_ratios[num_specs-1];
      for (spec_ix = 0; spec_ix <= num_specs-2; ++spec_ix) {
        log_threshold_ratios[spec_ix] = std::log10( threshold_spec[spec_ix+1] /
                                                    threshold_spec[spec_ix]);
      }
      for (size_t test_ix = 1; test_ix <= num_tests-2; ++test_ix) {
        size_t map_ix = mapped_ix[test_ix];
        // Note -- div-0 protected: log_param_ratios=0 would require two
        // adjacent entries in threshold_spec to be equal. That is protected
        // against by ensuring that threshold_spec must be monotonic.
        frac = std::log10( tests[test_ix].threshold / threshold_spec[map_ix]) /
                    log_threshold_ratios[map_ix];
        tests[test_ix].time_limit = time_spec[map_ix] *
                                  std::pow( time_spec[map_ix+1] / time_spec[map_ix],
                                            frac);
      }
    }
    return true;
  }

  /***************************************************************************
  * Name: update
  * Purpose: Main executable of this constraint.
  ***************************************************************************/
  void update()
  {
    if (!active) {return;}

    for (auto & test : tests) {
      test.test_violation(variable,
                          delta_time);
    }
    post_update(); // inherited.
  }

 private: // not implemented; not copyable
  ThresholdTimedConstraintSpecData( const ThresholdTimedConstraintSpecData &);
  ThresholdTimedConstraintSpecData & operator=(
                                     const ThresholdTimedConstraintSpecData &);
};
#endif
