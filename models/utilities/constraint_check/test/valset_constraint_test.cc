#include "../include/constraint_set.hh"
#include "../include/constraint_enum.hh"
#include "../include/valset_constraint.hh"
#include "mocks/cml/cml_message_mock.hh"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Set common configuration options for the ValSetInstantConstraint tests.
void set_val_set_instant_constraint_params(ValSetInstantConstraint<double, 2>& article) {
    article.tests[0].set_violation_condition(ConstraintEnum::In);
    article.tests[0].values = {1.0, 2.0, 3.0};
    article.tests[1].set_violation_condition(ConstraintEnum::In);
    article.tests[1].values = {3.0, 4.0, 5.0};
    article.violate_on_any_test = true;
}

// Set common configuration options for the ValSetTimedConstraint tests.
void set_val_set_timed_constraint_params(ValSetTimedConstraint<double, 2>& article) {
    article.tests[0].set_violation_condition(ConstraintEnum::In);
    article.tests[0].values = {1.0, 2.0, 3.0};
    article.tests[0].time_limit = 1.1;
    article.tests[1].set_violation_condition(ConstraintEnum::In);
    article.tests[1].values = {3.0, 4.0, 5.0};
    article.tests[1].time_limit = 1.1;
    article.violate_on_any_test = true;
}

// Test a nominal update of the ValSetInstantConstraint
TEST(ValSetInstantConstraint, Update) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    double var = 0.0;
    ValSetInstantConstraint<double, 2> article(var);
    set_val_set_instant_constraint_params(article);
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
TEST(ValSetInstantConstraint, Inactive) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    const double var = 2.0;
    ValSetInstantConstraint<double, 2> article(var);
    set_val_set_instant_constraint_params(article);
    article.initialize();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_FALSE(article.is_active());

    // This would ordinarily emit a violation since 2 is part of one of the value sets.
    article.update();
    EXPECT_EQ(article.violation_count, 0);
}

// Test that a suspicious configuration set causes a warning to be emitted during
// initialization.
TEST(ValSetInstantConstraint, SuspiciousConfiguration) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    const double var = 0.0;
    ValSetInstantConstraint<double, 2> article(var);
    set_val_set_instant_constraint_params(article);

    // Set the violation conditions to be when the variable is outside any interval.
    article.tests[0].set_violation_condition(ConstraintEnum::Out);
    article.violate_on_any_test = true;

    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Warning, _, _, HasSubstr("Possible mismatch on constraint")));
    article.initialize();
}

// Test a nominal update of the ValSetInstantConstraint
TEST(ValSetTimedConstraint, Update) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    double var = 0.0;
    const double dt = 1.0;
    ValSetTimedConstraint<double, 2> article(var, dt);
    set_val_set_timed_constraint_params(article);
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
TEST(ValSetTimedConstraint, Inactive) {
    // Testing nominal behavior only. No messages should be emitted.
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;
    EXPECT_CALL(cml_message_mock, publish).Times(0);

    const double var = 2.0;
    const double dt = 1000000.0;
    ValSetTimedConstraint<double, 2> article(var, dt);
    set_val_set_timed_constraint_params(article);
    article.initialize();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_FALSE(article.is_active());

    // This would ordinarily emit a violation since 2 is part of one of the value sets
    // and the necessary persistence has been reached.
    article.update();
    EXPECT_EQ(article.violation_count, 0);
}

// Test that a suspicious configuration set causes a warning to be emitted during
// initialization.
TEST(ValSetTimedConstraint, SuspiciousConfiguration) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Construct with the ConstraintSet for coverage.
    const double var = 0.0;
    const double dt = 1.0;
    ConstraintSet set;
    ValSetTimedConstraint<double, 2> article(var, dt, set);
    set_val_set_timed_constraint_params(article);

    // Set the violation conditions to be when the variable is outside any interval.
    article.tests[0].set_violation_condition(ConstraintEnum::Out);
    article.violate_on_any_test = true;

    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Warning, _, _, HasSubstr("Possible mismatch on constraint")));
    article.initialize();
}

}
