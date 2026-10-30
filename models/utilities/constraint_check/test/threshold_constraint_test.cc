#include "../include/constraint_enum.hh"
#include "../include/constraint_set.hh"
#include "../include/threshold_constraint.hh"
#include "mocks/cml/cml_message_mock.hh"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Set common configuration options for the ThresholdInstantConstraint tests
void set_threshold_instant_constraint_params(ThresholdInstantConstraint<double, 2>& article) {
    article.violate_on_any_test = true;
    article.tests[0].set_violation_condition(ConstraintEnum::GT);
    article.tests[0].threshold = 0.0;
    article.tests[1].set_violation_condition(ConstraintEnum::GT);
    article.tests[1].threshold = 10.0;
}

// Set common configuration options for the ThresholdTimedConstraint tests
void set_threshold_timed_constraint_params(ThresholdTimedConstraint<double, 2>& article) {
    article.violate_on_any_test = true;
    article.tests[0].set_violation_condition(ConstraintEnum::GT);
    article.tests[0].threshold = 0.0;
    article.tests[0].time_limit = 1.1;
    article.tests[1].set_violation_condition(ConstraintEnum::GT);
    article.tests[1].threshold = 10.0;
    article.tests[1].time_limit = 1.1;
}

// Set common configuration options for the ThresholdTimedConstraintSpecData tests
void set_threshold_timed_constraint_spec_data_params(ThresholdTimedConstraintSpecData<double, 5, 5>& article) {
    article.violate_on_any_test = true;
    article.threshold_spec[0] = 0.0;
    article.threshold_spec[1] = 1.0;
    article.threshold_spec[2] = 2.0;
    article.threshold_spec[3] = 3.0;
    article.threshold_spec[4] = 4.0;
    article.tests[0].set_violation_condition(ConstraintEnum::GT);
    article.tests[0].threshold = 0.0;
    article.tests[0].time_limit = 1.1;
    article.tests[1].set_violation_condition(ConstraintEnum::GT);
    article.tests[1].threshold = 10.0;
    article.tests[1].time_limit = 1.1;
    article.tests[2].set_violation_condition(ConstraintEnum::GT);
    article.tests[2].threshold = 100.0;
    article.tests[2].time_limit = 1.1;
    article.tests[3].set_violation_condition(ConstraintEnum::GT);
    article.tests[3].threshold = 1000.0;
    article.tests[3].time_limit = 1.1;
    article.tests[4].set_violation_condition(ConstraintEnum::GT);
    article.tests[4].threshold = 10000.0;
    article.tests[4].time_limit = 1.1;
}

// Get coverage of the constructor that accepts a ConstraintSet input.
TEST(ThresholdInstantConstraint, ConstructWithConstraintSet) {
    ConstraintSet set;
    const double var {};
    ThresholdInstantConstraint<double, 2> article(var, set);
    EXPECT_EQ(*set.constraints.begin(), &article);
}

// Test a nominal update of the ThresholdInstantConstraint.
TEST(ThresholdInstantConstraint, Update) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    double var = -1.0;
    ThresholdInstantConstraint<double, 2> article(var);
    set_threshold_instant_constraint_params(article);
    article.initialize();
    article.activate();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_TRUE(article.is_active());

    // No violation.
    article.update();
    EXPECT_EQ(article.violation_count, 0);

    // Violation.
    var = 2.0;
    article.update();
    EXPECT_EQ(article.violation_count, 1);
}

// Test that an inactive constraint doesn't report any violations.
TEST(ThresholdInstantConstraint, Inactive) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    double var = -1.0;
    ThresholdInstantConstraint<double, 2> article(var);
    set_threshold_instant_constraint_params(article);
    article.initialize();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_FALSE(article.is_active());

    // This would ordinarily emit a violation since 2 is greater than constraint-test 0's
    // threshold.
    var = 2.0;
    article.update();
    EXPECT_EQ(article.violation_count, 0);
}

