#include "../include/constraint_test_timed_templates.hh"
#include "mocks/cml/cml_message_mock.hh"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Test that no violation is reported if the model is inactive.
TEST(ConstraintTestThresholdTimed, Inactive) {
    ConstraintTest_ThresholdTimed<int> article;
    article.enabled = false;
    article.recover_quiet_violation = true;
    article.threshold = 1;
    article.set_violation_condition(ConstraintEnum::EQ);

    // Initialization should proceed even when disabled.
    article.initialize();
    EXPECT_TRUE(article.is_initialized());

    // Since the model is disabled, it should not be active.
    article.activate();
    EXPECT_FALSE(article.is_active());

    // Since the model is inactive, a value which would ordinarily result in a violation
    // will not be reported as such.
    EXPECT_FALSE(article.test_violation(10000, 1.0));
    EXPECT_FALSE(article.get_violation());
}

// Test the violation determination logic for timed threshold constraints with the
// timing feature disabled.
TEST(ConstraintTestThresholdTimed, TestViolationNoTimer) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_ThresholdTimed<int> int_test;
    ConstraintTest_ThresholdTimed<double> double_test;
    ConstraintTest_ThresholdTimed<float> float_test;

    // Initialize all test articles.
    int_test.threshold = 3;
    int_test.set_violation_condition(ConstraintEnum::EQ);
    int_test.use_timer = false;
    double_test.threshold = 3.0;
    double_test.set_violation_condition(ConstraintEnum::EQ);
    double_test.use_timer = false;
    float_test.threshold = 3.0f;
    float_test.set_violation_condition(ConstraintEnum::EQ);
    float_test.use_timer = false;

    EXPECT_EQ(int_test.get_threshold(), 3);
    EXPECT_EQ(double_test.get_threshold(), 3.0);
    EXPECT_EQ(float_test.get_threshold(), 3.0f);

    int_test.initialize();
    double_test.initialize();
    float_test.initialize();
    int_test.activate();
    double_test.activate();
    float_test.activate();

    // Equality
    int_test.set_violation_condition(ConstraintEnum::EQ);
    double_test.set_violation_condition(ConstraintEnum::EQ);
    float_test.set_violation_condition(ConstraintEnum::EQ);
    EXPECT_TRUE(int_test.test_violation(3, 1.0));
    EXPECT_TRUE(double_test.test_violation(3.0, 1.0));
    EXPECT_TRUE(float_test.test_violation(3.0f, 1.0));
    EXPECT_FALSE(int_test.test_violation(-2, 1.0));
    EXPECT_FALSE(double_test.test_violation(-2.0, 1.0));
    EXPECT_FALSE(float_test.test_violation(-2.0f, 1.0));

    // Inequality
    int_test.set_violation_condition(ConstraintEnum::NEQ);
    double_test.set_violation_condition(ConstraintEnum::NEQ);
    float_test.set_violation_condition(ConstraintEnum::NEQ);
    EXPECT_FALSE(int_test.test_violation(3, 1.0));
    EXPECT_FALSE(double_test.test_violation(3.0, 1.0));
    EXPECT_FALSE(float_test.test_violation(3.0f, 1.0));
    EXPECT_TRUE(int_test.test_violation(-2, 1.0));
    EXPECT_TRUE(double_test.test_violation(-2.0, 1.0));
    EXPECT_TRUE(float_test.test_violation(-2.0f, 1.0));

    // Greater-than
    int_test.set_violation_condition(ConstraintEnum::GT);
    double_test.set_violation_condition(ConstraintEnum::GT);
    float_test.set_violation_condition(ConstraintEnum::GT);
    EXPECT_TRUE(int_test.test_violation(4, 1.0));
    EXPECT_TRUE(double_test.test_violation(4.0, 1.0));
    EXPECT_TRUE(float_test.test_violation(4.0f, 1.0));
    EXPECT_FALSE(int_test.test_violation(-2, 1.0));
    EXPECT_FALSE(double_test.test_violation(-2.0, 1.0));
    EXPECT_FALSE(float_test.test_violation(-2.0f, 1.0));

    // Greater-than or equal-to
    int_test.set_violation_condition(ConstraintEnum::GE);
    double_test.set_violation_condition(ConstraintEnum::GE);
    float_test.set_violation_condition(ConstraintEnum::GE);
    EXPECT_TRUE(int_test.test_violation(4, 1.0));
    EXPECT_TRUE(double_test.test_violation(4.0, 1.0));
    EXPECT_TRUE(float_test.test_violation(4.0f, 1.0));
    EXPECT_TRUE(int_test.test_violation(3, 1.0));
    EXPECT_TRUE(double_test.test_violation(3.0, 1.0));
    EXPECT_TRUE(float_test.test_violation(3.0f, 1.0));
    EXPECT_FALSE(int_test.test_violation(-2, 1.0));
    EXPECT_FALSE(double_test.test_violation(-2.0, 1.0));
    EXPECT_FALSE(float_test.test_violation(-2.0f, 1.0));

    // Less-than
    int_test.set_violation_condition(ConstraintEnum::LT);
    double_test.set_violation_condition(ConstraintEnum::LT);
    float_test.set_violation_condition(ConstraintEnum::LT);
    EXPECT_FALSE(int_test.test_violation(4, 1.0));
    EXPECT_FALSE(double_test.test_violation(4.0, 1.0));
    EXPECT_FALSE(float_test.test_violation(4.0f, 1.0));
    EXPECT_TRUE(int_test.test_violation(-2, 1.0));
    EXPECT_TRUE(double_test.test_violation(-2.0, 1.0));
    EXPECT_TRUE(float_test.test_violation(-2.0f, 1.0));

    // Less-than or equal-to
    int_test.set_violation_condition(ConstraintEnum::LE);
    double_test.set_violation_condition(ConstraintEnum::LE);
    float_test.set_violation_condition(ConstraintEnum::LE);
    EXPECT_FALSE(int_test.test_violation(4, 1.0));
    EXPECT_FALSE(double_test.test_violation(4.0, 1.0));
    EXPECT_FALSE(float_test.test_violation(4.0f, 1.0));
    EXPECT_TRUE(int_test.test_violation(3, 1.0));
    EXPECT_TRUE(double_test.test_violation(3.0, 1.0));
    EXPECT_TRUE(float_test.test_violation(3.0f, 1.0));
    EXPECT_TRUE(int_test.test_violation(-2, 1.0));
    EXPECT_TRUE(double_test.test_violation(-2.0, 1.0));
    EXPECT_TRUE(float_test.test_violation(-2.0f, 1.0));
}

