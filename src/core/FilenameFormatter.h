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
/// Provides utilities to format filenames with dynamic placeholders.
/// Supported placeholders:
/// {year}, {month}, {day}, {hour}, {minute}, {second}
/// </summary>
namespace FilenameFormatter
{
    /// <summary>
    /// Formats a filename template string by replacing placeholders
    /// (e.g., "{year}{month}{day}") with the current local time values.
    /// </summary>
    /// <param name="templateStr">Filename template (e.g., "log_{year}-{month}-{day}.txt")</param>
    /// <returns>Formatted filename string with replaced time values.</returns>
    std::string Format(const std::string& templateStr);
}