// Get coverage of the constructor that accepts a ConstraintSet input.
TEST(ThresholdTimedConstraint, ConstructWithConstraintSet) {
    ConstraintSet set;
    const double var {};
    const double dt = 1.0;
    ThresholdTimedConstraint<double, 2> article(var, dt, set);
    EXPECT_EQ(*set.constraints.begin(), &article);
}

// Test a nominal update of the ThresholdTimedConstraint.
TEST(ThresholdTimedConstraint, Update) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    double var = -1.0;
    const double dt = 1.0;
    ThresholdTimedConstraint<double, 2> article(var, dt);
    set_threshold_timed_constraint_params(article);
    article.initialize();
    article.activate();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_TRUE(article.is_active());

    // No violation.
    article.update();
    EXPECT_EQ(article.violation_count, 0);

    // Violation, but the persistence has not yet been reached.
    var = 2.0;
    article.update();
    EXPECT_EQ(article.violation_count, 0);

    // Violation with necessary persistence.
    article.update();
    EXPECT_EQ(article.violation_count, 1);
}

// Test that an inactive constraint doesn't report any violations.
TEST(ThresholdTimedConstraint, Inactive) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    const double var = 2.0;
    const double dt = 1000000.0;
    ThresholdTimedConstraint<double, 2> article(var, dt);
    set_threshold_timed_constraint_params(article);
    article.initialize();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_FALSE(article.is_active());

    // This would ordinarily emit a violation since 2 is greater than one of the
    // thresholds and the necessary persistence has been reached.
    article.update();
    EXPECT_EQ(article.violation_count, 0);
}

// Get coverage of the constructor that accepts a ConstraintSet input.
TEST(ThresholdTimedConstraintSpecData, ConstructWithConstraintSet) {
    ConstraintSet set;
    const double var {};
    const double dt = 1.0;
    ThresholdTimedConstraintSpecData<double, 5, 5> article(var, dt, set);
    EXPECT_EQ(*set.constraints.begin(), &article);
}

// Test the method which sets the number of specs.
TEST(ThresholdTimedConstraintSpecData, SetNumSpecs) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    double var {};
    const double dt = 1.0;
    ThresholdTimedConstraintSpecData<double, 5, 5> article(var, dt);
    set_threshold_timed_constraint_spec_data_params(article);

    // Nominal call. Article has not yet been initialized.
    EXPECT_FALSE(article.is_initialized());
    article.set_num_specs(2);

    // Invalid call. The number of specs is too large.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("Error in attempting to set the number of specification values")));
    article.set_num_specs(2000);

    // Invalid call. The model has already been initialized.
    article.initialize();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("Error in attempting to set the number of specification values")));
    article.set_num_specs(1);
}

// Test various issues with initialization due to invalid configuration options.
TEST(ThresholdTimedConstraintSpecData, InitializationFailures) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    double var {};
    const double dt = 1.0;
    ThresholdTimedConstraintSpecData<double, 5, 5> article(var, dt);
    set_threshold_timed_constraint_spec_data_params(article);

    // 0 specs.
    article.set_num_specs(0);
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _,
            HasSubstr("Cannot initialize a constraint with number of specifications\nassigned to 0")));
    article.initialize();
    EXPECT_FALSE(article.is_initialized());

    // 1 spec with more than 1 test.
    article.set_num_specs(1);
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _,
            HasSubstr("Cannot create multiple test points (bins) for a constraint defined\nwith a single specification")));
    article.initialize();
    EXPECT_FALSE(article.is_initialized());
}