// Test the violation determination logic for timed threshold constraints with the
// timing feature enabled.
TEST(ConstraintTestThresholdTimed, TestViolationWithTimer) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_ThresholdTimed<int> int_test;
    ConstraintTest_ThresholdTimed<double> double_test;
    ConstraintTest_ThresholdTimed<float> float_test;

    // Initialize all test articles.
    int_test.threshold = 3;
    int_test.set_violation_condition(ConstraintEnum::NEQ);
    int_test.use_timer = true;
    int_test.time_limit = 2.0;
    double_test.threshold = 3.0;
    double_test.set_violation_condition(ConstraintEnum::NEQ);
    double_test.use_timer = true;
    double_test.time_limit = 2.0;
    float_test.threshold = 3.0f;
    float_test.set_violation_condition(ConstraintEnum::NEQ);
    float_test.use_timer = true;
    float_test.time_limit = 2.0;

    EXPECT_EQ(int_test.get_time_limit(), 2.0);
    EXPECT_EQ(double_test.get_time_limit(), 2.0);
    EXPECT_EQ(float_test.get_time_limit(), 2.0);

    int_test.initialize();
    double_test.initialize();
    float_test.initialize();
    int_test.activate();
    double_test.activate();
    float_test.activate();

    constexpr double dt = 1.0;

    // Cycle 1: no violation.
    EXPECT_FALSE(int_test.test_violation(3, dt));
    EXPECT_FALSE(double_test.test_violation(3.0, dt));
    EXPECT_FALSE(float_test.test_violation(3.0f, dt));

    // Cycle 2: the timer increments. No violation is reported yet.
    EXPECT_FALSE(int_test.test_violation(10, dt));
    EXPECT_FALSE(double_test.test_violation(10.0, dt));
    EXPECT_FALSE(float_test.test_violation(10.0f, dt));

    // Cycle 3: the violation has been active for 2 seconds. The violation is reported.
    EXPECT_TRUE(int_test.test_violation(10, dt));
    EXPECT_TRUE(double_test.test_violation(10.0, dt));
    EXPECT_TRUE(float_test.test_violation(10.0f, dt));

    // Cycle 4: no longer violating the constraint.
    EXPECT_FALSE(int_test.test_violation(3, dt));
    EXPECT_FALSE(double_test.test_violation(3.0, dt));
    EXPECT_FALSE(float_test.test_violation(3.0f, dt));
}

