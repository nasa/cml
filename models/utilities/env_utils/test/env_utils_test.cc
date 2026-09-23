#include "../include/env_utils.hh"

#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>
#include <string>

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

// Test the environment variable expansion function for braced variables.
TEST(EnvUtils, ExpandBracedEnvironmentVariables) {
    // Both variables exist.
    setenv("CML_ENVUTILS_TEST_VAR_BRACED1", "blue", 1);
    setenv("CML_ENVUTILS_TEST_VAR_BRACED2", "mouse", 1);
    EXPECT_EQ(
        expand_env_variables("I have a pet ${CML_ENVUTILS_TEST_VAR_BRACED1} ${CML_ENVUTILS_TEST_VAR_BRACED2}"),
        "I have a pet blue mouse");

    // One variable doesn't exist.
    unsetenv("CML_ENVUTILS_TEST_VAR_BRACED1");
    EXPECT_THROW(
        expand_env_variables("I have a pet ${CML_ENVUTILS_TEST_VAR_BRACED1} ${CML_ENVUTILS_TEST_VAR_BRACED2}"),
        std::runtime_error);

    // None of the environment variables exist.
    unsetenv("CML_ENVUTILS_TEST_VAR_BRACED2");
    EXPECT_THROW(
        expand_env_variables("I have a pet ${CML_ENVUTILS_TEST_VAR_BRACED1} ${CML_ENVUTILS_TEST_VAR_BRACED2}"),
        std::runtime_error);
}

// Test the environment variable expansion function for unbraced variables.
TEST(EnvUtils, ExpandUnbracedEnvironmentVariables) {
    // Both variables exist.
    setenv("CML_ENVUTILS_TEST_VAR_BRACED1", "blue", 1);
    setenv("CML_ENVUTILS_TEST_VAR_BRACED2", "mouse", 1);
    EXPECT_EQ(
        expand_env_variables("I have a pet $CML_ENVUTILS_TEST_VAR_BRACED1 $CML_ENVUTILS_TEST_VAR_BRACED2"),
        "I have a pet blue mouse");

    // One variable doesn't exist.
    unsetenv("CML_ENVUTILS_TEST_VAR_BRACED1");
    EXPECT_THROW(
        expand_env_variables("I have a pet $CML_ENVUTILS_TEST_VAR_BRACED1 $CML_ENVUTILS_TEST_VAR_BRACED2"),
        std::runtime_error);

    // None of the environment variables exist.
    unsetenv("CML_ENVUTILS_TEST_VAR_BRACED2");
    EXPECT_THROW(
        expand_env_variables("I have a pet $CML_ENVUTILS_TEST_VAR_BRACED1 $CML_ENVUTILS_TEST_VAR_BRACED2"),
        std::runtime_error);
}

// Test edge cases of the environment variables expansion function.
TEST(EnvUtils, ExpandEnvironmentVariablesEdgeCases) {
    // A string containing no environment variables is returned as-is.
    const std::string no_env_vars = "No environment variables";
    EXPECT_EQ(no_env_vars, expand_env_variables(no_env_vars));

    // Test the regex which determines variable delimiters.
    setenv("CML_ENVUTILS_TEST_VAR_EDGE1", "John", 1);
    setenv("CML_ENVUTILS_TEST_VAR_EDGE2", "son", 2);
    EXPECT_EQ(
        expand_env_variables("$CML_ENVUTILS_TEST_VAR_EDGE1$CML_ENVUTILS_TEST_VAR_EDGE2 Space Center"),
        "Johnson Space Center");
}

} // namespace