// Configure the model to populate tests from arrays with an equal number of tests and specs (2).
TEST(ThresholdTimedConstraintSpecData, EqualNumberOfBinsAndSpecs) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    double var {};
    const double dt = 1.0;
    ThresholdTimedConstraintSpecData<double, 5, 5> article(var, dt);
    set_threshold_timed_constraint_spec_data_params(article);
    article.threshold_spec[0] = 10.0;
    article.threshold_spec[1] = 20.0;
    article.threshold_spec[2] = 30.0;
    article.threshold_spec[3] = 40.0;
    article.threshold_spec[4] = 50.0;
    article.time_spec[0] = 1.1;
    article.time_spec[1] = 2.2;
    article.time_spec[2] = 3.3;
    article.time_spec[3] = 4.4;
    article.time_spec[4] = 5.5;
    article.initialize();
    article.activate();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_TRUE(article.is_active());
    EXPECT_EQ(article.tests[0].get_threshold(), article.threshold_spec[0]);
    EXPECT_EQ(article.tests[1].get_threshold(), article.threshold_spec[1]);
    EXPECT_EQ(article.tests[2].get_threshold(), article.threshold_spec[2]);
    EXPECT_EQ(article.tests[3].get_threshold(), article.threshold_spec[3]);
    EXPECT_EQ(article.tests[4].get_threshold(), article.threshold_spec[4]);
    EXPECT_EQ(article.tests[0].get_time_limit(), article.time_spec[0]);
    EXPECT_EQ(article.tests[1].get_time_limit(), article.time_spec[1]);
    EXPECT_EQ(article.tests[2].get_time_limit(), article.time_spec[2]);
    EXPECT_EQ(article.tests[3].get_time_limit(), article.time_spec[3]);
    EXPECT_EQ(article.tests[4].get_time_limit(), article.time_spec[4]);

    // No violation.
    var = -100.0;
    article.update();
    EXPECT_EQ(article.violation_count, 0);

    // Violation, but the persistence has not yet been reached.
    var = 50.0;
    article.update();
    EXPECT_EQ(article.violation_count, 0);

    // Violation with necessary persistence.
    article.update();
    EXPECT_EQ(article.violation_count, 1);
}

// Test the various issues with initialization when interpolating the values.
TEST(ThresholdTimedConstraintSpecData, InterpolationFailures) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Threshold specs are non-monotonic.
    {
        const double var {};
        const double dt = 1.0;
        ThresholdTimedConstraintSpecData<double, 5, 5> article(var, dt);
        set_threshold_timed_constraint_spec_data_params(article);
        article.set_num_specs(4);
        article.threshold_spec[0] = 0.0;
        article.threshold_spec[1] = 1.0;
        article.threshold_spec[2] = 2.0;
        article.threshold_spec[3] = 0.5;

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("specification values are not monotonic")));
        article.initialize();
        EXPECT_FALSE(article.is_initialized());
    }

    // Spec values have inconsistent signs (cross zero) for logarithmic interpolation.
    {
        const double var {};
        const double dt = 1.0;
        ThresholdTimedConstraintSpecData<double, 5, 5> article(var, dt);
        set_threshold_timed_constraint_spec_data_params(article);
        article.use_linear_interpolation = false;
        article.set_num_specs(4);

        // Threshold spec sign differs.
        article.threshold_spec[0] = -4.0;
        article.threshold_spec[1] = -1.0;
        article.threshold_spec[2] = 2.0;
        article.threshold_spec[3] = 10.0;
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _,
                HasSubstr("Logarithmic interpolation requires that the specification values have a consistent sign")));
        article.initialize();
        EXPECT_FALSE(article.is_initialized());

        // Time spec sign differs.
        article.threshold_spec[0] = 1.0;
        article.threshold_spec[1] = 2.0;
        article.threshold_spec[2] = 3.0;
        article.threshold_spec[3] = 4.0;
        article.time_spec[0] = 0.0;
        article.time_spec[1] = 1.0;
        article.time_spec[2] = 2.0;
        article.time_spec[3] = 3.0;
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _,
                HasSubstr("Logarithmic interpolation requires that the specification values have a consistent sign")));
        article.initialize();
        EXPECT_FALSE(article.is_initialized());
    }
}

