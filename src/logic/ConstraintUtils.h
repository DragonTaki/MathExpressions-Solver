/* ----- ----- ----- ----- */
// ConstraintUtils.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/16
// Update Date: 2025/10/16
// Version: v1.0
/* ----- ----- ----- ----- */

#pragma once
#include <string>
#include <unordered_map>

#include "Constraint.h"
#include "core/constants/ExpressionTokens.h"

namespace ConstraintUtils {

/**
 * @brief Checks whether a character is globally allowed under the given constraint map.
 * @param exprChar The character being checked.
 * @param constraintsMap Constraint collection.
 * @return True if this character can be used at all.
 */
bool isCharAllowed(
    char exprChar,
    const std::unordered_map<char, Constraint>& constraintsMap
);

/**
 * @brief Checks whether a character is allowed to appear at a specific position.
 * @param exprChar The character being checked.
 * @param position The position in expression.
 * @param constraintsMap Constraint collection.
 * @return True if allowed at that position.
 */
bool isCharAllowedAtPos(
    char exprChar,
    int position,
    const std::unordered_map<char, Constraint>& constraintsMap
);

/**
 * @brief Checks whether a token (digit or operator) itself is valid.
 */
bool isTokenValid(const Expression::Token& token);

/**
 * @brief Checks whether a token sequence remains syntactically valid after appending the last token.
 */
bool isTokenSequenceValid(const std::vector<Expression::Token>& tokensList);

/**
 * @brief Checks whether an expression satisfies min/max constraints for all symbols.
 */
bool isCandidateValid(
    const std::string& exprLine,
    const std::unordered_map<char, Constraint>& constraintsMap
);

} // namespace (end of ConstraintUtils)
