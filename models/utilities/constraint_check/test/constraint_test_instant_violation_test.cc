#include "../include/constraint_test_templates.hh"
#include "mocks/cml/cml_message_mock.hh"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Test that no violation is reported if the model is inactive.
TEST(ConstraintTestThreshold, Inactive) {
    ConstraintTest_Threshold<int> article;
    article.enabled = false;
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
    EXPECT_FALSE(article.test_violation(10000));
    EXPECT_FALSE(article.get_violation());
}

// Test the violation determination logic for instantaneous threshold constraints.
TEST(ConstraintTestThreshold, TestViolation) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_Threshold<int> int_test;
    ConstraintTest_Threshold<double> double_test;
    ConstraintTest_Threshold<float> float_test;

    // Initialize all test articles.
    int_test.threshold = 3;
    int_test.set_violation_condition(ConstraintEnum::EQ);
    double_test.threshold = 3.0;
    double_test.set_violation_condition(ConstraintEnum::EQ);
    float_test.threshold = 3.0f;
    float_test.set_violation_condition(ConstraintEnum::EQ);

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
    EXPECT_TRUE(int_test.test_violation(3));
    EXPECT_TRUE(double_test.test_violation(3.0));
    EXPECT_TRUE(float_test.test_violation(3.0f));
    EXPECT_FALSE(int_test.test_violation(-2));
    EXPECT_FALSE(double_test.test_violation(-2.0));
    EXPECT_FALSE(float_test.test_violation(-2.0f));

    // Inequality
    int_test.set_violation_condition(ConstraintEnum::NEQ);
    double_test.set_violation_condition(ConstraintEnum::NEQ);
    float_test.set_violation_condition(ConstraintEnum::NEQ);
    EXPECT_FALSE(int_test.test_violation(3));
    EXPECT_FALSE(double_test.test_violation(3.0));
    EXPECT_FALSE(float_test.test_violation(3.0f));
    EXPECT_TRUE(int_test.test_violation(-2));
    EXPECT_TRUE(double_test.test_violation(-2.0));
    EXPECT_TRUE(float_test.test_violation(-2.0f));

    // Greater-than
    int_test.set_violation_condition(ConstraintEnum::GT);
    double_test.set_violation_condition(ConstraintEnum::GT);
    float_test.set_violation_condition(ConstraintEnum::GT);
    EXPECT_TRUE(int_test.test_violation(4));
    EXPECT_TRUE(double_test.test_violation(4.0));
    EXPECT_TRUE(float_test.test_violation(4.0f));
    EXPECT_FALSE(int_test.test_violation(-2));
    EXPECT_FALSE(double_test.test_violation(-2.0));
    EXPECT_FALSE(float_test.test_violation(-2.0f));

    // Greater-than or equal-to
    int_test.set_violation_condition(ConstraintEnum::GE);
    double_test.set_violation_condition(ConstraintEnum::GE);
    float_test.set_violation_condition(ConstraintEnum::GE);
    EXPECT_TRUE(int_test.test_violation(4));
    EXPECT_TRUE(double_test.test_violation(4.0));
    EXPECT_TRUE(float_test.test_violation(4.0f));
    EXPECT_TRUE(int_test.test_violation(3));
    EXPECT_TRUE(double_test.test_violation(3.0));
    EXPECT_TRUE(float_test.test_violation(3.0f));
    EXPECT_FALSE(int_test.test_violation(-2));
    EXPECT_FALSE(double_test.test_violation(-2.0));
    EXPECT_FALSE(float_test.test_violation(-2.0f));

    // Less-than
    int_test.set_violation_condition(ConstraintEnum::LT);
    double_test.set_violation_condition(ConstraintEnum::LT);
    float_test.set_violation_condition(ConstraintEnum::LT);
    EXPECT_FALSE(int_test.test_violation(4));
    EXPECT_FALSE(double_test.test_violation(4.0));
    EXPECT_FALSE(float_test.test_violation(4.0f));
    EXPECT_TRUE(int_test.test_violation(-2));
    EXPECT_TRUE(double_test.test_violation(-2.0));
    EXPECT_TRUE(float_test.test_violation(-2.0f));

    // Less-than or equal-to
    int_test.set_violation_condition(ConstraintEnum::LE);
    double_test.set_violation_condition(ConstraintEnum::LE);
    float_test.set_violation_condition(ConstraintEnum::LE);
    EXPECT_FALSE(int_test.test_violation(4));
    EXPECT_FALSE(double_test.test_violation(4.0));
    EXPECT_FALSE(float_test.test_violation(4.0f));
    EXPECT_TRUE(int_test.test_violation(3));
    EXPECT_TRUE(double_test.test_violation(3.0));
    EXPECT_TRUE(float_test.test_violation(3.0f));
    EXPECT_TRUE(int_test.test_violation(-2));
    EXPECT_TRUE(double_test.test_violation(-2.0));
    EXPECT_TRUE(float_test.test_violation(-2.0f));
}