// Test initialization with linearly interpolating the test values
TEST(ThresholdTimedConstraintSpecData, LinearInterpolation) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    // Floating point comparison tolerance.
    constexpr double tol = 1e-12;

    const double var {};
    const double dt = 1.0;
    ThresholdTimedConstraintSpecData<double, 5, 5> article(var, dt);
    set_threshold_timed_constraint_spec_data_params(article);
    article.use_linear_interpolation = true;

    // 5 tests with 3 specification points.
    //  Threshold      Time limit
    //  ---------      ----------
    //     1.0             1.1
    //      *               *    <----- Interpolated to (2.0, 2.2)
    //      *               *    <----- Interpolated to (3.0, 3.3)
    //      *               *    <----- Interpolated to (4.0, 4.4)
    //     5.0             5.5
    article.set_num_specs(3);
    article.threshold_spec[0] = 1.0;
    article.threshold_spec[1] = 3.0;
    article.threshold_spec[2] = 5.0;
    article.time_spec[0] = 1.1;
    article.time_spec[1] = 3.3;
    article.time_spec[2] = 5.5;
    article.initialize();

    EXPECT_TRUE(article.is_initialized());
    EXPECT_NEAR(article.tests[0].get_threshold(), 1.0, tol);
    EXPECT_NEAR(article.tests[1].get_threshold(), 2.0, tol);
    EXPECT_NEAR(article.tests[2].get_threshold(), 3.0, tol);
    EXPECT_NEAR(article.tests[3].get_threshold(), 4.0, tol);
    EXPECT_NEAR(article.tests[4].get_threshold(), 5.0, tol);
    EXPECT_NEAR(article.tests[0].get_time_limit(), 1.1, tol);
    EXPECT_NEAR(article.tests[1].get_time_limit(), 2.2, tol);
    EXPECT_NEAR(article.tests[2].get_time_limit(), 3.3, tol);
    EXPECT_NEAR(article.tests[3].get_time_limit(), 4.4, tol);
    EXPECT_NEAR(article.tests[4].get_time_limit(), 5.5, tol);
}

// Test initialization with logarithmically interpolating the test values
TEST(ThresholdTimedConstraintSpecData, LogarithmicInterpolation) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    // Floating point comparison tolerance.
    constexpr double tol = 1e-12;

    const double var {};
    const double dt = 1.0;
    ThresholdTimedConstraintSpecData<double, 5, 5> article(var, dt);
    set_threshold_timed_constraint_spec_data_params(article);
    article.use_linear_interpolation = false;

    // 5 tests with 3 specification points.
    //  Threshold      Time limit
    //  ---------      ----------
    //     1.0             1.1
    //      *               *    <----- Interpolated to (2501.0, 4.7146655202367693)
    //      *               *    <----- Interpolated to (5001.0, 5.0921873730037737)
    //      *               *    <----- Interpolated to (7501.0, 5.3269158993552947)
    //   10001.0           5.5
    article.set_num_specs(3);
    article.threshold_spec[0] = 1.0;
    article.threshold_spec[1] = 101.0;
    article.threshold_spec[2] = 10001.0;
    article.time_spec[0] = 1.1;
    article.time_spec[1] = 3.3;
    article.time_spec[2] = 5.5;
    article.initialize();

    EXPECT_TRUE(article.is_initialized());
    EXPECT_NEAR(article.tests[0].get_threshold(), 1.0, tol);
    EXPECT_NEAR(article.tests[1].get_threshold(), 2501.0, tol);
    EXPECT_NEAR(article.tests[2].get_threshold(), 5001.0, tol);
    EXPECT_NEAR(article.tests[3].get_threshold(), 7501.0, tol);
    EXPECT_NEAR(article.tests[4].get_threshold(), 10001.0, tol);
    EXPECT_NEAR(article.tests[0].get_time_limit(), 1.1, tol);
    EXPECT_NEAR(article.tests[1].get_time_limit(), 4.7146655202367693, tol);
    EXPECT_NEAR(article.tests[2].get_time_limit(), 5.0921873730037737, tol);
    EXPECT_NEAR(article.tests[3].get_time_limit(), 5.3269158993552947, tol);
    EXPECT_NEAR(article.tests[4].get_time_limit(), 5.5, tol);
}

}
