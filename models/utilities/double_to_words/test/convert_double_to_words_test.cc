#include "../include/convert_double_to_words.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Common interface for testing all three APIs for the double to words model.
void test_double_to_words(
    const double value,
    const double resolution,
    const unsigned int bit_size,
    const std::vector<uint32_t>& expected) {
    using testing::Eq;
    using testing::Pointwise;

    constexpr unsigned int word_count = 4U;
    uint32_t word_array[word_count];
    std::vector<uint32_t> word_vector(word_count);

    convert_double_to_uint_words(value, resolution, word_vector, bit_size);
    convert_double_to_uint_words(value, resolution, word_array, std::size(word_array), bit_size);
    ConvertDoubleToUintWords article(value, resolution, word_count, bit_size);
    article.update();

    EXPECT_THAT(word_vector, Pointwise(Eq(), expected));
    EXPECT_THAT(word_array, Pointwise(Eq(), expected));
    EXPECT_THAT(article.words, Pointwise(Eq(), expected));
    EXPECT_EQ(article.words.size(), word_count);
}

// Test various invalid configuration options.
TEST(ConvertDoubleToUintWords, InvalidConfiguration) {
    using testing::_;
    using testing::HasSubstr;

    CMLMessage::Mock cml_message_mock;

    // Passing an invalid pointer to the free function interface.
    EXPECT_CALL(
        cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("words array was passed in as NULL")));
    convert_double_to_uint_words(1.0, 1e-3, nullptr, 2, 8);

    // Invalid word count.
    {
        constexpr double value = 0.001;
        constexpr double resolution = 0.001;
        constexpr unsigned int word_count = 0U;
        constexpr unsigned int bit_size = 16U;
        EXPECT_CALL(
            cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("will generate 0 words")));
        const ConvertDoubleToUintWords article(value, resolution, word_count, bit_size);
        EXPECT_EQ(article.words.size(), 0);
    }

    // Invalid bit size.
    {
        constexpr double value = 0.001;
        constexpr double resolution = 0.001;
        constexpr unsigned int word_count = 2U;
        constexpr unsigned int bit_size = 64U;
        EXPECT_CALL(
            cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("bit_size cannot be > 32")));
        ConvertDoubleToUintWords(value, resolution, word_count, bit_size);
    }

    // Value to convert is too large.
    {
        constexpr double value = 0.001;
        constexpr double resolution = 1e-200;
        constexpr unsigned int word_count = 2U;
        constexpr unsigned int bit_size = 16U;
        ConvertDoubleToUintWords article(value, resolution, word_count, bit_size);
        EXPECT_CALL(
            cml_message_mock,
            publish(CMLMessage::Inform, _, _, HasSubstr("too large to be represented")));
        article.update();

        // All bits should be set to 1. With 16 bits, expecting 16^4 - 1 = 65535.
        ASSERT_EQ(article.words.size(), 2);
        ASSERT_EQ(article.words[0], 65535);
        ASSERT_EQ(article.words[1], 65535);
    }

    // Value to convert is too small.
    {
        constexpr double value = 0.3;
        constexpr double resolution = 1.0;
        constexpr unsigned int word_count = 2U;
        constexpr unsigned int bit_size = 16U;
        ConvertDoubleToUintWords article(value, resolution, word_count, bit_size);
        EXPECT_FALSE(article.check_values());
        ASSERT_EQ(article.words[0], 0);
        ASSERT_EQ(article.words[1], 0);
    }

    // Resolution is finer than the separation between double values.
    {
        constexpr double value = 9007199260000000.0;
        constexpr double resolution = 1.0;
        constexpr unsigned int word_count = 4U;
        constexpr unsigned int bit_size = 16U;
        ConvertDoubleToUintWords article(value, resolution, word_count, bit_size);
        EXPECT_CALL(
            cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Resolution of words is lower than the resolution of double")));
        article.check_values();
    }
}

