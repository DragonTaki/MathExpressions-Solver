/* ----- ----- ----- ----- */
// ConsoleUtils.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/16
// Update Date: 2025/10/16
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <vector>

/**
 * @file ConsoleUtils.h
 * @brief Utilities for console output formatting and dimension querying.
 *
 * <summary>
 * Provides helper functions to query the console width and print a list of
 * candidate strings in a neatly aligned, inline table format. These functions
 * handle platform differences between Windows and Unix-like systems.
 * </summary>
 */
namespace ConsoleUtils {

/**
 * @brief Retrieves the current console width in characters.
 *
 * <summary>
 * `getConsoleWidth` returns the width of the terminal window in number of characters.
 * It handles platform-specific calls:
 * - On Windows, it uses `GetConsoleScreenBufferInfo`.
 * - On Unix-like systems, it uses `ioctl` with `TIOCGWINSZ`.
 * If the width cannot be determined, a default fallback of 80 characters is returned.
 *
 * @return int Console width in characters.
 *
 * @example
 * @code
 * int width = ConsoleUtils::getConsoleWidth();
 * fmt::print("Console width is {}\n", width);
 * @endcode
 */
int getConsoleWidth();

/**
 * @brief Prints a vector of candidate strings in multiple columns within the console.
 *
 * <summary>
 * `printCandidatesInline` formats a list of candidates into columns to fit
 * within the current console width. The number of columns is automatically
 * calculated based on the console width and the length of the candidate strings.
 * If the list is empty, the function does nothing.
 *
 * Features:
 * - Dynamically calculates number of columns based on console width.
 * - Aligns each column using `fmt::print` for consistent spacing.
 * - Pads empty cells in the last row to maintain alignment.
 *
 * @param candidates Vector of strings representing candidate entries to print.
 *
 * @example
 * @code
 * std::vector<std::string> candidates = {"12+34", "56-7", "8*9"};
 * ConsoleUtils::printCandidatesInline(candidates);
 * @endcode
 */
void printCandidatesInline(const std::vector<std::string>& candidates);

} // namespace (end of ConsoleUtils)
