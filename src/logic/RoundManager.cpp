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
#include "core/input/InputUtils.h"
#include "core/logging/AppLogger.h"
#include "util/ConsoleUtils.h"

/**
 * @brief Initializes a new round while keeping existing game configuration.
 *
 * This function clears all per-round data (candidates, constraints, etc.)
 * while retaining the specified expression length and operator set.
 * It is typically used when starting a new round within the same game.
 *
 * @param exprLength The length of the expression for the round.
 * @param operatorsSet The set of available operator characters (e.g. '+', '-', '*', '/').
 */
void RoundManager::initializeRound(
    int exprLength,
    const std::unordered_set<char>& operatorsSet
) {
    gameRoundState.resetRoundData();  // Clear round data (keep configuration)
    gameRoundState.exprLength = exprLength;
    gameRoundState.operatorsSet = operatorsSet;

    constraintsMap.clear();
    currentCandidatesList.clear();

    AppLogger::Debug("Initialized new round.");
}

/**
 * @brief Resets only the current round data without altering game configuration.
 *
 * This function clears the round history, constraint map, and candidate list.
 * It is useful for restarting the same round parameters without modifying global settings.
 */
void RoundManager::resetRound() {
    gameRoundState.resetRoundData();
    constraintsMap.clear();
    currentCandidatesList.clear();

    AppLogger::Info("Round has been reset.");
}

/**
 * @brief Performs a full reset of the game, clearing all data and configuration.
 *
 * This function resets the game state completely, including expression length,
 * operators, constraints, and candidate list.
 */
void RoundManager::resetGame() {
    gameRoundState.resetGameData();
    constraintsMap.clear();
    currentCandidatesList.clear();

    AppLogger::Info("Game has been fully reset.");
}

/**
 * @brief Reads the player's input for both the expression and its feedback colors.
 *
 * This function prompts the user to input an expression and corresponding
 * color feedback, supporting special commands like `"undo"` (rollback previous round)
 * and `"end"` (finish the current session).
 *
 * @param[out] exprLine The user-entered expression string (e.g., `"12+46=58"`).
 * @param[out] exprColorLine The user-entered color feedback string (e.g., `"ryryygrr"`).
 * @return `true` if input was successfully read; `false` if the user chose to end input or an error occurred.
 */
bool RoundManager::readPlayerInput(std::string& exprLine, std::string& exprColorLine) {
    int roundIndex = static_cast<int>(gameRoundState.roundHistory.size());

    std::string exprPrompt;
    std::string colorPrompt;

    AppLogger::Prompt("----- ----- ----- -----", LogColor::Gray);
    if (roundIndex == 0) {
        exprPrompt = std::format(
            "Round {} - Input expression (or 'end' to finish, 'undo' to rollback)\n"
            "Example: 12+46=58 (or 12 + 46 = 58)\n"
            "Your input: ", roundIndex + 1);

        colorPrompt = std::format(
            "Round {} - Input color feedback\n"
            "Example: ryryygrr (or r y r y y g r r)\n"
            "Your input: ", roundIndex + 1);
    } else {
        exprPrompt = std::format("Round {} - Input expression: ", roundIndex + 1);
        colorPrompt = std::format("Round {} - Input color feedback: ", roundIndex + 1);
    }

    /**
     * @brief Lambda function that handles special text commands during expression input.
     *
     * Recognizes `"undo"` (rollback previous round) and `"end"` (terminate input).
     */
    auto handleExprSpecial = [this](const std::string& command) -> SpecialCommandResult {
        if (command == "undo") {
            // Perform rollback, continue waiting for input afterward
            bool ok = this->rollback();
            return SpecialCommandResult::HANDLED_CONTINUE;
        }
        if (command == "end") {
            // Stop accepting input for this round
            return SpecialCommandResult::HANDLED_STOP_INPUT;
        }
        return SpecialCommandResult::NOT_SPECIAL;
    };

    /**
     * @brief Lambda function that handles special commands during color feedback input.
     *
     * Allows `"undo"` and `"end"` to function similarly to expression input stage.
     */
    auto handleColorSpecial = [this](const std::string& command) -> SpecialCommandResult {
        if (command == "undo") {
            bool ok = this->rollback();
            return SpecialCommandResult::HANDLED_CONTINUE;
        }
        if (command == "end") {
            return SpecialCommandResult::HANDLED_STOP_INPUT;
        }
        return SpecialCommandResult::NOT_SPECIAL;
    };

    // Read expression and color feedback using input readers
    if (!exprReader.readExpression(exprLine, gameRoundState.exprLength, validator, handleExprSpecial, exprPrompt))
        return false;

    if (!exprReader.readColorFeedback(exprColorLine, gameRoundState.exprLength, handleColorSpecial, colorPrompt))
        return false;

    return true;
}