// Test the double to unsigned integer conversion with values that do not require round off
// correction.
TEST(ConvertDoubleToUintWords, NoRoundoff) {
    // Case 1
    // 0.001 with a resolution of 0.001. Max word value = 16^4 = 65536.
    // Significance values:
    //     Index 3: 0.001 * 65536^3 = 281474976710.656
    //     Index 2: 0.001 * 65536^2 = 4294967.296
    //     Index 1: 0.001 * 65536^1 = 65.536
    //     Index 0: 0.001 * 65536^0 = 0.001
    //
    //     Word 3 = 0.001 / 281474976710656.0 = 0; remainder = 0.001
    //     Word 2 = 0.001 / 4294967296.0      = 0; remainder = 0.001
    //     Word 1 = 0.001 / 65536.0           = 0; remainder = 0.001
    //     Word 0 = 0.001 / 1.0               = 1; remainder = 0.000
    test_double_to_words(0.001, 0.001, 4U, {1U, 0U, 0U, 0U});

    // Case 2
    // 5.002 with a resolution of 0.001. Max word value = 4^4 = 16.
    // Significance values:
    //     Index 3: 0.001 * 16^3 = 4.096
    //     Index 2: 0.001 * 16^2 = 0.256
    //     Index 1: 0.001 * 16^1 = 0.016
    //     Index 0: 0.001 * 16^0 = 0.001
    //
    //     Word 3 = 5.002 / 4.096 = 1;  remainder = 0.906
    //     Word 2 = 0.906 / 0.256 = 3;  remainder = 0.138
    //     Word 1 = 0.138 / 0.016 = 8;  remainder = 0.010
    //     Word 0 = 0.010 / 0.001 = 10; remainder = 0.000
    test_double_to_words(5.002, 0.001, 4U, {10U, 8U, 3U, 1U});

    // Case 3
    // 5.001 with a resolution of 0.001. Max word value = 4^4 = 16.
    // Significance values:
    //     Index 3: 0.001 * 16^3 = 4.096
    //     Index 2: 0.001 * 16^2 = 0.256
    //     Index 1: 0.001 * 16^1 = 0.016
    //     Index 0: 0.001 * 16^0 = 0.001
    //
    //     Word 3 = 5.001 / 4.096 = 1; remainder = 0.905
    //     Word 2 = 0.905 / 0.256 = 3; remainder = 0.137
    //     Word 1 = 0.137 / 0.016 = 8; remainder = 0.009
    //     Word 0 = 0.009 / 0.001 = 9; remainder = 0.000
    test_double_to_words(5.001, 0.001, 4U, {9U, 8U, 3U, 1U});

    // Case 4
    // 123456.0 with a resolution of 1.0. Max word value = 16^4 = 65536.
    // Significance values:
    //     Index 3: 1.0 * 65536^3 = 281474976710656.0
    //     Index 2: 1.0 * 65536^2 = 4294967296.0
    //     Index 1: 1.0 * 65536^1 = 65536.0
    //     Index 0: 1.0 * 65536^0 = 1.0
    //
    //     Word 3 = 123456.0 / 281474976710656.0 = 0;     remainder = 123456.0
    //     Word 2 = 123456.0 / 4294967296.0      = 0;     remainder = 123456.0
    //     Word 1 = 123456.0 / 65536.0           = 1;     remainder = 57920.0
    //     Word 0 = 57920.0  / 1.0               = 57920; remainder = 0.0
    test_double_to_words(123456.0, 1.0, 16U, {57920U, 1U, 0U, 0U});

    // Case 5
    // 123457.0 with a resolution of 1.0. Max word value = 16^4 = 65536.
    // Significance values:
    //     Index 3: 1.0 * 65536^3 = 281474976710656.0
    //     Index 2: 1.0 * 65536^2 = 4294967296.0
    //     Index 1: 1.0 * 65536^1 = 65536.0
    //     Index 0: 1.0 * 65536^0 = 1.0
    //
    //     Word 3 = 123457.0 / 281474976710656.0 = 0;     remainder = 123457.0
    //     Word 2 = 123457.0 / 4294967296.0      = 0;     remainder = 123457.0
    //     Word 1 = 123457.0 / 65536.0           = 1;     remainder = 57921.0
    //     Word 0 = 57921.0  / 1.0               = 57921; remainder = 0.0
    test_double_to_words(123457.0, 1.0, 16U, {57921U, 1U, 0U, 0U});

    // Case 6
    // 123458.0 with a resolution of 1.0. Max word value = 16^4 = 65536.
    // Significance values:
    //     Index 3: 1.0 * 65536^3 = 281474976710656.0
    //     Index 2: 1.0 * 65536^2 = 4294967296.0
    //     Index 1: 1.0 * 65536^1 = 65536.0
    //     Index 0: 1.0 * 65536^0 = 1.0
    //
    //     Word 3 = 123458.0 / 281474976710656.0 = 0;     remainder = 123458.0
    //     Word 2 = 123458.0 / 4294967296.0      = 0;     remainder = 123458.0
    //     Word 1 = 123458.0 / 65536.0           = 1;     remainder = 57922.0
    //     Word 0 = 57922.0  / 1.0               = 57922; remainder = 0.0
    test_double_to_words(123458.0, 1.0, 16U, {57922U, 1U, 0U, 0U});

    // Case 7
    // 900987654321.181 with a resolution of 0.001. Max word value = 16^4 = 65536.
    // Significance values:
    //     Index 3: 0.001 * 65536^3 = 281474976710.656
    //     Index 2: 0.001 * 65536^2 = 4294967.296
    //     Index 1: 0.001 * 65536^1 = 65.536
    //     Index 0: 0.001 * 65536^0 = 0.001
    //
    //     Word 3 = 900987654321.181  / 281474976710.656 = 3;     remainder = 56562724189.21301
    //     Word 2 = 56562724189.21301 / 4294967.296      = 13169; remainder = 2299868.18901062
    //     Word 1 = 2299868.18901062  / 65.536           = 35093; remainder = 13.34101061988622
    //     Word 0 = 13.34101061988622 / 0.001            = 13341; remainder = 1.061988621842147e-05
    //
    // Remainder is less than half the resolution, so no round-off correction.
    test_double_to_words(900987654321.181, 0.001, 16U, {13341U, 35093U, 13169U, 3U});
}

