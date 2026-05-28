/*******************************************************************************

PURPOSE:
   (Define a set of constraints for testing purposes)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
    (New))
 )
*******************************************************************************/


#include "../../include/constraint_set.hh"
#include "../../include/threshold_constraint.hh"
#include "../../include/interval_constraint.hh"

// This dummy struct is required to force Trick ICG to generate attributes for
// template members of template types when we Trickify CML. We don't actually
// use this struct. If you run into similar issues in your sim, you'll need to
// duplicate this for the types that you're using with the Constraint Tests.
struct TestConstraintTypes
{
  ConstraintTest_ThresholdTimed<double> constraint_test_threshold_timed_double;
  ConstraintTest_ThresholdTimed<int>    constraint_test_threshold_timed_int;
  ConstraintTest_Threshold<double>      constraint_test_threshold_double;
  ConstraintTest_Threshold<int>         constraint_test_threshold_int;
  ConstraintTest_IntervalTimed<double>  constraint_test_interval_timed_double;
  ConstraintTest_IntervalTimed<int>     constraint_test_interval_timed_int;
  ConstraintTest_Interval<double>       constraint_test_interval_double;
};

class TestConstraintSet
{
 public:
  ConstraintSet                  set;
  ThresholdTimedConstraint<double, 4>             threshold_x4t;
  ThresholdInstantConstraint<double, 4>           threshold_x4i;

  ThresholdTimedConstraint<int, 4>                threshold_i4t;
  ThresholdInstantConstraint<int, 4>              threshold_i4i;

  ThresholdTimedConstraintSpecData<double, 4, 4>  threshold_x4s;
  ThresholdTimedConstraintSpecData<double, 4, 3>  threshold_x3s;
  ThresholdTimedConstraintSpecData<double, 4, 3>  threshold_x3s_neg;
  ThresholdTimedConstraintSpecData<double, 4, 5>  threshold_x5s;

  IntervalTimedConstraint<double, 2>              interval_x2t;
  IntervalTimedConstraint<double, 2>              interval_x2t_neg;
  IntervalTimedConstraint<int, 2>                 interval_i2t;
  IntervalInstantConstraint<double, 2>            interval_x2i;

  TestConstraintSet( const double& delta_time,
                     const double& var_x,
                     const double& var_x_neg,
                     const int&    var_i)
    :
    set(),
    threshold_x4t(     var_x,     delta_time,  set),
    threshold_x4i(     var_x,     set),
    threshold_i4t(     var_i,     delta_time,  set),
    threshold_i4i(     var_i,     set),
    threshold_x4s(     var_x,     delta_time,  set),
    threshold_x3s(     var_x,     delta_time,  set),
    threshold_x3s_neg( var_x_neg, delta_time,  set),
    threshold_x5s(     var_x,     delta_time,  set),
    interval_x2t(      var_x,     delta_time,  set),
    interval_x2t_neg(  var_x_neg, delta_time,  set),
    interval_i2t(      var_i,     delta_time,  set),
    interval_x2i(      var_x,     set)

