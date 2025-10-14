
/* ----- ----- ----- ----- */
// Utils.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/01
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <algorithm>

inline std::string removeSpaces(const std::string& s) {
    std::string res = s;
    res.erase(std::remove(res.begin(), res.end(), ' '), res.end());
    return res;
}
