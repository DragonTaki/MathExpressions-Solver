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

/**
 * @namespace ConstraintUtils
 * @brief Provides a collection of validation utility functions used for constraint checking
 *        during mathematical expression generation and verification.
 *
 * This module defines a set of utility functions that operate at different logical levels:
 * - **Character-level**: determines if a symbol can appear based on its constraints.
 * - **Position-level**: validates if a symbol is allowed at a particular position.
 * - **Token-level**: checks whether tokens (numbers/operators) are syntactically valid.
 * - **Expression-level**: ensures that a full expression adheres to global and positional constraints.
 *
 * These utilities are used by candidate expression generators and validators
 * to filter out invalid expressions early in the generation process.
 */
namespace ConstraintUtils {

    /**
     * @brief Checks whether a character is globally allowed under the given constraint map.
     *
     * This function verifies whether a specific character can appear at all in the generated
     * expression based on its constraint record. A character is disallowed if it:
     * - Does not exist in the provided `constraintsMap`.
     * - Has both `minCount` and `maxCount` equal to zero (completely forbidden).
     * - Has already reached its maximum allowed usage count (`usedCount >= maxCount`).
     *
     * @param exprChar The character being checked.
     * @param constraintsMap Map of symbol constraints, where the key is a character and
     *        the value is its associated `Constraint` object.
     * @return `true` if this character is still allowed to be used, `false` otherwise.
     */
bool isCharAllowed(
    char exprChar,
    const std::unordered_map<char, Constraint>& constraintsMap
);

    /**
     * @brief Checks whether a character is allowed to appear at a specific position.
     *
     * Determines if the given character can legally occupy the specified index position
     * in the expression. If the character has been marked as disallowed at that position
     * (e.g., due to feedback from "yellow" or "red" constraints), the function will return false.
     *
     * @param exprChar The character being checked.
     * @param position The index position within the expression (0-based).
     * @param constraintsMap Map containing all symbol constraints.
     * @return `true` if the character is allowed at the given position; `false` otherwise.
     */
bool isCharAllowedAtPos(
    char exprChar,
    int position,
    const std::unordered_map<char, Constraint>& constraintsMap
);

    /**
     * @brief Validates the internal structure and format of a single token.
     *
     * Ensures that a token (either a number or operator) conforms to basic syntactic rules.
     * Specifically:
     * - A numeric token must not be empty.
     * - A numeric token cannot begin with '0' unless it represents a single zero.
     * - All characters in a numeric token must be digits.
     *
     * Operator tokens are not deeply validated here, as their validation occurs in
     * `isTokenSequenceValid()`.
     *
     * @param token The `Expression::Token` object representing a parsed symbol (digit or operator).
     * @return `true` if the token is valid; `false` otherwise.
     */
bool isTokenValid(const Expression::Token& token);

    /**
     * @brief Checks whether a token sequence remains syntactically valid after appending the last token.
     *
     * This function evaluates a sequence of tokens representing a partial or full expression to ensure
     * that it maintains correct syntactic structure. It prevents invalid constructions such as:
     * - Starting an expression with an operator.
     * - Two consecutive operators (e.g., `++`, `/*`).
     * - Repeated exponentiation symbols (`^^`).
     * - Division by zero or standalone `0` as a number.
     *
     * @param tokensList The ordered list of tokens representing the current expression state.
     * @return `true` if the sequence is syntactically valid so far; `false` otherwise.
     */
bool isTokenSequenceValid(const std::vector<Expression::Token>& tokensList);

    /**
     * @brief Checks whether an expression satisfies all defined constraints for every symbol.
     *
     * Performs a comprehensive validation of a candidate expression string. It ensures that:
     * - Each character adheres to global usage and positional constraints.
     * - No symbol appears more than its allowed maximum, nor fewer times than its required minimum.
     * - No two symbols occupy the same fixed "green" position.
     *
     * This function combines both structural and constraint-level checks to confirm
     * that an expression can be considered a valid candidate.
     *
     * @param exprLine The full expression string to be validated.
     * @param constraintsMap Map containing the constraint definitions for all relevant symbols.
     * @return `true` if the expression satisfies all constraints; `false` otherwise.
     */
bool isCandidateValid(
    const std::string& exprLine,
    const std::unordered_map<char, Constraint>& constraintsMap
);

}  // namespace (end of ConstraintUtils)