// Test that the model emits an error message when configured improperly.
TEST(ConstraintTestThreshold, InvalidConfiguration) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    ConstraintTest_Threshold<int> article;
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
TEST(ConstraintTestInterval, Inactive) {
    ConstraintTest_Interval<int> article;
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
    EXPECT_FALSE(article.test_violation(5));
    EXPECT_FALSE(article.get_violation());
}

// Test the violation determination logic for instantaneous interval constraints.
TEST(ConstraintTestInterval, TestViolation) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_Interval<int> inside_interval_test;
    ConstraintTest_Interval<int> outside_interval_test;

    // Initialize all test articles.
    inside_interval_test.lower_bound = -3;
    inside_interval_test.upper_bound = 2;
    inside_interval_test.set_violation_condition(ConstraintEnum::In);

    outside_interval_test.lower_bound = -3;
    outside_interval_test.upper_bound = 2;
    outside_interval_test.set_violation_condition(ConstraintEnum::Out);

    inside_interval_test.initialize();
    inside_interval_test.activate();

    outside_interval_test.initialize();
    outside_interval_test.activate();

    // Closed-closed interval.
    inside_interval_test.interval_bounds = ConstraintEnum::ClosedClosed;
    outside_interval_test.interval_bounds = ConstraintEnum::ClosedClosed;
    EXPECT_FALSE(inside_interval_test.test_violation(-5));
    EXPECT_TRUE(outside_interval_test.test_violation(-5));

    EXPECT_TRUE(inside_interval_test.test_violation(-3));
    EXPECT_FALSE(outside_interval_test.test_violation(-3));

    EXPECT_TRUE(inside_interval_test.test_violation(0));
    EXPECT_FALSE(outside_interval_test.test_violation(0));

    EXPECT_TRUE(inside_interval_test.test_violation(2));
    EXPECT_FALSE(outside_interval_test.test_violation(2));

    EXPECT_FALSE(inside_interval_test.test_violation(5));
    EXPECT_TRUE(outside_interval_test.test_violation(5));

    // Open-open interval.
    inside_interval_test.interval_bounds = ConstraintEnum::OpenOpen;
    outside_interval_test.interval_bounds = ConstraintEnum::OpenOpen;
    EXPECT_FALSE(inside_interval_test.test_violation(-5));
    EXPECT_TRUE(outside_interval_test.test_violation(-5));

    EXPECT_FALSE(inside_interval_test.test_violation(-3));
    EXPECT_TRUE(outside_interval_test.test_violation(-3));

    EXPECT_TRUE(inside_interval_test.test_violation(0));
    EXPECT_FALSE(outside_interval_test.test_violation(0));

    EXPECT_FALSE(inside_interval_test.test_violation(2));
    EXPECT_TRUE(outside_interval_test.test_violation(2));

    EXPECT_FALSE(inside_interval_test.test_violation(5));
    EXPECT_TRUE(outside_interval_test.test_violation(5));

    // Closed-open interval.
    inside_interval_test.interval_bounds = ConstraintEnum::ClosedOpen;
    outside_interval_test.interval_bounds = ConstraintEnum::ClosedOpen;
    EXPECT_FALSE(inside_interval_test.test_violation(-5));
    EXPECT_TRUE(outside_interval_test.test_violation(-5));

    EXPECT_TRUE(inside_interval_test.test_violation(-3));
    EXPECT_FALSE(outside_interval_test.test_violation(-3));

    EXPECT_TRUE(inside_interval_test.test_violation(0));
    EXPECT_FALSE(outside_interval_test.test_violation(0));

    EXPECT_FALSE(inside_interval_test.test_violation(2));
    EXPECT_TRUE(outside_interval_test.test_violation(2));

    EXPECT_FALSE(inside_interval_test.test_violation(5));
    EXPECT_TRUE(outside_interval_test.test_violation(5));

    // Open-closed interval.
    inside_interval_test.interval_bounds = ConstraintEnum::OpenClosed;
    outside_interval_test.interval_bounds = ConstraintEnum::OpenClosed;
    EXPECT_FALSE(inside_interval_test.test_violation(-5));
    EXPECT_TRUE(outside_interval_test.test_violation(-5));

    EXPECT_FALSE(inside_interval_test.test_violation(-3));
    EXPECT_TRUE(outside_interval_test.test_violation(-3));

    EXPECT_TRUE(inside_interval_test.test_violation(0));
    EXPECT_FALSE(outside_interval_test.test_violation(0));

    EXPECT_TRUE(inside_interval_test.test_violation(2));
    EXPECT_FALSE(outside_interval_test.test_violation(2));

    EXPECT_FALSE(inside_interval_test.test_violation(5));
    EXPECT_TRUE(outside_interval_test.test_violation(5));
}

