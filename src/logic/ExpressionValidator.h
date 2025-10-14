/* ----- ----- ----- ----- */
// ExpressionValidator.h
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/01
// Version: v1.0
/* ----- ----- ----- ----- */

#ifndef EXPRESSION_VALIDATOR_H
#define EXPRESSION_VALIDATOR_H

#include <optional>
#include <string>
#include <unordered_set>

/// Check if the mathematical expression is valid, and if the values ​​on both sides of the equal sign are equal.
/// @param s The input string (e.g. "12+46=58")
/// @param n Length limit (Must be equal to "s.size()")
/// @param validOps Set of valid operators (e.g. "{ '+', '-', '*', '/', '^' }")
/// @return If this a valid and correct mathematical expression
class ExpressionValidator {
private:
    std::unordered_set<char> validOps;

public:
    ExpressionValidator() = default;
    void setValidOps(const std::unordered_set<char>& ops) { validOps = ops; }

    long long evalExpr(const std::string& s); // Use internal validOps
    static std::optional<long long> safeEval(const std::string& expr);
    bool isValidExpression(const std::string& s, int n);
};

#endif // EXPRESSION_VALIDATOR_H