/**
 * @brief Processes a single round of input and updates constraints and candidates accordingly.
 *
 * This method handles:
 *  - Reading player input (expression and color feedback)
 *  - Updating constraint maps
 *  - Regenerating or filtering candidate expressions
 *  - Logging and console output
 *
 * @return `true` if the round was processed successfully; `false` if user ended input or an exception occurred.
 */
bool RoundManager::processRoundInput() {
    try {
        RoundRecord currentRound;
    
        // Check if expression spec is required
        if (gameRoundState.exprLength == 0) {
            int exprLength;
            std::unordered_set<char> operatorsSet;

            if (!specReader.readExpressionSpec(exprLength, operatorsSet))
                throw std::runtime_error("Failed to read length and operators.");

            initializeRound(exprLength, operatorsSet);
            validator.setValidOps(gameRoundState.operatorsSet);
        }

        // Read expression and feedback input
        if (!readPlayerInput(currentRound.exprLine, currentRound.exprColorLine))
            return false;  // User requested to end / undo handled in callback

        // Save this round's data to history
        gameRoundState.roundHistory.push_back(currentRound);

        // Update constraint map using current feedback
        updateConstraint(constraintsMap, currentRound.exprLine, currentRound.exprColorLine);

        // Generate or filter candidate list
        bool firstRoundInput = (gameRoundState.roundHistory.size() == 1);
        if (firstRoundInput) {
            CandidateGenerator generator(validator);
            gameRoundState.initialCandidatesList = generator.generate(
                gameRoundState.exprLength,
                gameRoundState.operatorsSet,
                {currentRound.exprLine},
                {currentRound.exprColorLine},
                constraintsMap
            );
            currentCandidatesList = gameRoundState.initialCandidatesList;
        } else {
            currentCandidatesList = validator.filterExpressions(currentCandidatesList, constraintsMap);
        }

        // Print result candidates
        if (currentCandidatesList.empty())
            AppLogger::Prompt("No solution.", LogColor::Red);
        else {
            ConsoleUtils::printCandidatesInline(currentCandidatesList);
        }

        return true;
    } catch (const std::exception& e) {
        AppLogger::Error(std::format("processRoundInput failed: {}", e.what()));
        return false;
    }
}

/**
 * @brief Rolls back the game state by removing the most recent round.
 *
 * This function removes the last round record from history, rebuilds the constraint
 * map from all remaining previous rounds, and recalculates the candidate list.
 * It is primarily triggered by the `"undo"` command from user input.
 *
 * @return `true` if rollback succeeded; `false` if there was no round to rollback.
 */
bool RoundManager::rollback()
{
    if (gameRoundState.roundHistory.empty()) {
        AppLogger::Prompt("No previous round to rollback.", LogColor::Red);
        return false;
    }

    // Remove the most recent round record
    gameRoundState.roundHistory.pop_back();
    AppLogger::Prompt("Rolled back one round.", LogColor::Magenta);

    // Rebuild constraints from remaining rounds
    constraintsMap.clear();
    constraintsMap = initializeConstraintsMap();
    for (auto& record : gameRoundState.roundHistory) {
        updateConstraint(constraintsMap, record.exprLine, record.exprColorLine);
    }

    // Rebuild candidate list
    if (gameRoundState.roundHistory.empty()) {
        // If no previous rounds, revert to initial candidates
        currentCandidatesList = gameRoundState.initialCandidatesList;
    } else {
        // Otherwise filter from the initial candidate pool
        currentCandidatesList = validator.filterExpressions(
            gameRoundState.initialCandidatesList,
            constraintsMap
        );
    }

    // Display current constraint state and filtered candidates
    printConstraint(constraintsMap);
    ConsoleUtils::printCandidatesInline(currentCandidatesList);

    return true;
}
