/* ----- ----- ----- ----- */
// ExpressionValidator.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/16
// Version: v1.1
/* ----- ----- ----- ----- */

#pragma once
#include <cmath>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Constraint.h"
#include "ExpressionValidator.h"

/**
 * @class ExpressionValidator
 * @brief Validates mathematical expressions and evaluates them safely.
 *
 * <summary>
 * This class provides methods to:
 * - Evaluate a mathematical expression string (supporting '+', '-', '*', '/', '^').
 * - Check if an expression is valid and if both sides of the equal sign match.
 * - Perform safe evaluation returning std::optional to handle errors gracefully.
 *
 * Supports standard integer arithmetic and exponentiation (^). Division by zero
 * and negative exponents are detected and reported as errors. It also supports
 * multi-digit integers and ensures integer-only division when necessary.
 * </summary>
 */
class ExpressionValidator {
private:
    /**
     * @brief Set of allowed operators for the expression evaluation.
     *
     * This set determines which arithmetic operators are permitted when evaluating
     * expressions. Must be initialized via `setValidOps()` before calling `evalExpr`.
     */
    std::unordered_set<char> ValidOperatorsSet;

public:
    /**
     * @brief Default constructor initializes an empty set of valid operators.
     */
    ExpressionValidator() = default;

    /**
     * @brief Sets the allowed operators for expression evaluation.
     *
     * @param operatorsSet A set of characters representing valid operators
     *                     (e.g., { '+', '-', '*', '/', '^' }).
     *
     * This must be called before evaluating expressions with `evalExpr` 
     * to ensure only permitted operators are used. Operator precedence
     * and associativity are respected during evaluation.
     */
    void setValidOps(const std::unordered_set<char>& operatorsSet) { ValidOperatorsSet = operatorsSet; }

    /**
     * @brief Evaluates a mathematical expression string.
     *
     * @param exprLine Input expression (e.g., "12+46*2").
     * @return double Result of the evaluated expression.
     * @throws std::runtime_error on invalid characters, division by zero,
     *         negative exponents, or malformed expressions.
     *
     * <summary>
     * This method uses the instance's `ValidOperatorsSet` to determine which operators
     * are allowed. It uses the Shunting Yard algorithm to parse infix notation
     * into Reverse Polish Notation (RPN), then evaluates the RPN. Supports
     * multi-digit integers and enforces integer-only division when applicable.
     * </summary>
     */
    double evalExpr(const std::string& exprLine);

    /**
     * @brief Safely evaluates an expression, returning an optional result.
     *
     * @param exprLine Input expression string.
     * @return std::optional<double> Evaluation result if successful,
     *         or std::nullopt if an error occurred.
     *
     * <summary>
     * Wraps `evalExpr` in a try-catch block to prevent exceptions from propagating.
     * This is useful when validating user input or filtering candidate expressions.
     * </summary>
     */
    std::optional<double> safeEval(const std::string& exprLine);

    /**
     * @brief Validates a mathematical expression of a given length.
     *
     * @param exprLine Expression string to validate (e.g., "12+46=58").
     * @param exprLength Required length of the expression.
     * @return true if the expression is valid and both sides of '=' evaluate equally,
     *         false otherwise.
     *
     * <summary>
     * Rules enforced:
     * - Expression must contain exactly one '=' character.
     * - Left and right sides of '=' must both be valid expressions.
     * - Both sides must evaluate to integers and match numerically.
     * - Expression length must match `exprLength`.
     * </summary>
     */
    bool isValidExpression(const std::string& exprLine, int exprLength);

    /**
     * @brief Checks whether a double value is effectively an integer.
     *
     * @param value Value to check.
     * @param epsilon Allowed tolerance for floating-point comparison (default 1e-9).
     * @return true if value is within `epsilon` of an integer.
     *
     * <summary>
     * Used internally to ensure both sides of an expression evaluate to integers.
     * </summary>
     */
    bool isInteger(double value, double epsilon = 1e-9);

    /**
     * @brief Filter candidate expressions according to current constraints.
     *
     * @param candidatesList List of candidate expression strings.
     * @param constraintsMap Mapping from character to Constraint object, representing
     *                       current rules (e.g., known digits, disallowed positions).
     * @return Filtered list of candidates satisfying all constraints.
     *
     * <summary>
     * Iterates through each candidate and checks it against `constraintsMap` using
     * ConstraintUtils::isCandidateValid. Only candidates passing all constraints
     * are returned.
     * </summary>
     */
    std::vector<std::string> filterExpressions(
        const std::vector<std::string>& candidatesList,
        const std::unordered_map<char, Constraint>& constraintsMap
    );
};
