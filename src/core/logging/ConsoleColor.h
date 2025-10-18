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
 * @brief Provides functions to convert LogColor or LogLevel to console output colors.
 */
namespace ConsoleColor {

    /**
     * @brief Retrieve console color for a given log level.
     *
     * @param logLevel LogLevel
     * @return LogColor corresponding to the level
     */
    LogColor GetLevelColor(LogLevel level);

    /**
     * @brief Convert LogColor to ANSI escape code (for UNIX terminals).
     *
     * @param logColor LogColor value
     * @return ANSI escape string
     */
    std::string ToAnsi(LogColor color);

#ifdef _WIN32
    /**
     * @brief Convert LogColor to Windows console attribute.
     *
     * @param logColor LogColor value
     * @return WORD representing Windows console color attribute
     */
    WORD ToWindowsAttr(LogColor color);
#endif

}
