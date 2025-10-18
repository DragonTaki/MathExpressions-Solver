/* ----- ----- ----- ----- */
// LogColor.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>

#include "LogLevel.h"

/**
 * @enum LogColor
 * @brief Represents the available colors for console log output.
 *
 * <summary>
 * This enum defines the colors that can be used for console logging.
 * It is used internally to map log levels to colors for easy readability.
 * </summary>
 */
enum class LogColor {
    Default,  ///< Reset to console default color
    Gray,     ///< Gray text, often for debug/trace
    Red,      ///< Red text, typically for errors
    Yellow,   ///< Yellow text, typically for warnings
    Green,    ///< Green text, optional informational
    Cyan,     ///< Cyan text, optional informational
    Blue,     ///< Blue text, optional informational
    Magenta,  ///< Magenta text, optional informational
    White     ///< Default white text
};

namespace ConsoleAnsi {
    inline constexpr const char* RESET   = "\033[0m";
    inline constexpr const char* GRAY    = "\033[90m";
    inline constexpr const char* RED     = "\033[31m";
    inline constexpr const char* GREEN   = "\033[32m";
    inline constexpr const char* YELLOW  = "\033[33m";
    inline constexpr const char* BLUE    = "\033[34m";
    inline constexpr const char* MAGENTA = "\033[35m";
    inline constexpr const char* CYAN    = "\033[36m";
    inline constexpr const char* WHITE   = "\033[37m";
}
