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

class RoundManager {
public:
    const std::unordered_set<char>& getOperatorsSet() const {
        return gameRoundState.operatorsSet;
    }

    void initializeRound(int exprLength, const std::unordered_set<char>& operatorsSet);
    void resetRound();
    void resetGame();

    bool startRound();
    bool nextRoundInput();

    bool rollback();

    /**
     * @brief Configure the validator with the current round's allowed operators.
     */
    void configureValidator() {
        validator.setValidOps(gameRoundState.operatorsSet);
    }

    /**
     * @brief Check whether a given expression string is valid under current rules.
     * @param exprLine The input expression to validate.
     * @return true if the expression is valid; otherwise false.
     */
    bool isValidExpression(const std::string& exprLine) {
        configureValidator();
        return validator.isValidExpression(exprLine, gameRoundState.exprLength);
    }

private:
    GameRoundState gameRoundState;  ///< Whole game data
    ExpressionValidator validator;  ///< 專用的表達式驗證器

    InputExpressionSpec specReader;
    InputExpressionLine exprReader;

    std::unordered_map<char, Constraint> constraintsMap;  ///< Current constraints
    std::vector<std::string> currentCandidatesList;       ///< Current filtered candidates
};