// Test cases where we need to correct for round off error.
TEST(ConvertDoubleToUintWords, RoundoffCorrection) {
    // Case 1
    // 0.001553 with a resolution of 0.0001. Max word value = 4^4 = 16
    // Significance values:
    //     Index 3: 0.0001 * 16^3 = 0.4096
    //     Index 2: 0.0001 * 16^2 = 0.0256
    //     Index 1: 0.0001 * 16^1 = 0.0016
    //     Index 0: 0.0001 * 16^0 = 0.0001
    //
    //     Word 3 = 0.003153 / 0.4096 = 0;  remainder = 0.003153
    //     Word 2 = 0.003153 / 0.0256 = 0;  remainder = 0.003153
    //     Word 1 = 0.003153 / 0.0016 = 1;  remainder = 0.001553
    //     Word 0 = 0.001553 / 0.0001 = 15; remainder = 0.000053
    //
    // Since there's a remainder greater than half the resolution, we increment
    // word 0, spilling over to 0 and incrementing word 1 by 1, rounding to the
    // closer value that loses less precision.
    test_double_to_words(0.003153, 0.0001, 4U, {0U, 2U, 0U, 0U});

    // Case 2
    // 12287.8 with a resolution of 1.0. Max word value = 4^4 = 16
    // Significance values:
    //     Index 3: 1.0 * 16^3 = 4096
    //     Index 2: 1.0 * 16^2 = 256
    //     Index 1: 1.0 * 16^1 = 16
    //     Index 0: 1.0 * 16^0 = 1
    //
    //     Word 3 = 12287.8 / 4096 = 2;  remainder = 4095.8
    //     Word 2 = 4095.8 / 256   = 15; remainder = 255.8
    //     Word 1 = 255.8 / 16     = 15; remainder = 15.8
    //     Word 0 = 15.8 / 1       = 15; remainder = 0.8
    //
    // Since there's a remainder greater than half the resolution, we increment
    // word 0, starting a chain reaction of spillovers until we reach word 3,
    // which is incremented by 1.
    test_double_to_words(12287.8, 1.0, 4U, {0U, 0U, 0U, 3U});

    // Case 3
    // 900987654321.182 with a resolution of 0.001. Max word value = 16^4 = 65536.
    // Significance values:
    //     Index 3: 0.001 * 65536^3 = 281474976710.656
    //     Index 2: 0.001 * 65536^2 = 4294967.296
    //     Index 1: 0.001 * 65536^1 = 65.536
    //     Index 0: 0.001 * 65536^0 = 0.001
    //
    //     Word 3 = 900987654321.182   / 281474976710.656 = 3;     remainder = 56562724189.21399
    //     Word 2 = 56562724189.21399  / 4294967.296      = 13169; remainder = 2299868.1899871826
    //     Word 1 = 2299868.1899871826 / 65.536           = 35093; remainder = 13.34198718238622
    //     Word 0 = 13.34198718238622  / 0.001            = 13341; remainder = 0.0009871823862184215
    //
    // Since there's a remainder greater than half the resolution, we increment
    // word 0 from 13341 to 13342.
    test_double_to_words(900987654321.182, 0.001, 16U, {13342U, 35093U, 13169U, 3U});
}

} // namespace
