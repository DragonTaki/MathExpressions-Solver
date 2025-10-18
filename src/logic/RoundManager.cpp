/* ----- ----- ----- ----- */
// RoundManager.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/17
// Update Date: 2025/10/17
// Version: v1.0
/* ----- ----- ----- ----- */

#include "RoundManager.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CandidateGenerator.h"
#include "Constraint.h"
#include "ExpressionValidator.h"
#include "GameRoundState.h"
#include "core/input/InputExpressionLine.h"
#include "core/input/InputExpressionSpec.h"
#include "core/logging/AppLogger.h"
#include "util/ConsoleUtils.h"

void RoundManager::initializeRound(
    int exprLength,
    const std::unordered_set<char>& operatorsSet
) {
    gameRoundState.resetRoundData(); // 清空回合資料
    gameRoundState.exprLength = exprLength;
    gameRoundState.operatorsSet = operatorsSet;

    constraintsMap.clear();
    currentCandidatesList.clear();

    AppLogger::Debug("Initialized new round.");
}

void RoundManager::resetRound() {
    gameRoundState.resetRoundData();
    constraintsMap.clear();
    currentCandidatesList.clear();

    AppLogger::Info("Round has been reset.");
}

void RoundManager::resetGame() {
    gameRoundState.resetGameData();
    constraintsMap.clear();
    currentCandidatesList.clear();

    AppLogger::Info("Game has been fully reset.");
}

// Input first round: length & operators, first expression, first color feedback
bool RoundManager::startRound() {
    try {
        int exprLength;
        std::unordered_set<char> operatorsSet;
    
        // Error handling & Read expression length and available operators
        if (!specReader.readExpressionSpec(exprLength, operatorsSet))
            throw std::runtime_error("Failed to read length and operators.");

        // 初始化回合狀態
        initializeRound(exprLength, operatorsSet);

        validator.setValidOps(gameRoundState.operatorsSet);

        // First time read expression and color feedback
        RoundRecord firstRound;
        AppLogger::Prompt("Input expression (or 'end' to finish, 'undo' to rollback) (e.g. \"12 + 46 = 58\", or \"12+46=58\"):", LogColor::Yellow);
        if (!exprReader.readExpression(firstRound.exprLine, gameRoundState.exprLength, validator))
            throw std::runtime_error("Failed to read first expression.");

        AppLogger::Prompt("Input color feedback (e.g. \"r y r y y g r r\", or \"ryryygrr\"): ", LogColor::Yellow);
        if (!exprReader.readColorFeedback(firstRound.exprColorLine, gameRoundState.exprLength))
            throw std::runtime_error("Failed to read first color feedback.");

        // Save to roundHistory
        gameRoundState.roundHistory.push_back(firstRound);

        // 更新 constraints
        updateConstraint(constraintsMap, firstRound.exprLine, firstRound.exprColorLine);

        // Generate initial candidates
        CandidateGenerator generator(validator);
        gameRoundState.initialCandidatesList = generator.generate(
            gameRoundState.exprLength,
            gameRoundState.operatorsSet,
            {firstRound.exprLine},
            {firstRound.exprColorLine},
            constraintsMap
        );

        currentCandidatesList = gameRoundState.initialCandidatesList;

        // Print results
        if (currentCandidatesList.empty())
            AppLogger::Prompt("No solution.", LogColor::Red);
        else {
            ConsoleUtils::printCandidatesInline(currentCandidatesList);
        }

        return true;
    } catch (const std::exception& e) {
        AppLogger::Error(std::format("startRound failed: {}", e.what()));
        return false;
    }
}

// Subsequent input, filtering from existing candidates
bool RoundManager::nextRoundInput() {
    // Error handling: Empty candidates (should not be here)
    if (currentCandidatesList.empty()) return false;

    std::string exprLine;       ///< Following expression guess, or "end" for end whole round
    std::string exprColorLine;  ///< The color pairing the expression

    AppLogger::Prompt("----- ----- ----- -----", LogColor::Gray);

    if (!InputExpressionLine::readPlayerInput(exprLine, exprColorLine, gameRoundState.exprLength, validator)) {
        if (exprLine == "undo") {
            rollback();
            return true;
        }
        if (exprLine == "end") return false;
        return false;  // 若輸入錯誤或EOF
    }
    
    // Update constraint
    bool isUpdated = updateConstraint(constraintsMap, exprLine, exprColorLine);
    if (isUpdated) {
        printConstraint(constraintsMap);
    }

    // Save current round to RoundRecord
    RoundRecord newRound{exprLine, exprColorLine};
    gameRoundState.roundHistory.push_back(newRound);

    // Filter candidates
    currentCandidatesList = validator.filterExpressions(currentCandidatesList, constraintsMap);

    // Show filtered results
    std::cout << "--- Filtered candidates ---\n";
    if (currentCandidatesList.empty())
        AppLogger::Prompt("No solution.", LogColor::Red);
    else
        ConsoleUtils::printCandidatesInline(currentCandidatesList);

    return true;
}

bool RoundManager::rollback()
{
    if (gameRoundState.roundHistory.empty()) {
        AppLogger::Prompt("No previous round to rollback.", LogColor::Red);
        return false;
    }

    // 移除最後一回合
    gameRoundState.roundHistory.pop_back();
    AppLogger::Info("Rolled back one round.");

    // 重建 constraint 與候選
    constraintsMap.clear();

    currentCandidatesList = validator.filterExpressions(gameRoundState.initialCandidatesList, constraintsMap);

    // 顯示結果
    printConstraint(constraintsMap);
    ConsoleUtils::printCandidatesInline(currentCandidatesList);

    return true;
}
