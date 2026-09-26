#include "../include/generic_multi_input_table.hh"
#include "../include/generic_single_input_table.hh"
#include "../include/table_independent_variable.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <numeric>
#include <utility>
#include <vector>

namespace {

// Inspect protected storage without changing the model's public interface.
class InspectableTable : public GenericMultiInputTable {
public:
    using GenericMultiInputTable::GenericMultiInputTable;
    const DoubleVec& stored_data() const { return data; }
};

TEST(TableMoveData, IndependentTakesVectorStorage) {
    double input = 512.5;
    TableIndependentVariable table(input);
    DoubleVec values(1024);
    std::iota(values.begin(), values.end(), 0.0);
    const double* const storage = values.data();

    ASSERT_TRUE(table.load_data(std::move(values)));
    EXPECT_EQ(table.data.data(), storage);
    EXPECT_EQ(table.get_size(), 1024U);
    ASSERT_TRUE(table.initialize());
    EXPECT_EQ(table.get_index(), 512U);
    EXPECT_DOUBLE_EQ(table.fraction, 0.5);
}

TEST(TableMoveData, IndependentAcceptsDecreasingAndSinglePointData) {
    double input = 1.5;
    TableIndependentVariable table(input);
    DoubleVec decreasing{3.0, 2.0, 1.0};
    const double* const storage = decreasing.data();
    ASSERT_TRUE(table.load_data(std::move(decreasing)));
    EXPECT_EQ(table.data.data(), storage);
    EXPECT_FALSE(table.is_table_increasing());
    ASSERT_TRUE(table.initialize());
    EXPECT_EQ(table.get_index(), 1U);
    EXPECT_DOUBLE_EQ(table.fraction, 0.5);

    TableIndependentVariable constant(input);
    DoubleVec one_point{2.0};
    const double* const one_storage = one_point.data();
    CMLMessage::Mock messages;
    EXPECT_CALL(messages, publish(CMLMessage::Warning, testing::_, testing::_, testing::_));
    ASSERT_TRUE(constant.load_data(std::move(one_point)));
    EXPECT_EQ(constant.data.data(), one_storage);
    ASSERT_TRUE(constant.initialize());
    EXPECT_EQ(constant.get_index(), 0U);
}

TEST(TableMoveData, IndependentRejectsWithoutConsumingInput) {
    const std::vector<DoubleVec> invalid{{}, {1.0, 1.0}, {0.0, 2.0, 1.0}};
    for (const auto& expected : invalid) {
        double input = 0.0;
        TableIndependentVariable table(input);
        DoubleVec values = expected;
        const double* const storage = values.data();
        CMLMessage::Mock messages;
        EXPECT_CALL(messages, publish(CMLMessage::Error, testing::_, testing::_, testing::_));
        EXPECT_FALSE(table.load_data(std::move(values)));
        EXPECT_EQ(values, expected);
        EXPECT_EQ(values.data(), storage);
        EXPECT_FALSE(table.is_data_loaded());
    }
}

TEST(TableMoveData, IndependentRejectsReloadUntilCleared) {
    double input = 0.5;
    TableIndependentVariable table(input);
    const DoubleVec original{0.0, 1.0};
    ASSERT_TRUE(table.load_data(original));
    DoubleVec replacement{0.0, 2.0, 4.0};
    const double* const storage = replacement.data();
    {
        CMLMessage::Mock messages;
        EXPECT_CALL(messages, publish(CMLMessage::Error, testing::_, testing::_, testing::_));
        EXPECT_FALSE(table.load_data(std::move(replacement)));
    }
    EXPECT_EQ(table.data, original);
    EXPECT_EQ(replacement.data(), storage);
    EXPECT_EQ(replacement.size(), 3U);
    table.clear_data();
    ASSERT_TRUE(table.load_data(std::move(replacement)));
    EXPECT_EQ(table.data.data(), storage);
}

TEST(TableMoveData, IndependentLvalueStillCopies) {
    double input = 0.0;
    TableIndependentVariable table(input);
    const DoubleVec values{0.0, 1.0, 2.0};
    ASSERT_TRUE(table.load_data(values));
    EXPECT_EQ(table.data, values);
    EXPECT_NE(table.data.data(), values.data());
}

TEST(TableMoveData, PointerOverloadsStillCopy) {
    double input = 0.5;
    double output = 0.0;
    double values[]{0.0, 1.0, 2.0};
    TableIndependentVariable independent(input);
    InspectableTable dependent(output);
    ASSERT_TRUE(independent.load_data(values, 3));
    ASSERT_TRUE(dependent.load_data(values, SizeVec{1, 3}));
    EXPECT_NE(independent.data.data(), values);
    EXPECT_NE(dependent.stored_data().data(), values);
    values[0] = 100.0;
    EXPECT_DOUBLE_EQ(independent.data.front(), 0.0);
    EXPECT_DOUBLE_EQ(dependent.stored_data().front(), 0.0);
}

TEST(TableMoveData, DependentTakesVectorStorageAndInterpolates) {
    double input = 0.5;
    double output = 0.0;
    TableIndependentVariable independent(input);
    ASSERT_TRUE(independent.load_data(DoubleVec{0.0, 1.0, 2.0}));
    ASSERT_TRUE(independent.initialize());
    InspectableTable table(output);
    table.add_independent(independent);
    DoubleVec values{10.0, 20.0, 30.0};
    const double* const storage = values.data();

    ASSERT_TRUE(table.load_data(std::move(values), SizeVec{1, 3}));
    EXPECT_EQ(table.stored_data().data(), storage);
    ASSERT_TRUE(table.initialize());
    ASSERT_TRUE(table.update());
    EXPECT_DOUBLE_EQ(output, 15.0);
    input = 1.5;
    ASSERT_TRUE(independent.update());
    ASSERT_TRUE(table.update());
    EXPECT_DOUBLE_EQ(output, 25.0);
}

TEST(TableMoveData, DependentSupportsMultipleDimensions) {
    double x = 0.25;
    double y = 0.5;
    double output = 0.0;
    TableIndependentVariable first(x);
    TableIndependentVariable second(y);
    ASSERT_TRUE(first.load_data(DoubleVec{0.0, 1.0}));
    ASSERT_TRUE(second.load_data(DoubleVec{0.0, 1.0}));
    ASSERT_TRUE(first.initialize());
    ASSERT_TRUE(second.initialize());
    InspectableTable table(output);
    table.add_independent(first);
    table.add_independent(second);
    DoubleVec values{0.0, 2.0, 4.0, 6.0};
    const double* const storage = values.data();

    ASSERT_TRUE(table.load_data(std::move(values), SizeVec{1, 2, 2}));
    EXPECT_EQ(table.stored_data().data(), storage);
    ASSERT_TRUE(table.initialize());
    ASSERT_TRUE(table.update());
    EXPECT_DOUBLE_EQ(output, 2.0); // 4*x + 2*y
}

TEST(TableMoveData, DependentRejectsWithoutConsumingInput) {
    const std::vector<SizeVec> dimensions{{}, {1}, {2, 3}, {1, 0}, {1, 4}};
    for (const auto& shape : dimensions) {
        double output = 0.0;
        InspectableTable table(output);
        DoubleVec values{10.0, 20.0, 30.0};
        const DoubleVec expected = values;
        const double* const storage = values.data();
        CMLMessage::Mock messages;
        EXPECT_CALL(messages, publish(CMLMessage::Error, testing::_, testing::_, testing::_))
            .Times(testing::AtLeast(1));
        EXPECT_FALSE(table.load_data(std::move(values), shape));
        EXPECT_EQ(values, expected);
        EXPECT_EQ(values.data(), storage);
        EXPECT_FALSE(table.is_data_loaded());
        // The existing copy overload must reject the same dimensions and size.
        InspectableTable copy_table(output);
        EXPECT_FALSE(copy_table.load_data(expected, shape));
        EXPECT_FALSE(copy_table.is_data_loaded());
    }
}

TEST(TableMoveData, DependentLvalueStillCopies) {
    double output = 0.0;
    InspectableTable table(output);
    const DoubleVec values{10.0, 20.0};
    ASSERT_TRUE(table.load_data(values, SizeVec{1, 2}));
    EXPECT_EQ(table.stored_data(), values);
    EXPECT_NE(table.stored_data().data(), values.data());
}

TEST(TableMoveData, DependentReloadMovesReplacementStorage) {
    double output = 0.0;
    InspectableTable table(output);
    ASSERT_TRUE(table.load_data(DoubleVec{10.0, 20.0}, SizeVec{1, 2}));
    DoubleVec replacement{30.0, 40.0, 50.0};
    const double* const storage = replacement.data();
    CMLMessage::Mock messages;
    EXPECT_CALL(messages, publish(CMLMessage::Warning, testing::_, testing::_, testing::_));
    ASSERT_TRUE(table.load_data(std::move(replacement), SizeVec{1, 3}));
    EXPECT_EQ(table.stored_data().data(), storage);
    EXPECT_EQ(table.get_data_size(), 3U);
}

TEST(TableMoveData, SingleInputTableInheritsMoveLoading) {
    double input = 0.5;
    double output = 0.0;
    TableIndependentVariable independent(input);
    ASSERT_TRUE(independent.load_data(DoubleVec{0.0, 1.0}));
    ASSERT_TRUE(independent.initialize());
    GenericSingleInputTable table(output);
    table.add_independent(independent);
    ASSERT_TRUE(table.load_data(DoubleVec{20.0, 40.0}, SizeVec{1, 2}));
    ASSERT_TRUE(table.initialize());
    ASSERT_TRUE(table.update());
    EXPECT_DOUBLE_EQ(output, 30.0);
}

} // namespace
