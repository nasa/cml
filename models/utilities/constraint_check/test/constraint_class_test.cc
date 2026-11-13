#include "../include/constraint.hh"
#include "../include/constraint_enum.hh"
#include "../include/constraint_test_templates.hh"
#include "mocks/cml/cml_message_mock.hh"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Test driver
class ConstraintUnitTest : public Constraint {
public:
    using Constraint::Constraint;
    void update() override {post_update();}
};

// Test various model configuration issues.
TEST(Constraint, ConfigurationErrors) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    ConstraintUnitTest article;
    article.violation_condition = ConstraintEnum::In;

    // Test an empty list of constraints.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("Constraint requires at least one constraint-test")));
    article.initialize();
    EXPECT_FALSE(article.is_initialized());

    // If initialization failed, the model should also fail to activate.
    article.activate();
    EXPECT_FALSE(article.is_active());

    // Create some constraint tests.
    // The first test will have an invalid violation condition.
    // The second test will fail initialization due to its value set being empty.
    ConstraintTest_ValSet<int> constraint_tests[2];
    constraint_tests[0].values = {1, 2, 3};
    constraint_tests[1].set_violation_condition(ConstraintEnum::In);
    article.register_tests(constraint_tests);

    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("Cannot initialize a value-set constraint-test with an empty\nset of values")));
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("Failure to initialize a constraint-test")));
    article.initialize();
    EXPECT_FALSE(article.is_initialized());
}

// Update the constraint with the violate_on_any_test option active.
TEST(Constraint, ViolateOnAnyTestViolation) {
    ConstraintUnitTest article;

    ConstraintTest_ValSet<int> constraint_tests[2];
    constraint_tests[0].values = {1, 2, 3};
    constraint_tests[0].set_violation_condition(ConstraintEnum::In);
    constraint_tests[1].values = {2, 4, 6, 8};
    constraint_tests[1].set_violation_condition(ConstraintEnum::In);
    article.register_tests(constraint_tests);
    article.violate_on_any_test = true;

    article.initialize();
    article.activate();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_TRUE(article.is_active());

    // Case 1: Neither constraint-test is violated.
    constraint_tests[0].test_violation(0);
    constraint_tests[1].test_violation(0);
    article.update();
    EXPECT_FALSE(constraint_tests[0].get_violation());
    EXPECT_FALSE(constraint_tests[1].get_violation());
    EXPECT_FALSE(article.violated);
    EXPECT_EQ(article.violation_count, 0);

    // Case 2: The first constraint-test is violated.
    constraint_tests[0].test_violation(1);
    constraint_tests[1].test_violation(1);
    article.update();
    EXPECT_TRUE(constraint_tests[0].get_violation());
    EXPECT_FALSE(constraint_tests[1].get_violation());
    EXPECT_TRUE(article.violated);
    EXPECT_EQ(article.violation_count, 1);
    EXPECT_EQ(article.test_violated_index, 0);

    // Case 3: Both constraint-tests are violated. Note that the violation count
    // remains the same since the previous violation status has not cleared.
    constraint_tests[0].test_violation(2);
    constraint_tests[1].test_violation(2);
    article.update();
    EXPECT_TRUE(constraint_tests[0].get_violation());
    EXPECT_TRUE(constraint_tests[1].get_violation());
    EXPECT_TRUE(article.violated);
    EXPECT_EQ(article.violation_count, 1);
    EXPECT_EQ(article.test_violated_index, 0);

    // Case 4: The second constraint-test is violated. Note that the violation count
    // remains the same since the previous violation status has not cleared.
    constraint_tests[0].test_violation(8);
    constraint_tests[1].test_violation(8);
    article.update();
    EXPECT_FALSE(constraint_tests[0].get_violation());
    EXPECT_TRUE(constraint_tests[1].get_violation());
    EXPECT_TRUE(article.violated);
    EXPECT_EQ(article.violation_count, 1);
    EXPECT_EQ(article.test_violated_index, 1);
}

// Update the constraint with the violate_on_any_test option off (i.e., require all test violations).
TEST(Constraint, ViolateOnAllTestViolations) {
    ConstraintUnitTest article;

    ConstraintTest_ValSet<int> constraint_tests[2];
    constraint_tests[0].values = {1, 2, 3};
    constraint_tests[0].set_violation_condition(ConstraintEnum::In);
    constraint_tests[1].values = {2, 4, 6, 8};
    constraint_tests[1].set_violation_condition(ConstraintEnum::In);
    article.register_tests(constraint_tests);
    article.violate_on_any_test = false;

    article.initialize();
    article.activate();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_TRUE(article.is_active());

    // Case 1: Neither constraint-test is violated.
    constraint_tests[0].test_violation(0);
    constraint_tests[1].test_violation(0);
    article.update();
    EXPECT_FALSE(constraint_tests[0].get_violation());
    EXPECT_FALSE(constraint_tests[1].get_violation());
    EXPECT_FALSE(article.violated);
    EXPECT_EQ(article.violation_count, 0);

    // Case 2: The first constraint-test is violated.
    constraint_tests[0].test_violation(1);
    constraint_tests[1].test_violation(1);
    article.update();
    EXPECT_TRUE(constraint_tests[0].get_violation());
    EXPECT_FALSE(constraint_tests[1].get_violation());
    EXPECT_FALSE(article.violated);

    // Case 3: Both constraint-tests are violated.
    constraint_tests[0].test_violation(2);
    constraint_tests[1].test_violation(2);
    article.update();
    EXPECT_TRUE(constraint_tests[0].get_violation());
    EXPECT_TRUE(constraint_tests[1].get_violation());
    EXPECT_TRUE(article.violated);
    EXPECT_EQ(article.violation_count, 1);
    EXPECT_EQ(article.test_violated_index, 0);

    // Case 4: The second constraint-test is violated.
    constraint_tests[0].test_violation(8);
    constraint_tests[1].test_violation(8);
    article.update();
    EXPECT_FALSE(constraint_tests[0].get_violation());
    EXPECT_TRUE(constraint_tests[1].get_violation());
    EXPECT_FALSE(article.violated);
}

}