  {
    threshold_x4t.tests[0].threshold = 10.0;
    threshold_x4t.tests[1].threshold = 20.0;
    threshold_x4t.tests[2].threshold = 30.0;
    threshold_x4t.tests[3].threshold = 35.0;
    threshold_x4t.tests[0].time_limit = 1.7;
    threshold_x4t.tests[1].time_limit = 1.0;
    threshold_x4t.tests[2].time_limit = 0.8;
    threshold_x4t.tests[3].time_limit = 0.5;
    threshold_x4t.violation_condition = ConstraintEnum::GE;

    threshold_x4i.tests[0].threshold = 10.0;
    threshold_x4i.tests[1].threshold = 20.0;
    threshold_x4i.tests[2].threshold = 30.0;
    threshold_x4i.tests[3].threshold = 35.0;
    threshold_x4i.violation_condition = ConstraintEnum::GT;

    threshold_i4t.tests[0].threshold = 10;
    threshold_i4t.tests[1].threshold = 20;
    threshold_i4t.tests[2].threshold = 30;
    threshold_i4t.tests[3].threshold = 35;
    threshold_i4t.tests[0].time_limit = 0.2;
    threshold_i4t.tests[1].time_limit = 1.0;
    threshold_i4t.tests[2].time_limit = 1.8;
    threshold_i4t.tests[3].time_limit = 2.5;
    threshold_i4t.violation_condition = ConstraintEnum::LT;

    threshold_i4i.tests[0].threshold = 10;
    threshold_i4i.tests[1].threshold = 20;
    threshold_i4i.tests[2].threshold = 30;
    threshold_i4i.tests[3].threshold = 40;
    threshold_i4i.violation_condition = ConstraintEnum::EQ;

    threshold_x4s.threshold_spec[0] = 10.0;
    threshold_x4s.threshold_spec[1] = 20.0;
    threshold_x4s.threshold_spec[2] = 30.0;
    threshold_x4s.threshold_spec[3] = 40.0;
    threshold_x4s.time_spec[0] = 0.5;
    threshold_x4s.time_spec[1] = 1.0;
    threshold_x4s.time_spec[2] = 1.8;
    threshold_x4s.time_spec[3] = 2.5;
    threshold_x4s.violation_condition = ConstraintEnum::LE;

    threshold_x3s.threshold_spec[0] = 10.0;
    threshold_x3s.threshold_spec[1] = 20.0;
    threshold_x3s.threshold_spec[2] = 30.0;
    threshold_x3s.threshold_spec[3] = 40.0;
    threshold_x3s.time_spec[0] = 0.5;
    threshold_x3s.time_spec[1] = 1.0;
    threshold_x3s.time_spec[2] = 1.8;
    threshold_x3s.time_spec[3] = 1.5;
    threshold_x3s.violation_condition = ConstraintEnum::LT;

    threshold_x3s_neg.threshold_spec[0] = -10.0;
    threshold_x3s_neg.threshold_spec[1] = -20.0;
    threshold_x3s_neg.threshold_spec[2] = -30.0;
    threshold_x3s_neg.threshold_spec[3] = -40.0;
    threshold_x3s_neg.time_spec[0] = 0.5;
    threshold_x3s_neg.time_spec[1] = 1.0;
    threshold_x3s_neg.time_spec[2] = 1.8;
    threshold_x3s_neg.time_spec[3] = 1.5;
    threshold_x3s_neg.violation_condition = ConstraintEnum::GT;

    threshold_x5s.threshold_spec[0] = 10.0;
    threshold_x5s.threshold_spec[1] = 20.0;
    threshold_x5s.threshold_spec[2] = 30.0;
    threshold_x5s.threshold_spec[3] = 40.0;
    threshold_x5s.time_spec[0] = 3.7;
    threshold_x5s.time_spec[1] = 3.0;
    threshold_x5s.time_spec[2] = 0.3;
    threshold_x5s.time_spec[3] = 0.1;
    threshold_x5s.violation_condition = ConstraintEnum::GT;

    interval_x2t.tests[0].lower_bound = 10.0;
    interval_x2t.tests[0].upper_bound = 20.0;
    interval_x2t.tests[1].lower_bound = 30.0;
    interval_x2t.tests[1].upper_bound = 40.0;
    interval_x2t.tests[0].time_limit = 0.5;
    interval_x2t.tests[1].time_limit = 0.8;
    interval_x2t.violation_condition = ConstraintEnum::In;

    interval_x2t_neg.tests[0].lower_bound = -20.0;
    interval_x2t_neg.tests[0].upper_bound = -10.0;
    interval_x2t_neg.tests[1].lower_bound = -40.0;
    interval_x2t_neg.tests[1].upper_bound = -30.0;
    interval_x2t_neg.tests[0].time_limit = 0.5;
    interval_x2t_neg.tests[1].time_limit = 0.8;
    interval_x2t_neg.violation_condition = ConstraintEnum::In;

    interval_i2t.tests[0].lower_bound = 10;
    interval_i2t.tests[0].upper_bound = 20;
    interval_i2t.tests[1].lower_bound = 30;
    interval_i2t.tests[1].upper_bound = 40;
    interval_i2t.tests[0].time_limit = 0.5;
    interval_i2t.tests[1].time_limit = 0.8;
    interval_i2t.violation_condition = ConstraintEnum::In;

    interval_x2i.tests[0].lower_bound = 10.0;
    interval_x2i.tests[0].upper_bound = 20.0;
    interval_x2i.tests[1].lower_bound = 30.0;
    interval_x2i.tests[1].upper_bound = 40.0;
    interval_x2i.violation_condition = ConstraintEnum::In;
  }

  void run_4_config()
  {
    threshold_x4t.tests[0].use_timer = false;
  }

 private: // not implemented; not copyable
  TestConstraintSet( const TestConstraintSet&);
  TestConstraintSet & operator=( const TestConstraintSet&);
};
