/* ----- ----- ----- ----- */
// Utils.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/01
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <algorithm>

/**
 * @file Utils.h
 * @brief General-purpose utility functions for string manipulation and other helpers.
 *
 * <summary>
 * This header provides small, reusable utility functions that do not belong to
 * a specific module but are commonly used throughout the expression solver project.
 * </summary>
 */
namespace Utils {

/**
 * @brief Removes all spaces from the given string.
 *
 * <summary>
 * `removeSpaces` takes an input string and returns a new string with all
 * space characters (' ') removed. It does not modify the original string.
 *
 * Implementation details:
 * 1. A copy of the input string is created.
 * 2. `std::remove` shifts all non-space characters toward the beginning of the string.
 * 3. `std::string::erase` trims the extra characters at the end after `std::remove`.
 * 
 * This approach is efficient and performs the removal in a single pass over the string.
 *
 * @param string Input string that may contain space characters.
 * @return std::string A new string with all spaces removed.
 *
 * @example
 * @code
 * std::string text = "Hello World!";
 * std::string result = Utils::removeSpaces(text); // result == "HelloWorld!"
 * @endcode
 */
inline std::string removeSpaces(const std::string& string) {
    std::string result = string;  ///< Make a copy of the input string to preserve original
    // Remove spaces by shifting non-space characters forward
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    return result;                ///< Return the string with spaces removed
}

} // namespace (end of Utils)
