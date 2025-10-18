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
 * @brief Retrieve console color for a log level.
 *
 * @param logLevel LogLevel
 * @return LogColor Corresponding console color
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
 * @brief Convert LogColor to ANSI escape code for POSIX terminals.
 *
 * @param logColor LogColor value
 * @return std::string ANSI escape code
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
 * @brief Convert LogColor to Windows console attribute.
 *
 * @param logColor LogColor value
 * @return WORD Windows console attribute
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
