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

/**
 * @brief Determines if a given character is allowed to appear in the current expression
 *        based on its defined constraints.
 *
 * This function checks whether a specific character can still be used, according to
 * its `minCount`, `maxCount`, and current `usedCount`. A character is forbidden if:
 * - It does not exist in the `constraintsMap`.
 * - It has both `minCount` and `maxCount` equal to 0 (completely forbidden).
 * - Its current usage has reached or exceeded the maximum allowed count.
 *
 * @param exprChar The character being tested.
 * @param constraintsMap A mapping of characters to their corresponding constraints.
 * @return `true` if the character can be used; `false` otherwise.
 */
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


/**
 * @brief Determines if a character can appear at a specific position in the expression.
 *
 * Checks whether the given character is banned from appearing in a specific index,
 * as determined by its constraint’s banned position set (e.g., due to 'y' or 'r' feedback).
 *
 * @param exprChar The character being checked.
 * @param position The position index within the expression.
 * @param constraintsMap A map of character constraints.
 * @return `true` if the character can appear at this position; `false` otherwise.
 */
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

/**
 * @brief Validates the structural integrity of a single token.
 *
 * Checks digit tokens for validity:
 * - Must not be empty.
 * - Cannot start with '0' (unless it’s a single '0').
 * - Must contain only digits.
 *
 * Operators are not validated here; they are handled elsewhere.
 *
 * @param token The token to validate.
 * @return `true` if the token is valid; `false` otherwise.
 */
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

/**
 * @brief Validates a sequence of tokens for syntactic correctness.
 *
 * Ensures that the sequence of tokens forms a potentially valid mathematical expression.
 * The rules enforced include:
 * - Expression cannot start with an operator.
 * - No consecutive operators are allowed.
 * - Consecutive exponentiation ('^') operators are invalid.
 * - Division by zero or standalone '0' tokens are invalid.
 *
 * @param tokensList The sequence of tokens representing part of an expression.
 * @return `true` if the token sequence is syntactically valid; `false` otherwise.
 */
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

        // Single '0' cannot be a token ("+0" / "-0" / "*0" / "^0" are invalid expressions)
        if (lastToken.value == "0")
            return false;
    }

    return true;
}

/**
 * @brief Ensures a character does not conflict with other characters’ fixed green positions.
 *
 * This function prevents a situation where two characters are both assigned
 * to the same green position (fixed correct position). It scans all constraints
 * and denies placement if another symbol is already "green" at the target position.
 *
 * @param exprChar The character being tested.
 * @param position The position index in the expression.
 * @param constraintsMap A map of constraints for all characters.
 * @return `true` if no conflicts are found; `false` otherwise.
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
            // Another character is already fixed at this position
            return false;
        }
    }

    return true;
}

/**
 * @brief Validates a complete expression candidate based on constraint rules.
 *
 * Performs a multi-level validation of an entire expression string against
 * defined symbol constraints. This includes:
 * - Character-level validation (existence and usage limits).
 * - Position-level validation (banned positions).
 * - Conflict checking for fixed positions (green overlaps).
 * - Final count matching with min/max occurrence rules.
 *
 * @param exprLine The full expression candidate string.
 * @param constraintsMap Map containing all constraints for each symbol.
 * @return `true` if the candidate satisfies all constraint conditions; `false` otherwise.
 */
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

    // Check if appearance count matches constraints
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

}  // namespace (end of ConstraintUtils)
