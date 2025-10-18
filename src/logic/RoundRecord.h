/* ----- ----- ----- ----- */
// RoundRecord.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <vector>

/**
 * @struct RoundRecord
 * @brief Represents a single player's input and its resulting candidate state.
 */
struct RoundRecord {
    std::string exprLine;       ///< Player-guessed expression
    std::string exprColorLine;  ///< Game-feedback color
};
