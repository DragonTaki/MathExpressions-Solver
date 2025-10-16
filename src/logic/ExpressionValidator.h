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
#include <unordered_set>
#include <vector>

/**
 * @class ExpressionValidator
 * @brief Validates mathematical expressions and evaluates them safely.
 *
 * This class provides methods to:
 * - Evaluate a mathematical expression string (supporting '+', '-', '*', '/', '^').
 * - Check if an expression is valid and if both sides of the equal sign match.
 * - Perform safe evaluation returning std::optional to handle errors gracefully.
 *
 * Supports standard integer arithmetic and exponentiation (^). Division by zero
 * and negative exponents are detected and reported as errors.
 */
class ExpressionValidator {
private:
    /**
     * @brief Set of allowed operators for the expression evaluation.
     */
    std::unordered_set<char> validOps;

public:
    /**
     * @brief Default constructor initializes an empty set of valid operators.
     */
    ExpressionValidator() = default;
    
    /**
     * @brief Sets the allowed operators for expression evaluation.
     * @param ops A set of characters representing valid operators
     *            (e.g., { '+', '-', '*', '/', '^' }).
     *
     * This must be called before evaluating expressions with `evalExpr` 
     * to ensure only permitted operators are used.
     */
    void setValidOps(const std::unordered_set<char>& ops) { validOps = ops; }

    /**
     * @brief Evaluates a mathematical expression string.
     * @param s Input expression (e.g., "12+46*2").
     * @return Result of the evaluated expression as `double`.
     * @throws std::runtime_error on invalid characters, division by zero,
     *         negative exponents, or malformed expressions.
     *
     * This method uses the instance's `validOps` to determine which operators
     * are allowed. Operator precedence and associativity are respected.
     */
    double evalExpr(const std::string& s);
    
    /**
     * @brief Safely evaluates an expression, returning an optional result.
     * @param expr Input expression string.
     * @return `std::optional<double>` containing the evaluation result if successful,
     *         or `std::nullopt` if an error occurred.
     *
     * This static method catches all exceptions from `evalExpr` and prevents
     * runtime crashes from invalid or malformed expressions.
     */
    static std::optional<double> safeEval(const std::string& expr);

    /**
     * @brief Validates a mathematical expression of a given length.
     * @param s Expression string to validate (e.g., "12+46=58").
     * @param n Required length of the expression.
     * @return `true` if the expression is valid and both sides of '=' evaluate equally,
     *         `false` otherwise.
     *
     * Rules enforced:
     * - Expression must contain exactly one '=' character.
     * - Left and right sides of '=' must both be valid expressions.
     * - Expression length must match `n`.
     */
    bool isValidExpression(const std::string& s, int n);

    bool isInteger(double val, double epsilon = 1e-9);

    std::vector<std::string> filterExpressions(
        const std::vector<std::string>& candidates,
        const std::string& guess,
        const std::string& color
    );
};
