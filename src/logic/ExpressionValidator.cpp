/* ----- ----- ----- ----- */
// ExpressionValidator.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/16
// Version: v1.1
/* ----- ----- ----- ----- */

#include "ExpressionValidator.h"
#include <algorithm>
#include <optional>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Constraint.h"
#include "ConstraintUtils.h"

namespace {
    /**
     * @brief Get operator precedence.
     *
     * <summary>
     * Defines standard arithmetic operator precedence:
     * '^' > '*' '/' > '+' '-'.
     * </summary>
     *
     * @param op Operator character ('+', '-', '*', '/', '^')
     * @return int Precedence level (higher = higher priority)
     */
    int precedence(char op) {
        if (op == '^') return 3;
        if (op == '*' || op == '/') return 2;
        if (op == '+' || op == '-') return 1;
        return 0;
    }

    /**
     * @brief Checks whether an operator is left-associative.
     *
     * <summary>
     * In this evaluator, all operators are left-associative
     * except for '^' which is right-associative.
     * </summary>
     *
     * @param op Operator character
     * @return true if left-associative; false if right-associative
     */
    bool isLeftAssociative(char op) {
        return (op != '^'); // '^' is right-associative
    }

    /**
     * @brief Applies a binary operator to two operands.
     *
     * @param a Left-hand operand
     * @param b Right-hand operand
     * @param op Operator character ('+', '-', '*', '/', '^')
     * @return double Result of the operation
     * @throws runtime_error On invalid operator, division by zero, or negative exponent
     */
    double applyOp(double a, double b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/':
                if (b == 0) throw std::runtime_error("Division by zero");
                return a / b;
            case '^': {
                if (b < 0) throw std::runtime_error("Negative exponent not supported");
                double res = 1;
                for (int i = 0; i < b; i++) res *= a;
                return res;
            }
        }
        throw std::runtime_error("Invalid operator");
    }
} // anonymous namespace

// ---- Public API ----
// ---- Shunting Yard + RPN Evaluate ----

/**
 * @brief Evaluates an arithmetic expression string and returns the result.
 *
 * <summary>
 * This function uses the Shunting Yard algorithm to convert infix expressions
 * to Reverse Polish Notation (RPN) and then evaluates the RPN.
 *
 * Supports operators: +, -, *, /, ^ (exponentiation)
 * Supports multi-digit integers.
 *
 * @param exprLine Expression string (e.g., "12+3*4")
 * @return double Evaluation result
 * @throws runtime_error If expression contains invalid characters, malformed RPN, or illegal operations
 */
double ExpressionValidator::evalExpr(const std::string& exprLine) {
    std::vector<std::string> output;   // RPN output queue
    std::stack<char> opsStack;    // Operator stack
    std::string num;              // Buffer for multi-digit numbers

    auto flushNum = [&]() {
        if (!num.empty()) {
            output.push_back(num);
            num.clear();
        }
    };

    // --- Shunting Yard Algorithm ---
    for (char c : exprLine) {
        if (isdigit(c)) {
            num.push_back(c);
        } else if (ValidOperatorsSet.count(c)) {
            flushNum();
            while (!opsStack.empty() && ValidOperatorsSet.count(opsStack.top())) {
                char top = opsStack.top();
                if ((isLeftAssociative(c) && precedence(c) <= precedence(top)) ||
                    (!isLeftAssociative(c) && precedence(c) < precedence(top))) {
                    output.push_back(std::string(1, top));
                    opsStack.pop();
                } else break;
            }
            opsStack.push(c);
        } else {
            throw std::runtime_error("Invalid character in expression");
        }
    }
    flushNum();
    while (!opsStack.empty()) {
        output.push_back(std::string(1, opsStack.top()));
        opsStack.pop();
    }

    // --- RPN Evaluation ---
    std::stack<double> st;
    for (auto& token : output) {
        if (isdigit(token[0])) {
            st.push(stoll(token));
        } else {
            if (st.size() < 2) throw std::runtime_error("Malformed expression");
            double b = st.top(); st.pop();
            double a = st.top(); st.pop();
            st.push(applyOp(a, b, token[0]));
        }
    }
    if (st.size() != 1) throw std::runtime_error("Malformed RPN eval");
    return st.top();
}

/**
 * @brief Safely evaluates an expression and returns optional result.
 *
 * <summary>
 * Wraps evalExpr in try-catch to prevent exceptions from propagating.
 * Returns std::nullopt if evaluation fails.
 * </summary>
 *
 * @param exprLine Expression string
 * @return std::optional<double> Evaluation result if successful; std::nullopt otherwise
 */
std::optional<double> ExpressionValidator::safeEval(const std::string& exprLine) {
    try {
        ExpressionValidator validator;
        double result = validator.evalExpr(exprLine);
        return result;
    } catch (...) {
        return std::nullopt;
    }
}

/**
 * @brief Checks whether a string is a valid arithmetic equation.
 *
 * <summary>
 * Validates if the string:
 * 1. Has exactly 'n' characters
 * 2. Contains exactly one '='
 * 3. Has non-empty left and right sides
 * 4. Left and right sides evaluate to the same value
 * </summary>
 *
 * @param exprLine Expression string (e.g., "12+35=47")
 * @param exprLength Expected length of expression
 * @return true if valid equation; false otherwise
 */
bool ExpressionValidator::isValidExpression(const std::string& exprLine, int exprLength) {
    if ((int)exprLine.size() != exprLength) return false;

    size_t eqSignPosition = exprLine.find('=');
    // If '=' not exist, or existed but at the string end
    if (eqSignPosition == std::string::npos || exprLine.find('=', eqSignPosition + 1) != std::string::npos)
        return false;

    std::string lhsExprLine = exprLine.substr(0, eqSignPosition);
    std::string rhsExprLine = exprLine.substr(eqSignPosition + 1);
    if (lhsExprLine.empty() || rhsExprLine.empty()) return false;

    try {
        double lhsResult = evalExpr(lhsExprLine);
        double rhsResult = evalExpr(rhsExprLine);
        return lhsResult == rhsResult;
    } catch (...) {
        return false;
    }
}

/**
 * @brief Checks whether a double value is effectively an integer, considering floating-point precision.
 *
 * @param value Value to check
 * @param epsilon Allowed tolerance (default 1e-9)
 * @return true if val is an integer within tolerance
 */
bool ExpressionValidator::isInteger(double value, double epsilon) {
    double nearest = std::round(value);               // Nearest integer
    return std::abs(value - nearest) < epsilon;       // Gap < epsilon => Considered as integer
}

/**
 * @brief Filter candidate expressions according to current constraints.
 *
 * @param candidates List of candidate expressions to filter
 * @param constraintsMap Current constraints mapping character -> Constraint
 * @return Filtered list of candidates satisfying all constraints
 */
std::vector<std::string> ExpressionValidator::filterExpressions(
    const std::vector<std::string>& candidatesList,
    const std::unordered_map<char, Constraint>& constraintsMap
) {
    std::vector<std::string> filteredCandidatesList;

    for (auto& c : candidatesList) {
        if (ConstraintUtils::isCandidateValid(c, constraintsMap))
            filteredCandidatesList.push_back(c);
    }

    return filteredCandidatesList;
}
