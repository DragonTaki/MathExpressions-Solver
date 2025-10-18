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

struct GameRoundState {
    int exprLength = 0;                              ///< Expression length for this game
    std::unordered_set<char> operatorsSet;           ///< Avaliable operator(s) for this game
    std::vector<std::string> initialCandidatesList;  ///< Candidate lists after first guess
    std::vector<RoundRecord> roundHistory;           ///< Player-guessed expressions and game-feedback colors for each round
    
    // 清空「整個回合資料，但保留長度與運算子」
    void resetRoundData() {
        initialCandidatesList.clear();
        roundHistory.clear();
    }

    // 完全重置，包括長度與運算子
    void resetGameData() {
        exprLength = 0;
        operatorsSet.clear();
        resetRoundData();
    }
};
