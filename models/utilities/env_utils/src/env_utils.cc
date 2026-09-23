/*******************************************************************************
PURPOSE: (Tools for reading environment variables.)

LIBRARY DEPENDENCIES: ()

PROGRAMMERS:
    (
     ((Nino Tarantino) (NASA) (09/16/26) (Relocate from Unit Test model))
    )
*******************************************************************************/

#include "../include/env_utils.hh"

#include <cstdlib>
#include <regex>
#include <string>

// Expand all environment variables in a given string
std::string expand_env_variables(const std::string& input) noexcept(false)
{
    std::string result;
    result.reserve(input.size());

    static const std::regex pattern(
        R"(\$\{([A-Za-z_][A-Za-z0-9_]*)\}|\$([A-Za-z_][A-Za-z0-9_]*))");
    const auto begin = std::sregex_iterator(input.begin(), input.end(), pattern);
    const auto end = std::sregex_iterator();

    std::size_t last_position = 0;
    for (auto it = begin; it != end; ++it) {
        const std::smatch& match = *it;

        // Copy text between the previous match and this one.
        const auto match_position = static_cast<std::size_t>(match.position());
        result.append(input, last_position, match_position - last_position);

        // Grab the value stored in the environment variable.
        std::string var_name;
        if (match[1].matched) {
            var_name = match[1].str();
        } else {
            var_name = match[2].str();
        }
        const auto env_var_contents = getenv_or_throw(var_name);

        // Add environment variable contents to result.
        result += env_var_contents;
        last_position = match_position + static_cast<std::size_t>(match.length());
    }

    result.append(input, last_position, input.size() - last_position);
    return result;
}
