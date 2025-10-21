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
 * @brief General-purpose utility functions for string manipulation and text normalization.
 *
 * <summary>
 * This header defines small, reusable utility functions that perform common
 * text processing tasks. These helpers are intentionally lightweight and do
 * not depend on any external libraries beyond the C++ standard library.
 * </summary>
 *
 * <remarks>
 * The `Utils` namespace serves as a shared repository of frequently used
 * operations such as trimming, normalization, and basic string transformations.
 * Each function is implemented inline to ensure zero runtime overhead
 * and optimal performance for short utility calls.
 * </remarks>
 *
 * <example>
 * Example usage:
 * @code
 * #include "Utils.h"
 *
 * std::string text = "  Hello World! ";
 * std::string cleaned = Utils::removeSpaces(text);  // "HelloWorld!"
 * std::string lowered = Utils::toLower(cleaned);    // "helloworld!"
 * @endcode
 * </example>
 */
namespace Utils {

/**
 * @brief Removes all spaces from the given string.
 *
 * <summary>
 * The `removeSpaces` function creates a copy of the input string and removes
 * all space characters (' ') from it. The original string remains unchanged.
 *
 * Internally, it uses `std::remove` from `<algorithm>` to shift all non-space
 * characters toward the beginning, followed by `std::string::erase` to trim
 * the redundant tail region. This method performs efficiently in a single pass.
 * </summary>
 *
 * <remarks>
 * - Only ASCII space `' '` characters are removed; tabs (`'\t'`) and other
 *   whitespace characters remain intact.
 * - The function does not modify the input reference, ensuring immutability.
 * </remarks>
 *
 * @param string The input string that may contain spaces.
 * @return std::string A new string with all spaces removed.
 *
 * @example
 * @code
 * std::string text = "A B C";
 * std::string result = Utils::removeSpaces(text); // result == "ABC"
 * @endcode
 */
inline std::string removeSpaces(const std::string& string) {
    std::string result = string;  ///< Make a copy of the input string to preserve original
    // Remove spaces by shifting non-space characters forward
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    return result;                ///< Return the string with spaces removed
}

/**
 * @brief Converts all alphabetic characters in the string to lowercase.
 *
 * <summary>
 * The `toLower` function returns a lowercase-transformed copy of the input string.
 * It uses `std::transform` and `std::tolower` to process each character efficiently.
 * </summary>
 *
 * <remarks>
 * - Only affects alphabetic characters (`A-Z`).  
 * - Non-alphabetic characters remain unchanged.  
 * - The operation is locale-independent (uses the default C locale).  
 * </remarks>
 *
 * @param string The input string to convert.
 * @return std::string A lowercase version of the input string.
 *
 * @example
 * @code
 * std::string text = "HeLLo World!";
 * std::string lower = Utils::toLower(text); // "hello world!"
 * @endcode
 */
inline std::string toLower(const std::string& string) {
    std::string result = string;  ///< Make a copy of the input string to preserve original
    // Convert to lowercase by iterating through all characters
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){ return std::tolower(c); });
    return result;                ///< Return the string with all characters converted to lowercase
}

}  // namespace (end of Utils)
