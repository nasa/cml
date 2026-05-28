#ifndef ENV_UTILS_H
#define ENV_UTILS_H

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

// Function that exits if getenv returns NULL
inline const char* getenv_or_exit(const char* var_name, void (*exit_fn)(int) = std::exit)
{
    const char* value = std::getenv(var_name);
    if (!value) 
    {
        std::cerr << "Error: Environment variable " << var_name << " is not set.\n";
        exit_fn(EXIT_FAILURE);
    }
    return value;
}

inline const char* getenv_or_default(const char* var_name, const char* default_value) 
{
    const char* value = std::getenv(var_name);
    return value != nullptr ? value : default_value;
}

inline const char* getenv_or_throw(const char* var_name) 
{
    if (!var_name)
        throw std::runtime_error("Invalid parameter: var_name is nullptr, 0 or NULL");
    const char* value = std::getenv(var_name);
    if (!value) 
    {
        throw std::runtime_error("Error: Environment variable \"" + std::string(var_name) + "\" is not set.");
    }
    return value;
}

#endif // ENV_UTILS_H
