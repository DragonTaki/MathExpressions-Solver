/* ----- ----- ----- ----- */
// GameRoundState.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/18
// Update Date: 2025/10/18
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <unordered_set>
#include <vector>

#include "RoundRecord.h"

/**
 * @struct GameRoundState
 * @brief Represents the complete runtime state of a single game session, including expression length,
 *        allowed operators, generated candidates, and all historical round data.
 *
 * This structure holds the current state of a mathematical expression puzzle game round.
 * It tracks both configuration data (such as available operators and expression length)
 * and mutable runtime data (such as candidates and guess history).
 *
 * The structure provides convenient functions to reset the round or the entire game state,
 * depending on whether configuration data (expression length and operators) should be preserved.
 */
struct GameRoundState {
    int exprLength = 0;                              ///< The required expression length for this game session
    std::unordered_set<char> operatorsSet;           ///< Avaliable operator(s) for this game
    std::vector<std::string> initialCandidatesList;  ///< Candidate lists after first guess
    std::vector<RoundRecord> roundHistory;           ///< Player-guessed expressions and game-feedback colors for each round
    
    /**
     * @brief Clears all round-related data while keeping the configuration (expression length and operators).
     *
     * This method is typically used when starting a new round within the same game settings.
     * It preserves the expression length and operator set, allowing re-use of the same configuration
     * without having to fully reinitialize the game.
     *
     * @note The `initialCandidatesList` and `roundHistory` are both cleared.
     */
    void resetRoundData() {
        initialCandidatesList.clear();
        roundHistory.clear();
    }

    /**
     * @brief Fully resets the game state, including configuration and round data.
     *
     * This method resets every field in the structure to its default state.
     * It is typically called when starting a brand new game session
     * with new parameters for expression length and available operators.
     *
     * @note This function internally calls `resetRoundData()` to clear round-specific data as well.
     */
    void resetGameData() {
        exprLength = 0;
        operatorsSet.clear();
        resetRoundData();
    }
};
