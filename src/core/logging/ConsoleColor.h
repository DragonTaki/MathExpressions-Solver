/* ----- ----- ----- ----- */
// ConsoleColor.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>

#include "LogColor.h"
#include "LogLevel.h"

#ifdef _WIN32
#include <Windows.h>
#endif

/**
 * @namespace ConsoleColor
 * @brief Provides functions for handling console text color conversions.
 *
 * <summary>
 * The ConsoleColor namespace defines helper functions that map log levels or 
 * LogColor values to actual console display colors.  
 * It supports both ANSI-based color formatting (for POSIX systems) and Windows 
 * console attributes, ensuring cross-platform color consistency for log output.
 * </summary>
 *
 * <remarks>
 * These functions are primarily used internally by logging components (such as 
 * AppLogger or LogFileManager) to apply color-coded messages based on severity.  
 * On Windows systems, `ToWindowsAttr()` should be used with 
 * `SetConsoleTextAttribute()` from the Windows API.  
 * On Linux/macOS, `ToAnsi()` returns the appropriate escape sequence for color output.
 * </remarks>
 *
 * <example>
 * ```cpp
 * // Example: Using colorized output for logs
 * LogColor color = ConsoleColor::GetLevelColor(LogLevel::Warn);
 * std::cout << ConsoleColor::ToAnsi(color) << "[Warning] Disk space low" << "\033[0m" << std::endl;
 * ```
 * </example>
 */
namespace ConsoleColor {

    /**
     * @brief Retrieves the default console color associated with a given log level.
     *
     * <summary>
     * Maps log severity levels (Trace, Debug, Info, Warn, Error) to corresponding 
     * display colors for consistent and intuitive visual feedback.
     * </summary>
     *
     * <details>
     * Example mapping:
     * - Trace, Debug → Gray  
     * - Info → White  
     * - Warn → Yellow  
     * - Error → Red  
     * - Default → System default color  
     * </details>
     *
     * @param level The log level whose color should be retrieved.
     * @return The LogColor corresponding to the specified LogLevel.
     */
    LogColor GetLevelColor(LogLevel level);

    /**
     * @brief Converts a LogColor value into an ANSI escape sequence for POSIX terminals.
     *
     * <summary>
     * Generates the ANSI escape code used to set terminal text color for output.  
     * These codes work in Linux, macOS, and other ANSI-compatible environments.
     * </summary>
     *
     * <remarks>
     * ANSI color codes are not natively supported in Windows Command Prompt prior to 
     * Windows 10 v1511, unless ANSI support is explicitly enabled.
     * </remarks>
     *
     * @param color The LogColor value to convert.
     * @return A string containing the corresponding ANSI escape sequence.
     *
     * <example>
     * ```cpp
     * std::cout << ConsoleColor::ToAnsi(LogColor::Green)
     *           << "Operation successful"
     *           << "\033[0m" << std::endl;
     * ```
     * </example>
     */
    std::string ToAnsi(LogColor color);

#ifdef _WIN32
    /**
     * @brief Converts a LogColor to its corresponding Windows console color attribute.
     *
     * <summary>
     * Returns the Windows console attribute value (a bitmask) that represents
     * the specified color. This can be used with the Win32 API function 
     * `SetConsoleTextAttribute()` to change text color.
     * </summary>
     *
     * <remarks>
     * Each color is composed of one or more of the following flags:  
     * FOREGROUND_RED, FOREGROUND_GREEN, FOREGROUND_BLUE, and FOREGROUND_INTENSITY.
     * </remarks>
     *
     * @param color The LogColor value to convert.
     * @return A WORD representing the corresponding Windows console attribute.
     *
     * <example>
     * ```cpp
     * HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     * SetConsoleTextAttribute(hConsole, ConsoleColor::ToWindowsAttr(LogColor::Cyan));
     * std::cout << "Cyan-colored message" << std::endl;
     * SetConsoleTextAttribute(hConsole, ConsoleColor::ToWindowsAttr(LogColor::Default));
     * ```
     * </example>
     */
    WORD ToWindowsAttr(LogColor color);
#endif

}