// Test that the model emits an error message when configured improperly.
TEST(ConstraintTestInterval, InvalidConfiguration) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    ConstraintTest_Interval<int> article;
    article.lower_bound = 1;
    article.upper_bound = 10;
    article.set_violation_condition(ConstraintEnum::In);
    article.initialize();
    article.activate();

    // Even with swapped upper and lower bounds, violations are reported correctly.
    article.lower_bound = 10;
    article.upper_bound = 1;
    article.initialize();
    EXPECT_TRUE(article.test_violation(5));

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
TEST(ConstraintTestValSet, Inactive) {
    ConstraintTest_ValSet<int> article;
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
    EXPECT_FALSE(article.test_violation(2));
    EXPECT_FALSE(article.get_violation());
}

// Test the violation determination logic for instantaneous value-set constraints.
TEST(ConstraintTestValSet, TestViolation) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    ConstraintTest_ValSet<int> inside_valset_test;
    ConstraintTest_ValSet<int> outside_valset_test;

    // Initialize all test articles.
    inside_valset_test.values = {1, -2, 3};
    inside_valset_test.set_violation_condition(ConstraintEnum::In);

    outside_valset_test.values = {1, -2, 3};
    outside_valset_test.set_violation_condition(ConstraintEnum::Out);

    inside_valset_test.initialize();
    inside_valset_test.activate();

    outside_valset_test.initialize();
    outside_valset_test.activate();

    // Value is contained in the set.
    EXPECT_TRUE(inside_valset_test.test_violation(-2));
    EXPECT_FALSE(outside_valset_test.test_violation(-2));

    // Value is not contained in the set.
    EXPECT_FALSE(inside_valset_test.test_violation(2));
    EXPECT_TRUE(outside_valset_test.test_violation(2));
}

// Test that the model emits an error message when configured improperly.
TEST(ConstraintTestValSet, InvalidConfiguration) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    ConstraintTest_ValSet<int> article;
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