// Test the logic which resets a violation when a "gap period" with no violation
// is long enough to reset the timer.
TEST(ConstraintTestThresholdTimed, TestGapTimer) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_ThresholdTimed<int> article;
    article.threshold = 1;
    article.set_violation_condition(ConstraintEnum::NEQ);
    article.use_timer = true;
    article.time_limit = 1.1;
    article.gap_time_limit = 3.0;
    article.initialize();
    article.activate();

    constexpr double dt = 1.0;

    // Cycle 1: the constraint is violated for 1 second, which is not enough to trip
    // the timer.
    EXPECT_FALSE(article.test_violation(10, dt));

    // Cycle 2: the constraint is violated for 2 seconds, which trips the timer.
    EXPECT_TRUE(article.test_violation(10, dt));

    // Cycles 3 and 4: the constraint is no longer violated, but the gap time limit has
    // not yet been reached. If another violation occurs within the next second, the
    EXPECT_FALSE(article.test_violation(1, dt));
    EXPECT_FALSE(article.test_violation(1, dt));

    // Cycle 5: the constraint is violated for another second. Ordinarily this wouldn't
    // trip the violation time limit but since we haven't reset the gap timer from the
    // previous numerical violation, a violation is reported.
    EXPECT_TRUE(article.test_violation(10, dt));
}

// Test that the model emits an error message when configured improperly.
TEST(ConstraintTestThresholdTimed, InvalidConfiguration) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    ConstraintTest_ThresholdTimed<int> article;
    article.threshold = 1;
    article.set_violation_condition(ConstraintEnum::LT);
    article.initialize();
    article.activate();

    // The inside/outside violation condition is for intervals and sets, not thresholds.
    // The model will reset its violation condition to greater-than.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _,
            HasSubstr("Cannot initialize a threshold constraint-test without a valid violation-condition")));
    article.set_violation_condition(ConstraintEnum::In);
    EXPECT_EQ(article.get_violation_condition(), ConstraintEnum::GT);
}

// Test that no violation is reported if the model is inactive.
TEST(ConstraintTestIntervalTimed, Inactive) {
    ConstraintTest_IntervalTimed<int> article;
    article.enabled = false;
    article.lower_bound = 1;
    article.upper_bound = 10;
    article.set_violation_condition(ConstraintEnum::In);

    // Initialization should proceed even when disabled.
    article.initialize();
    EXPECT_TRUE(article.is_initialized());

    // Since the model is disabled, it should not be active.
    article.activate();
    EXPECT_FALSE(article.is_active());

    // Since the model is inactive, a value which would ordinarily result in a violation
    // will not be reported as such.
    EXPECT_FALSE(article.test_violation(5, 1.0));
    EXPECT_FALSE(article.get_violation());
}

