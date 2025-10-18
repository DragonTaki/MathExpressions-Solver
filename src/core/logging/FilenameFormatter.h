/* ----- ----- ----- ----- */
// FilenameFormatter.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/13
// Update Date: 2025/10/13
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>

/// <summary>
/// Provides utilities for generating filenames with dynamic time-based placeholders.
/// </summary>
/// <remarks>
/// Supported placeholders:
/// - {year}   : 4-digit year
/// - {month}  : 2-digit month (01-12)
/// - {day}    : 2-digit day (01-31)
/// - {hour}   : 2-digit hour (00-23)
/// - {minute} : 2-digit minute (00-59)
/// - {second} : 2-digit second (00-59)
/// </remarks>
namespace FilenameFormatter {
    
/**
 * @brief Formats a filename template by replacing time placeholders with current local time.
 *
 * <summary>
 * This function takes a filename template string containing placeholders
 * (e.g., "log_{year}-{month}-{day}_{hour}.{minute}.{second}.txt")
 * and replaces them with the corresponding current local time values.
 * </summary>
 *
 * @param templateStr A filename template containing placeholders.
 * @return A formatted filename string with all placeholders replaced by current local time values.
 *
 * @example
 * std::string filename = FilenameFormatter::Format("log_{year}-{month}-{day}.txt");
 * // Result: "log_2025-10-13.txt" (depending on current date)
 */
std::string Format(const std::string& templateStr);

} // namespace (end of Expression)
