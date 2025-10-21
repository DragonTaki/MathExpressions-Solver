/* ----- ----- ----- ----- */
// RoundManager.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/17
// Update Date: 2025/10/17
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <unordered_map>
#include <vector>

#include "Constraint.h"
#include "ExpressionValidator.h"
#include "GameRoundState.h"
#include "core/input/InputExpressionLine.h"
#include "core/input/InputExpressionSpec.h"

/**
 * @class RoundManager
 * @brief Handles the overall game round logic, including input processing, constraint updates, and candidate filtering.
 *
 * The **RoundManager** serves as the central coordinator for a single gameplay session.  
 * It manages the following responsibilities:
 * - Reading and validating player input (expressions and color feedback)
 * - Maintaining the current round and game state (`GameRoundState`)
 * - Updating and applying constraints (`Constraint`) for symbol validation
 * - Generating or filtering expression candidates
 * - Supporting rollback (undo) and round/game resets
 *
 * The class integrates several components:
 * - `InputExpressionSpec`: Reads game specifications (expression length, operator set)
 * - `InputExpressionLine`: Reads per-round player input and color feedback
 * - `ExpressionValidator`: Validates and filters expressions according to constraints
 */
class RoundManager {
public:
    /**
     * @brief Retrieves the current set of allowed operators for this round.
     * 
     * This function provides read-only access to the operator set used
     * in the current game configuration.
     *
     * @return Constant reference to the unordered set of allowed operators.
     */
    const std::unordered_set<char>& getOperatorsSet() const {
        return gameRoundState.operatorsSet;
    }

    /**
     * @brief Initializes a new round with given parameters.
     *
     * Clears all round data (candidates, constraints, etc.) and sets up the
     * game state with the specified expression length and available operators.
     *
     * @param exprLength The target expression length (number of characters).
     * @param operatorsSet The set of allowed operators for this round.
     */
    void initializeRound(int exprLength, const std::unordered_set<char>& operatorsSet);

    /**
     * @brief Resets only the current round data.
     *
     * This function clears the round history, constraint map, and candidate list,
     * but retains the game configuration such as expression length and operator set.
     */
    void resetRound();

    /**
     * @brief Performs a complete game reset.
     *
     * This function clears all stored data including expression length,
     * operator set, constraints, and candidates, returning the game to its initial state.
     */
    void resetGame();

    /**
     * @brief Reads a player's input expression and corresponding feedback color.
     *
     * Prompts the player for both an expression and a feedback color line.
     * Supports special commands such as `"undo"` and `"end"` during input.
     *
     * @param[out] exprLine Player input expression string (e.g., `"12+46=58"`).
     * @param[out] exprColorLine Feedback color line (e.g., `"ryryygrr"`).
     * @return `true` if valid input was read successfully, or `false` if input ended or failed.
     */
    bool readPlayerInput(std::string& exprLine, std::string& exprColorLine);

    /**
     * @brief Processes a complete player round, updating constraints and generating candidates.
     *
     * Handles the full logic flow for one player round:
     *  - Reads player input.
     *  - Updates constraint map based on feedback.
     *  - Generates or filters expression candidates.
     *  - Logs progress and displays results.
     *
     * @return `true` if round processing succeeded; `false` if input ended or an error occurred.
     */
    bool processRoundInput();

    /**
     * @brief Reverts the game state to the previous round.
     *
     * Removes the latest round record from history and rebuilds the constraint map
     * and candidate list from the remaining rounds.
     *
     * @return `true` if rollback succeeded, `false` if no previous round existed.
     */
    bool rollback();

    /**
     * @brief Configures the internal expression validator using the current round’s operator set.
     *
     * Ensures the validator is aware of the active operator rules before performing validation.
     */
    void configureValidator() {
        validator.setValidOps(gameRoundState.operatorsSet);
    }

    /**
     * @brief Validates an expression under the current round configuration.
     *
     * Ensures the expression conforms to the defined syntax rules and operator constraints.
     *
     * @param exprLine The input expression string to validate.
     * @return `true` if the expression is syntactically valid under current rules, otherwise `false`.
     */
    bool isValidExpression(const std::string& exprLine) {
        configureValidator();
        return validator.isValidExpression(exprLine, gameRoundState.exprLength);
    }

private:
    GameRoundState gameRoundState;   ///< Stores full game and round-related state data
    ExpressionValidator validator;   ///< Validates expressions and filters candidates according to constraints

    InputExpressionSpec specReader;  ///< Handles reading game configuration (expression length, operator set)
    InputExpressionLine exprReader;  ///< Handles reading player expression and color feedback

    std::unordered_map<char, Constraint> constraintsMap;  ///< Active constraint map representing symbol restrictions
    std::vector<std::string> currentCandidatesList;       ///< List of currently filtered expression candidates
};
