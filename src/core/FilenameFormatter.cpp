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
    std::string Format(const std::string& templateStr)
    {
        // Get current time
        std::time_t now = std::time(nullptr);
        std::tm localTime{};
#if defined(_WIN32)
        localtime_s(&localTime, &now);
#else
        localtime_r(&now, &localTime);
#endif

        // Build replacements
        std::ostringstream year, month, day, hour, minute, second;
        year << std::setw(4) << std::setfill('0') << (1900 + localTime.tm_year);
        month << std::setw(2) << std::setfill('0') << (localTime.tm_mon + 1);
        day << std::setw(2) << std::setfill('0') << localTime.tm_mday;
        hour << std::setw(2) << std::setfill('0') << localTime.tm_hour;
        minute << std::setw(2) << std::setfill('0') << localTime.tm_min;
        second << std::setw(2) << std::setfill('0') << localTime.tm_sec;

        // Replace placeholders manually (no regex to avoid performance overhead)
        std::string result = templateStr;
        auto replace = [&](const std::string& key, const std::string& value)
        {
            size_t pos = 0;
            while ((pos = result.find(key, pos)) != std::string::npos)
            {
                result.replace(pos, key.size(), value);
                pos += value.size();
            }
        };

        replace("{year}", year.str());
        replace("{month}", month.str());
        replace("{day}", day.str());
        replace("{hour}", hour.str());
        replace("{minute}", minute.str());
        replace("{second}", second.str());

        return result;
    }
}
