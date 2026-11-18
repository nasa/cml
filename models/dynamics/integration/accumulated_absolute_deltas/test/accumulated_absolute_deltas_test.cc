#include "../include/accumulated_absolute_deltas.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

inline constexpr double tolerance = 1e-12; // Float comparison tolerance

TEST(AccumulatedAbsoluteDeltas, Construction) {
    using testing::_;
    using testing::HasSubstr;

    const double variable[3] {};
    CMLMessage::Mock cml_message_mock;

    // Test attempting to construct with a null pointer.
    EXPECT_CALL(
        cml_message_mock,
        publish(CMLMessage::Fail, _, _, HasSubstr("The variable being accumulated is NULL")));
    const AccumulatedAbsoluteDeltas null_test(nullptr, 3);

    // Test attempting to construct with an array of size 0.
    EXPECT_CALL(
        cml_message_mock,
        publish(CMLMessage::Fail, _, _, HasSubstr("The size of the array must be positive")));
    const AccumulatedAbsoluteDeltas zero_test(variable, 0);

    // Test normal construction.
    const AccumulatedAbsoluteDeltas valid1(variable, std::size(variable));
    const AccumulatedAbsoluteDeltas valid2(variable);
    EXPECT_NE(valid1.accumulated_deltas, nullptr);
    EXPECT_NE(valid2.accumulated_deltas, nullptr);
}

TEST(AccumulatedAbsoluteDeltas, Disabled) {
    double data = 0.0;
    AccumulatedAbsoluteDeltas article(&data, 1);
    article.disable();

    // Attempt to initialize the model while it's disabled.
    article.initialize();
    EXPECT_FALSE(article.is_initialized());

    // The data changes but since the model is disabled, it will not accumulate
    // the deltas.
    data = 1.0;
    article.update();
    EXPECT_EQ(article.accumulated_deltas[0], 0.0);
}

TEST(AccumulatedAbsoluteDeltas, Update) {
    double data[2] {1.0, -1.0};
    AccumulatedAbsoluteDeltas article(data);
    article.subscribe();
    article.initialize();

    // Test a few updates.
    // Update 1:
    //    Element 0 changes to 0.1: net change = 0.1
    //    Element 1 stays the same: net change = 0
    data[0] = 1.1;
    data[1] = -1.0;
    article.update();
    EXPECT_NEAR(article.accumulated_deltas[0], 0.1, tolerance);
    EXPECT_NEAR(article.accumulated_deltas[1], 0.0, tolerance);

    // Update 2:
    //    Element 0 changes to 0: net change = 0.1 + (1.1 - 0) = 1.2
    //    Element 1 changes to 0: net change = 1
    data[0] = 0.0;
    data[1] = 0.0;
    article.update();
    EXPECT_NEAR(article.accumulated_deltas[0], 1.2, tolerance);
    EXPECT_NEAR(article.accumulated_deltas[1], 1.0, tolerance);

    // Update 3:
    //    Element 0 changes to -5: net change = 1.2 + 5 = 6.2
    //    Element 1 changes to 10: net change = 1 + 10 = 11
    data[0] = -5.0;
    data[1] = 10.0;
    article.update();
    EXPECT_NEAR(article.accumulated_deltas[0], 6.2, tolerance);
    EXPECT_NEAR(article.accumulated_deltas[1], 11.0, tolerance);

    // The model is deactivated. The data it's accumulating changes but the
    // accumulated deltas remain the same.
    article.unsubscribe();
    data[0] = 0.0;
    data[1] = 0.0;
    article.update();
    EXPECT_NEAR(article.accumulated_deltas[0], 6.2, tolerance);
    EXPECT_NEAR(article.accumulated_deltas[1], 11.0, tolerance);
}

} // namespace
