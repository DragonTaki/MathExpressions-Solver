/* ----- ----- ----- ----- */
// FilenameFormatter.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/13
// Update Date: 2025/10/13
// Version: v1.0
/* ----- ----- ----- ----- */

#include "FilenameFormatter.h"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace FilenameFormatter
{
    /**
     * @brief Formats a filename template string using current local time.
     *
     * <summary>
     * Retrieves the current local time, constructs zero-padded
     * year, month, day, hour, minute, and second strings,
     * and replaces all corresponding placeholders in the template.
     * </summary>
     *
     * @param templateStr Template string containing placeholders like "{year}", "{month}", etc.
     * @return A string where all placeholders are replaced with current local time values.
     *
     * <remarks>
     * The function does not use regex for performance reasons.
     * Placeholder replacement is performed with a simple loop.
     * </remarks>
     *
     * @example
     * std::string formatted = FilenameFormatter::Format("log_{year}-{month}-{day}.txt");
     * // Might produce: "log_2025-10-13.txt"
     */
    std::string Format(const std::string& templateStr)
    {
        // Get current local time
        std::time_t now = std::time(nullptr);
        std::tm localTime{};
#if defined(_WIN32)
        localtime_s(&localTime, &now);   // Thread-safe on Windows
#else
        localtime_r(&now, &localTime);   // Thread-safe on POSIX
#endif

        // Build zero-padded time components
        std::ostringstream year, month, day, hour, minute, second;
        year << std::setw(4) << std::setfill('0') << (1900 + localTime.tm_year);
        month << std::setw(2) << std::setfill('0') << (localTime.tm_mon + 1);
        day << std::setw(2) << std::setfill('0') << localTime.tm_mday;
        hour << std::setw(2) << std::setfill('0') << localTime.tm_hour;
        minute << std::setw(2) << std::setfill('0') << localTime.tm_min;
        second << std::setw(2) << std::setfill('0') << localTime.tm_sec;

        // Placeholder replacement lambda
        std::string result = templateStr;
        auto replace = [&](const std::string& key, const std::string& value)
        {
            size_t pos = 0;
            while ((pos = result.find(key, pos)) != std::string::npos)
            {
                result.replace(pos, key.size(), value);
                pos += value.size();   // Move past inserted value
            }
        };

        // Replace all placeholders
        replace("{year}", year.str());
        replace("{month}", month.str());
        replace("{day}", day.str());
        replace("{hour}", hour.str());
        replace("{minute}", minute.str());
        replace("{second}", second.str());

        return result;
    }
}
