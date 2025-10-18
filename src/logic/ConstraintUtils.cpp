/* ----- ----- ----- ----- */
// ConstraintUtils.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/16
// Update Date: 2025/10/16
// Version: v1.0
/* ----- ----- ----- ----- */

#include "ConstraintUtils.h"
#include <algorithm>
#include <cctype>
#include <unordered_set>

#include "core/constants/ExpressionConstants.h"
#include "core/constants/ExpressionTokens.h"
#include "core/logging/AppLogger.h"

namespace ConstraintUtils {

// ---------------- Character-level ----------------
bool isCharAllowed(char exprChar, const std::unordered_map<char, Constraint>& constraintsMap) {
    auto constraintIt = constraintsMap.find(exprChar);

    // Error handling, should not happened
    if (constraintIt == constraintsMap.end()) {
        /*AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, passed: not exist in constraints map",
            tokenVecToString(currentTokens), exprChar));*/
        return false;
    }

    const Constraint& constraint = constraintIt->second;

    // Only got 'r' color, not contained in answer
    if (constraint.minCount() == 0 && constraint.maxCount() == 0) {
        /*AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, skipped: forbidden (min=max=0)",
            tokenVecToString(currentTokens), exprChar));*/
        return false;
    }

    // Got some 'g'/'y', and has 'r' to restrict the max use
    if (constraint.usedCount() >= constraint.maxCount()) {
        /*AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, skipped: Reach max usage limit {}",
            tokenVecToString(currentTokens), exprChar, constraint.maxCount()));*/
        return false;
    }

    return true;
}

// ---------------- Position-level ----------------
bool isCharAllowedAtPos(char exprChar, int position, const std::unordered_map<char, Constraint>& constraintsMap) {
    auto constraintIt = constraintsMap.find(exprChar);

    // The character got 'y' or 'r' at this position, means not allowed here
    if (constraintIt != constraintsMap.end()) {
        const Constraint& constraint = constraintIt->second;
        if (constraint.bannedPos().count(position)) {
            /*AppLogger::Trace(fmt::format("[_dfs, now='{}'] Try {}, skipped: Disallowed at pos {}",
                tokenVecToString(currentTokens), exprChar, position));*/
            return false;
        }
    }

    return true;
}

// ---------------- Token-level ----------------
bool isTokenValid(const Expression::Token& token) {
    if (token.type == Expression::TokenType::Digit) {
        const std::string& tokenValue = token.value;

        // Error handling: Token couldn't be empty (shouldn't be here)
        if (tokenValue.empty()) {
            return false;
        }

        // Digit token should not be "0", or start with '0' 
        if (tokenValue[0] == '0') {
            return false;
        }

        // Check if all digits (shouldn't be here)
        for (char c : tokenValue) {
            if (!std::isdigit(static_cast<unsigned char>(c)))
                return false;
        }

        return true;
    }
    // No check for operator so far
    // Single character checking already done in isCharAllowed() and isCharAllowedAtPos()

    return true;
}

// ---------------- Expression-level ----------------
bool isTokenSequenceValid(const std::vector<Expression::Token>& tokensList) {
    if (tokensList.empty()) return false;

    const Expression::Token& lastToken      = tokensList.back();         ///< Last token
    const Expression::Token* previousToken  = tokensList.size() >= 2     ///< Previous token
        ? &tokensList[tokensList.size() - 2] : nullptr;
    const Expression::Token* previous_2Token = tokensList.size() >= 3    ///< Previous' previous token
        ? &tokensList[tokensList.size() - 3] : nullptr;

    // Operator cannot be first character
    if (tokensList.size() == 1 &&
        lastToken.type == Expression::TokenType::Operator)
        return false;
    
    // Operator logic check
    if (lastToken.type == Expression::TokenType::Operator) {

        // Operator follow another operator is not allowed
        if (previousToken &&
            previousToken->type == Expression::TokenType::Operator)
            return false;

        // Consecutive factorials are not allowed
        if (previous_2Token &&
            previous_2Token->value == "^" &&
            lastToken.value == "^")
            return false;
    }
    // Digit logic check
    else {
        // '0' cannot follow '/'
        if (!tokensList.empty() &&
            tokensList.back().value == "/" &&
            lastToken.value == "0")
            return false;
    }

    return true;
}

/**
 * @brief Check if the current position conflicts with any green positions of other characters.
 * 
 * @param exprChar The character at the current position.
 * @param position The current index in the expression.
 * @param constraintsMap Map of character constraints.
 * @return true if no green-position conflict, false if some other character must occupy this position.
 */
bool isCharSafeAtPosition(
    char exprChar,
    int position,
    const std::unordered_map<char, Constraint>& constraintsMap)
{
    for (const auto& [otherChar, constraint] : constraintsMap) {
        if (otherChar == exprChar) continue;

        const auto& greenPositions = constraint.greenPos();
        if (greenPositions.count(position) > 0) {
            // 這個位置已被其他字元綠色固定，當前字元不可用
            return false;
        }
    }

    return true;
}

// ---------------- Candidate Check ----------------
bool isCandidateValid(
    const std::string& exprLine,
    const std::unordered_map<char, Constraint>& constraintsMap
) {
    // Character-level and position-level check
    for (size_t exprCharPosition = 0; exprCharPosition < exprLine.size(); ++exprCharPosition) {
        char exprChar = exprLine[exprCharPosition];

        // If allowed in constraintsMap
        if (!isCharAllowed(exprChar, constraintsMap)) {
            return false;
        }

        // If allowed at the position
        if (!isCharAllowedAtPos(exprChar, static_cast<int>(exprCharPosition), constraintsMap)) {
            return false;
        }

        // If no other symbol occupied the position
        if (!isCharSafeAtPosition(exprChar, exprCharPosition, constraintsMap)) {
            return false;
        }
    }

    // Check if macth min/max constraints
    std::unordered_map<char, int> appearCountMap;
    for (char exprChar : exprLine) {
        appearCountMap[exprChar]++;
    }

    for (const auto& [exprChar, constraint] : constraintsMap) {
        int appearCount = appearCountMap[exprChar];
        if (appearCount < constraint.minCount() || appearCount > constraint.maxCount()) {
            return false;
        }
    }

    return true;
}

} // namespace (end of ConstraintUtils)