// Test the violation determination logic for timed interval constraints with
// the timing feature disabled.
TEST(ConstraintTestIntervalTimed, TestViolationNoTimer) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_IntervalTimed<int> inside_interval_test;
    ConstraintTest_IntervalTimed<int> outside_interval_test;

    // Initialize all test articles.
    inside_interval_test.lower_bound = -3;
    inside_interval_test.upper_bound = 2;
    inside_interval_test.set_violation_condition(ConstraintEnum::In);
    inside_interval_test.use_timer = false;

    outside_interval_test.lower_bound = -3;
    outside_interval_test.upper_bound = 2;
    outside_interval_test.set_violation_condition(ConstraintEnum::Out);
    outside_interval_test.use_timer = false;

    inside_interval_test.initialize();
    inside_interval_test.activate();

    outside_interval_test.initialize();
    outside_interval_test.activate();

    constexpr double dt = 1.0;

    // Closed-closed interval.
    inside_interval_test.interval_bounds = ConstraintEnum::ClosedClosed;
    outside_interval_test.interval_bounds = ConstraintEnum::ClosedClosed;
    EXPECT_FALSE(inside_interval_test.test_violation(-5, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(-5, dt));

    EXPECT_TRUE(inside_interval_test.test_violation(-3, dt));
    EXPECT_FALSE(outside_interval_test.test_violation(-3, dt));

    EXPECT_TRUE(inside_interval_test.test_violation(0, dt));
    EXPECT_FALSE(outside_interval_test.test_violation(0, dt));

    EXPECT_TRUE(inside_interval_test.test_violation(2, dt));
    EXPECT_FALSE(outside_interval_test.test_violation(2, dt));

    EXPECT_FALSE(inside_interval_test.test_violation(5, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(5, dt));

    // Open-open interval.
    inside_interval_test.interval_bounds = ConstraintEnum::OpenOpen;
    outside_interval_test.interval_bounds = ConstraintEnum::OpenOpen;
    EXPECT_FALSE(inside_interval_test.test_violation(-5, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(-5, dt));

    EXPECT_FALSE(inside_interval_test.test_violation(-3, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(-3, dt));

    EXPECT_TRUE(inside_interval_test.test_violation(0, dt));
    EXPECT_FALSE(outside_interval_test.test_violation(0, dt));

    EXPECT_FALSE(inside_interval_test.test_violation(2, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(2, dt));

    EXPECT_FALSE(inside_interval_test.test_violation(5, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(5, dt));

    // Closed-open interval.
    inside_interval_test.interval_bounds = ConstraintEnum::ClosedOpen;
    outside_interval_test.interval_bounds = ConstraintEnum::ClosedOpen;
    EXPECT_FALSE(inside_interval_test.test_violation(-5, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(-5, dt));

    EXPECT_TRUE(inside_interval_test.test_violation(-3, dt));
    EXPECT_FALSE(outside_interval_test.test_violation(-3, dt));

    EXPECT_TRUE(inside_interval_test.test_violation(0, dt));
    EXPECT_FALSE(outside_interval_test.test_violation(0, dt));

    EXPECT_FALSE(inside_interval_test.test_violation(2, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(2, dt));

    EXPECT_FALSE(inside_interval_test.test_violation(5, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(5, dt));

    // Open-closed interval.
    inside_interval_test.interval_bounds = ConstraintEnum::OpenClosed;
    outside_interval_test.interval_bounds = ConstraintEnum::OpenClosed;
    EXPECT_FALSE(inside_interval_test.test_violation(-5, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(-5, dt));

    EXPECT_FALSE(inside_interval_test.test_violation(-3, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(-3, dt));

    EXPECT_TRUE(inside_interval_test.test_violation(0, dt));
    EXPECT_FALSE(outside_interval_test.test_violation(0, dt));

    EXPECT_TRUE(inside_interval_test.test_violation(2, dt));
    EXPECT_FALSE(outside_interval_test.test_violation(2, dt));

    EXPECT_FALSE(inside_interval_test.test_violation(5, dt));
    EXPECT_TRUE(outside_interval_test.test_violation(5, dt));
}

// Test the violation determination logic for timed interval constraints with
// the timing feature enabled.
TEST(ConstraintTestIntervalTimed, TestViolationWithTimer) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_IntervalTimed<int> article;

    // Initialize all test articles.
    article.lower_bound = -3;
    article.upper_bound = 2;
    article.set_violation_condition(ConstraintEnum::In);
    article.use_timer = true;
    article.time_limit = 2.0;
    article.interval_bounds = ConstraintEnum::OpenOpen;

    article.initialize();
    article.activate();

    constexpr double dt = 1.0;

    // Cycle 1: no violation.
    EXPECT_FALSE(article.test_violation(10, dt));

    // Cycle 2: violation has not reached the necessary persistence.
    EXPECT_FALSE(article.test_violation(0, dt));

    // Cycle 3: violation has reached the necessary persistence.
    EXPECT_TRUE(article.test_violation(0, dt));

    // Cycle 4: no violation.
    EXPECT_FALSE(article.test_violation(10, dt));
}

// Test that the model emits an error message when configured improperly.
TEST(ConstraintTestIntervalTimed, InvalidConfiguration) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    ConstraintTest_IntervalTimed<int> article;
    article.lower_bound = 1;
    article.upper_bound = 10;
    article.set_violation_condition(ConstraintEnum::In);
    article.initialize();
    article.activate();

    // Even with swapped upper and lower bounds, violations are reported correctly.
    article.lower_bound = 10;
    article.upper_bound = 1;
    article.initialize();
    EXPECT_TRUE(article.test_violation(5, 1.0));

    // The violation condition must be inside or outside, not a comparison function.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _,
            HasSubstr("Cannot initialize an interval constraint-test")));
    article.set_violation_condition(ConstraintEnum::EQ);

    // The upper and lower bounds can't be too close together.
    article.upper_bound = 5;
    article.lower_bound = 5;
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("zero-width interval")));
    EXPECT_FALSE(article.initialize());
}

// Test that no violation is reported if the model is inactive.
TEST(ConstraintTestValSetTimed, Inactive) {
    ConstraintTest_ValSetTimed<int> article;
    article.enabled = false;
    article.values = {1, 2, 3};
    article.set_violation_condition(ConstraintEnum::In);

    // Initialization should proceed even when disabled.
    article.initialize();
    EXPECT_TRUE(article.is_initialized());

    // Since the model is disabled, it should not be active.
    article.activate();
    EXPECT_FALSE(article.is_active());

    // Since the model is inactive, a value which would ordinarily result in a violation
    // will not be reported as such.
    EXPECT_FALSE(article.test_violation(2, 1.0));
    EXPECT_FALSE(article.get_violation());
}

// Test the violation determination logic for timed value-set constraints with
// the timing feature disabled.
TEST(ConstraintTestValSet, TestViolationNoTimer) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_ValSetTimed<int> inside_valset_test;
    ConstraintTest_ValSetTimed<int> outside_valset_test;

    // Initialize all test articles.
    inside_valset_test.values = {1, -2, 3};
    inside_valset_test.set_violation_condition(ConstraintEnum::In);
    inside_valset_test.use_timer = false;

    outside_valset_test.values = {1, -2, 3};
    outside_valset_test.set_violation_condition(ConstraintEnum::Out);
    outside_valset_test.use_timer = false;

    inside_valset_test.initialize();
    inside_valset_test.activate();

    outside_valset_test.initialize();
    outside_valset_test.activate();

    constexpr double dt = 1.0;

    // Value is contained in the set.
    EXPECT_TRUE(inside_valset_test.test_violation(-2, dt));
    EXPECT_FALSE(outside_valset_test.test_violation(-2, dt));

    // Value is not contained in the set.
    EXPECT_FALSE(inside_valset_test.test_violation(2, dt));
    EXPECT_TRUE(outside_valset_test.test_violation(2, dt));
}

// Test the violation determination logic for timed value-set constraints with
// the timing feature enabled.
TEST(ConstraintTestValSet, TestViolationWithTimer) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_ValSetTimed<int> article;

    // Initialize all test articles.
    article.values = {1, -2, 3};
    article.set_violation_condition(ConstraintEnum::In);
    article.use_timer = true;
    article.time_limit = 2.0;

    article.initialize();
    article.activate();

    constexpr double dt = 1.0;

    // Cycle 1: no violation.
    EXPECT_FALSE(article.test_violation(10, dt));

    // Cycle 2: violation has not reached the necessary persistence.
    EXPECT_FALSE(article.test_violation(1, dt));

    // Cycle 3: violation has reached the necessary persistence.
    EXPECT_TRUE(article.test_violation(1, dt));

    // Cycle 4: no violation.
    EXPECT_FALSE(article.test_violation(10, dt));
}

// Test that the model emits an error message when configured improperly.
TEST(ConstraintTestValSetTimed, InvalidConfiguration) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    ConstraintTest_ValSetTimed<int> article;
    article.values = {1, -2, 3};
    article.set_violation_condition(ConstraintEnum::In);
    article.initialize();
    article.activate();

    // The violation condition must be inside or outside, not a comparison function.
    EXPECT_CALL(cml_message_mock, publish(CMLMessage::Error, _, _, _));
    article.set_violation_condition(ConstraintEnum::EQ);
    EXPECT_EQ(article.get_violation_condition(), ConstraintEnum::In);

    // There must be at least one value to test against.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _,
            HasSubstr("Cannot initialize a value-set constraint-test with an empty\nset of values")));
    article.values.clear();
    EXPECT_FALSE(article.initialize());
}

}
