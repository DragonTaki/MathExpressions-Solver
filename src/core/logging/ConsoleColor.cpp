/* ----- ----- ----- ----- */
// ConsoleColor.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#include "ConsoleColor.h"

#include "LogColor.h"
#include "LogLevel.h"

namespace ConsoleColor {

/**
 * @brief Retrieves the default console color corresponding to a given log level.
 *
 * <summary>
 * This function maps each LogLevel to a predefined LogColor for consistent
 * console output formatting. It ensures that log messages of different
 * severity levels are displayed with appropriate visual distinction.
 * </summary>
 *
 * <details>
 * Example color mapping:
 * - Trace, Debug → Gray  
 * - Info → White  
 * - Warn → Yellow  
 * - Error → Red  
 * - Default / Unknown → Default (system default color)
 * </details>
 *
 * @param logLevel The log level whose color should be retrieved.
 * @return The LogColor corresponding to the specified LogLevel.
 */
LogColor GetLevelColor(LogLevel logLevel) {
    switch (logLevel) {
        case LogLevel::Trace: return LogColor::Gray;
        case LogLevel::Debug: return LogColor::Gray;
        case LogLevel::Info:  return LogColor::White;
        case LogLevel::Warn:  return LogColor::Yellow;
        case LogLevel::Error: return LogColor::Red;
        default:              return LogColor::Default;
    }
}

/**
 * @brief Converts a LogColor value to its corresponding ANSI escape code for POSIX terminals.
 *
 * <summary>
 * ANSI escape codes are used in Unix-like systems (Linux, macOS, etc.)
 * to colorize terminal text output. This function translates LogColor values
 * into appropriate ANSI sequences for visual display.
 * </summary>
 *
 * <remarks>
 * These codes are ignored on Windows terminals unless ANSI support is explicitly enabled.
 * </remarks>
 *
 * @param logColor The LogColor to convert.
 * @return A string containing the ANSI escape sequence for the specified color.
 *
 * <example>
 * std::cout << ConsoleColor::ToAnsi(LogColor::Green) << "Success!" << "\033[0m";
 * </example>
 */
std::string ToAnsi(LogColor logColor) {
    switch (logColor) {
        case LogColor::Gray:    return "\033[90m";
        case LogColor::Red:     return "\033[31m";
        case LogColor::Yellow:  return "\033[33m";
        case LogColor::Green:   return "\033[32m";
        case LogColor::Cyan:    return "\033[36m";
        case LogColor::Blue:    return "\033[34m";
        case LogColor::Magenta: return "\033[35m";
        case LogColor::White:   return "\033[37m";
        default:                return "\033[0m";
    }
}

#ifdef _WIN32
/**
 * @brief Converts a LogColor value into a Windows console attribute constant.
 *
 * <summary>
 * This function maps LogColor values to Windows API console attributes,
 * which are used with SetConsoleTextAttribute() to change text color.
 * </summary>
 *
 * <remarks>
 * Works only on Windows platforms.  
 * The FOREGROUND_* constants define RGB color components and intensity.
 * </remarks>
 *
 * @param logColor The LogColor to convert.
 * @return A WORD value representing the Windows console text color attribute.
 *
 * <example>
 * HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
 * SetConsoleTextAttribute(hConsole, ConsoleColor::ToWindowsAttr(LogColor::Yellow));
 * std::cout << "Warning message" << std::endl;
 * </example>
 */
WORD ToWindowsAttr(LogColor logColor) {
    switch (logColor) {
        case LogColor::Gray:    return FOREGROUND_INTENSITY;
        case LogColor::Red:     return FOREGROUND_RED | FOREGROUND_INTENSITY;
        case LogColor::Yellow:  return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case LogColor::Green:   return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case LogColor::Cyan:    return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case LogColor::Blue:    return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case LogColor::Magenta: return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case LogColor::White:   return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        default:                return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}
#endif

}  // namespace (end of ConsoleColor)
