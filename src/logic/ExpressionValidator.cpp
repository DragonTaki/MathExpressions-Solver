/* ----- ----- ----- ----- */
// ExpressionValidator.cpp
// Do not distribute or modify
// Author: DragonTaki (https://github.com/DragonTaki)
// Create Date: 2025/10/01
// Update Date: 2025/10/01
// Version: v1.0
/* ----- ----- ----- ----- */

#include <optional>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "ExpressionValidator.h"

using namespace std;

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
     * @return long long Result of the operation
     * @throws runtime_error On invalid operator, division by zero, or negative exponent
     */
    long long applyOp(long long a, long long b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/':
                if (b == 0) throw runtime_error("Division by zero");
                return a / b;
            case '^': {
                if (b < 0) throw runtime_error("Negative exponent not supported");
                long long res = 1;
                for (int i = 0; i < b; i++) res *= a;
                return res;
            }
        }
        throw runtime_error("Invalid operator");
    }
} // anonymous namespace

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
 * @param s Expression string (e.g., "12+3*4")
 * @return long long Evaluation result
 * @throws runtime_error If expression contains invalid characters, malformed RPN, or illegal operations
 */
long long ExpressionValidator::evalExpr(const string& s) {
    vector<string> output;   // RPN output queue
    stack<char> opsStack;    // Operator stack
    string num;              // Buffer for multi-digit numbers

    auto flushNum = [&]() {
        if (!num.empty()) {
            output.push_back(num);
            num.clear();
        }
    };

    // --- Shunting Yard Algorithm ---
    for (char c : s) {
        if (isdigit(c)) {
            num.push_back(c);
        } else if (validOps.count(c)) {
            flushNum();
            while (!opsStack.empty() && validOps.count(opsStack.top())) {
                char top = opsStack.top();
                if ((isLeftAssociative(c) && precedence(c) <= precedence(top)) ||
                    (!isLeftAssociative(c) && precedence(c) < precedence(top))) {
                    output.push_back(string(1, top));
                    opsStack.pop();
                } else break;
            }
            opsStack.push(c);
        } else {
            throw runtime_error("Invalid character in expression");
        }
    }
    flushNum();
    while (!opsStack.empty()) {
        output.push_back(string(1, opsStack.top()));
        opsStack.pop();
    }

    // --- RPN Evaluation ---
    stack<long long> st;
    for (auto& token : output) {
        if (isdigit(token[0])) {
            st.push(stoll(token));
        } else {
            if (st.size() < 2) throw runtime_error("Malformed expression");
            long long b = st.top(); st.pop();
            long long a = st.top(); st.pop();
            st.push(applyOp(a, b, token[0]));
        }
    }
    if (st.size() != 1) throw runtime_error("Malformed RPN eval");
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
 * @param expr Expression string
 * @return std::optional<long long> Evaluation result if successful; std::nullopt otherwise
 */
std::optional<long long> ExpressionValidator::safeEval(const std::string& expr) {
    try {
        ExpressionValidator validator;
        long long res = validator.evalExpr(expr);
        return res;
    } catch (...) {
        return std::nullopt;
    }
}

// ---- Public API ----

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
 * @param s Expression string (e.g., "12+35=47")
 * @param n Expected length of expression
 * @return true if valid equation; false otherwise
 */
bool ExpressionValidator::isValidExpression(const string& s, int n) {
    if ((int)s.size() != n) return false;

    size_t eqPos = s.find('=');
    if (eqPos == string::npos || s.find('=', eqPos+1) != string::npos)
        return false;

    string left = s.substr(0, eqPos);
    string right = s.substr(eqPos+1);
    if (left.empty() || right.empty()) return false;

    try {
        long long lv = evalExpr(left);
        long long rv = evalExpr(right);
        return lv == rv;
    } catch (...) {
        return false;
    }
}
