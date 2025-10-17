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
 * @brief Removes all spaces from the given string.
 *
 * <summary>
 * This is a simple utility function that creates a copy of the input string
 * with all space characters (' ') removed. It uses std::remove and std::string::erase
 * to efficiently remove spaces in a single pass.
 * </summary>
 *
 * @param s Input string that may contain spaces.
 * @return A new std::string with all spaces removed.
 *
 * @example
 * std::string text = "Hello World!";
 * std::string result = removeSpaces(text); // result == "HelloWorld!"
 */
inline std::string removeSpaces(const std::string& s) {
    std::string res = s;  ///< Make a copy of the input string
    // std::remove shifts all non-space characters forward and returns new logical end
    res.erase(std::remove(res.begin(), res.end(), ' '), res.end());
    return res;           ///< Return the string with spaces removed
}
