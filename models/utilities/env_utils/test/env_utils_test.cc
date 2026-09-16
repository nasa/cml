#include "../include/env_utils.hh"

#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>

namespace {

// Test usage of the function which exits when the environment variable is not set.
TEST(EnvUtils, Exit) {
    // Variable exists.
    setenv("CML_ENVUTILS_TEST_VAR_EXIT_TEST", "test-value", 1);
    EXPECT_EQ(getenv_or_exit("CML_ENVUTILS_TEST_VAR_EXIT_TEST"), "test-value");

    // Variable doesn't exist.
    unsetenv("DOES_NOT_EXIST_EXIT_TEST");
    bool exit_called = false;
    auto dummy_exit_fn = [&exit_called] ([[maybe_unused]] int ret) -> void {exit_called = true;};
    getenv_or_exit("DOES_NOT_EXIST_EXIT_TEST", dummy_exit_fn);
    EXPECT_TRUE(exit_called);
}

// Test usage of the function which returns a default value when the environment variable
// is not set.
TEST(EnvUtils, DefaultValue) {
    // Variable exists.
    setenv("CML_ENVUTILS_TEST_VAR_DEFAULT_VALUE_TEST", "test-value", 1);
    EXPECT_EQ(getenv_or_default("CML_ENVUTILS_TEST_VAR_DEFAULT_VALUE_TEST", "DEFAULT"), "test-value");

    // Variable doesn't exist.
    unsetenv("DOES_NOT_EXIST_DEFAULT_VALUE_TEST");
    EXPECT_EQ(getenv_or_default("DOES_NOT_EXIST_DEFAULT_VALUE_TEST", "DEFAULT"), "DEFAULT");
}

// Test usage of the function which throws when the environment variable is not set.
TEST(EnvUtils, Throw) {
    // Variable exists.
    setenv("CML_ENVUTILS_TEST_VAR_THROW_TEST", "test-value", 1);
    EXPECT_EQ(getenv_or_throw("CML_ENVUTILS_TEST_VAR_THROW_TEST"), "test-value");

    // Variable doesn't exist.
    unsetenv("DOES_NOT_EXIST_THROW_TEST");
    EXPECT_THROW(getenv_or_throw("DOES_NOT_EXIST_THROW_TEST"), std::runtime_error);
}

} // namespace
