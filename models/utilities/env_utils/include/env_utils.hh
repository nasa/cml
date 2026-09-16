/*******************************************************************************
PURPOSE: (Tools for reading environment variables.)

LIBRARY DEPENDENCIES: ()

PROGRAMMERS:
  (
   ((Nino Tarantino) (NASA) (09/16/26) (Use C++ interfaces))
  )
*******************************************************************************/
#ifndef CML_ENV_UTILS_HH
#define CML_ENV_UTILS_HH

#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

/**
 * Retrieve the environment variable if it exists, otherwise terminate the program
 *
 * @param var_name Environment variable to retrieve
 * @param exit_fn  Function to be called if the variable is not set
 * @return         The contents of the environment variable
 */
inline std::string getenv_or_exit(
    const std::string& var_name,
    const std::function<void(int)>& exit_fn = std::exit)
{
    const char* value = std::getenv(var_name.c_str());
    if (value == nullptr) {
        std::cerr << "Error: Environment variable \"" << var_name << "\" is not set.\n";
        exit_fn(EXIT_FAILURE);
        return "";
    }
    return value;
}

/**
 * Retrieve the environment variable if it exists, otherwise return a user-defined default value
 *
 * @param var_name      Environment variable to retrieve
 * @param default_value Value to return if the variable is not set
 * @return              The contents of the environment variable if set, otherwise the default value
 */
inline std::string getenv_or_default(const std::string& var_name, const std::string& default_value)
{
    const char* value = std::getenv(var_name.c_str());
    if (value == nullptr) {
        return default_value;
    }
    return value;
}

/**
 * Retrieve the environment variable if it exists, otherwise throw an error
 *
 * @throws std::runtime_error If the environment variable is not set
 * @param var_name            Environment variable to retrieve
 * @return                    The contents of the environment variable
 */
inline std::string getenv_or_throw(const std::string& var_name) noexcept(false)
{
    const char* value = std::getenv(var_name.c_str());
    if (value == nullptr) {
        throw std::runtime_error("Error: Environment variable \"" + var_name + "\" is not set.");
    }
    return value;
}

#endif
